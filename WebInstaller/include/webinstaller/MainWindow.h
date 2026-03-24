#pragma once

#include <string>
#include <windows.h>
#include "IProgressCallback.h"
#include "resource.h"

struct HWND__;

namespace webinstaller {

/// Main application window. Implements IProgressCallback for UI updates.
class MainWindow : public IProgressCallback {
public:
    MainWindow();
    ~MainWindow();

    /// Create and show window. @param nCmdShow SW_* show flag. @return false on failure.
    bool Create(int nCmdShow);
    /// Run message loop until WM_QUIT. @return exit code.
    int MessageLoop();
    void OnProgress(int percent) override;
    void OnStatus(const wchar_t* text) override;

    HWND__* GetHwnd() const { return m_hWnd; }
    bool IsWorkerRunning() const { return m_workerRunning; }
    void SetWorkerRunning(bool v) { m_workerRunning = v; }
    void SetStartTime(const std::wstring& t) { m_startTime = t; }
    void StartWorker(const std::wstring& baseUrl);
    void OnWorkerDone();
    void OnWorkerError(const std::wstring& error);
    void SetHwnd(void* hwnd) { m_hWnd = reinterpret_cast<HWND__*>(hwnd); }

private:
    static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    LRESULT HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    HWND__* m_hWnd = nullptr;
    HWND__* m_hEditUrl = nullptr;
    HWND__* m_hBtnGo = nullptr;
    HWND__* m_hProgress = nullptr;
    HWND__* m_hStatus = nullptr;
    std::wstring m_startTime;
    volatile bool m_workerRunning = false;
};

}  // namespace webinstaller
