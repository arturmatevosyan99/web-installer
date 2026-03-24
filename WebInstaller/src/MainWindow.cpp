/// @file MainWindow.cpp
/// Main window UI and worker thread coordination.
#define WIN32_LEAN_AND_MEAN
#include "webinstaller/MainWindow.h"
#include "webinstaller/Utils.h"
#include "webinstaller/InstallerController.h"
#include "webinstaller/WinINetHttpClient.h"
#include "webinstaller/ResourceFileProvider.h"
#include "webinstaller/ElevationManager.h"
#include "webinstaller/Config.h"
#include "resource.h"
#include <windows.h>
#include <commctrl.h>
#include <shellapi.h>

#pragma comment(lib, "comctl32.lib")

namespace {

/// Parameters passed to background worker thread.
struct WorkerParams {
    webinstaller::MainWindow* mainWnd;
    std::wstring baseUrl;
    std::wstring startTime;
    std::wstring elevationResult;
    bool useWinINet;
};

DWORD WINAPI WorkerThread(LPVOID param) {
    auto* p = static_cast<WorkerParams*>(param);
    webinstaller::MainWindow* mainWnd = p->mainWnd;
    std::wstring baseUrl = p->baseUrl;
    std::wstring startTime = p->startTime;
    std::wstring elevationResult = p->elevationResult;
    bool useWinINet = p->useWinINet;
    delete p;

    webinstaller::WinINetHttpClient httpClient;
    webinstaller::ResourceFileProvider fallbackProvider;
    if (!mainWnd)
        return 0;

    webinstaller::InstallerController controller(
        &httpClient, &fallbackProvider, mainWnd,
        startTime, elevationResult, useWinINet);

    auto result = controller.Run(baseUrl);
    bool ok = result.first;
    std::wstring errMsg = result.second;
    if (ok) {
        PostMessageW(reinterpret_cast<HWND>(mainWnd->GetHwnd()), WM_WORKER_DONE, 0, 0);
    } else {
        std::wstring* err = new std::wstring(errMsg);
        PostMessageW(reinterpret_cast<HWND>(mainWnd->GetHwnd()), WM_WORKER_ERROR, 0, reinterpret_cast<LPARAM>(err));
    }
    mainWnd->SetWorkerRunning(false);
    return 0;
}

}  // namespace

namespace webinstaller {

MainWindow::MainWindow() = default;

MainWindow::~MainWindow() = default;

void MainWindow::OnProgress(int percent) {
    if (m_hProgress) {
        SendMessageW(reinterpret_cast<HWND>(m_hProgress), PBM_SETPOS, percent, 0);
    }
}

void MainWindow::OnStatus(const wchar_t* text) {
    if (m_hStatus) {
        SetWindowTextW(reinterpret_cast<HWND>(m_hStatus), text);
    }
}



void MainWindow::StartWorker(const std::wstring& baseUrl) {
    auto* p = new WorkerParams;
    p->mainWnd = this;
    p->baseUrl = baseUrl;
    p->startTime = m_startTime.empty() ? webinstaller::GetTimestamp() : m_startTime;
    p->elevationResult = ElevationManager::GetResult();
    p->useWinINet = true;
    for (int i = 1; i < __argc; i++) {
        if (_wcsicmp(__wargv[i], L"--curl") == 0)
            p->useWinINet = false;
    }
    HANDLE hThread = CreateThread(nullptr, 0, WorkerThread, p, 0, nullptr);
    if (hThread)
        CloseHandle(hThread);
}

void MainWindow::OnWorkerDone() {
    m_workerRunning = false;
    OnStatus(L"Done");
}

void MainWindow::OnWorkerError(const std::wstring& error) {
    m_workerRunning = false;
    MessageBoxW(reinterpret_cast<HWND>(m_hWnd), error.c_str(), L"Error", MB_OK | MB_ICONERROR);
    OnStatus(L"Error");
}

LRESULT CALLBACK MainWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    MainWindow* wnd = nullptr;
    if (msg == WM_NCCREATE) {
        auto* cs = reinterpret_cast<CREATESTRUCTW*>(lParam);
        wnd = static_cast<MainWindow*>(cs->lpCreateParams);
        wnd->SetHwnd(hWnd);
        SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(wnd));
    } else {
        wnd = reinterpret_cast<MainWindow*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));
    }
    if (wnd)
        return wnd->HandleMessage(hWnd, msg, wParam, lParam);
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

LRESULT MainWindow::HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE: {
        INITCOMMONCONTROLSEX icc = { sizeof(icc), ICC_PROGRESS_CLASS };
        InitCommonControlsEx(&icc);

        m_hEditUrl = reinterpret_cast<HWND__*>(CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", Config::DEFAULT_URL,
            WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, 10, 10, 380, 24, reinterpret_cast<HWND>(m_hWnd),
            reinterpret_cast<HMENU>(static_cast<INT_PTR>(IDC_EDIT_URL)), nullptr, nullptr));
        m_hBtnGo = reinterpret_cast<HWND__*>(CreateWindowExW(0, L"BUTTON", L"Download and Run",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 10, 44, 150, 28, reinterpret_cast<HWND>(m_hWnd),
            reinterpret_cast<HMENU>(static_cast<INT_PTR>(IDC_BTN_GO)), nullptr, nullptr));
        m_hProgress = reinterpret_cast<HWND__*>(CreateWindowExW(0, PROGRESS_CLASSW, nullptr,
            WS_CHILD | WS_VISIBLE | PBS_SMOOTH, 10, 82, 380, 24, reinterpret_cast<HWND>(m_hWnd),
            reinterpret_cast<HMENU>(static_cast<INT_PTR>(IDC_PROGRESS)), nullptr, nullptr));
        m_hStatus = reinterpret_cast<HWND__*>(CreateWindowExW(0, L"STATIC", L"Ready",
            WS_CHILD | WS_VISIBLE, 10, 116, 380, 20, reinterpret_cast<HWND>(m_hWnd),
            reinterpret_cast<HMENU>(static_cast<INT_PTR>(IDC_STATIC_STATUS)), nullptr, nullptr));
        SendMessageW(reinterpret_cast<HWND>(m_hProgress), PBM_SETRANGE, 0, MAKELPARAM(0, 100));
        break;
    }
    case WM_COMMAND:
        if (LOWORD(wParam) == IDC_BTN_GO && HIWORD(wParam) == BN_CLICKED) {
            if (m_workerRunning) break;
            wchar_t buf[512];
            GetWindowTextW(reinterpret_cast<HWND>(m_hEditUrl), buf, 512);
            std::wstring url = buf;
            while (!url.empty() && url.back() == L' ')
                url.pop_back();
            if (url.empty()) {
                OnStatus(L"Enter server URL");
                break;
            }
            if (url.back() == L'/')
                url.pop_back();
            m_workerRunning = true;
            SendMessageW(reinterpret_cast<HWND>(m_hProgress), PBM_SETPOS, 0, 0);
            OnStatus(L"Starting...");
            StartWorker(url);
        }
        break;
    case WM_WORKER_DONE:
        OnWorkerDone();
        break;
    case WM_WORKER_ERROR: {
        std::wstring* pErr = reinterpret_cast<std::wstring*>(lParam);
        OnWorkerError(*pErr);
        delete pErr;
        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProcW(hWnd, msg, wParam, lParam);
    }
    return 0;
}

bool MainWindow::Create(int nCmdShow) {
    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(wc);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = reinterpret_cast<HBRUSH>(static_cast<LONG_PTR>(COLOR_WINDOW + 1));
    wc.lpszClassName = L"WebInstallerClass";
    RegisterClassExW(&wc);

    m_hWnd = reinterpret_cast<HWND__*>(CreateWindowExW(0, L"WebInstallerClass", L"Web Installer",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
        CW_USEDEFAULT, CW_USEDEFAULT, 420, 200, nullptr, nullptr,
        GetModuleHandleW(nullptr), this));
    if (!m_hWnd)
        return false;

    ShowWindow(reinterpret_cast<HWND>(m_hWnd), nCmdShow);
    return true;
}

int MainWindow::MessageLoop() {
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return static_cast<int>(msg.wParam);
}

}  // namespace webinstaller
