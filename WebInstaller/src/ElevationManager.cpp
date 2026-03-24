/// @file ElevationManager.cpp
/// UAC elevation via ShellExecute runas.
#define WIN32_LEAN_AND_MEAN
#include "webinstaller/ElevationManager.h"
#include <windows.h>
#include <shellapi.h>
#include <string>

#pragma comment(lib, "shell32.lib")

namespace webinstaller {

/// "granted" or "denied" - set after elevation attempt.
static thread_local const wchar_t* g_result = L"denied";

bool ElevationManager::IsElevated() {
    BOOL elevated = FALSE;
    HANDLE hToken = nullptr;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
        TOKEN_ELEVATION te;
        DWORD size;
        if (GetTokenInformation(hToken, TokenElevation, &te, sizeof(te), &size))
            elevated = te.TokenIsElevated;
        CloseHandle(hToken);
    }
    return elevated != FALSE;
}

bool ElevationManager::TryElevate(int argc, wchar_t** argv) {
    wchar_t path[MAX_PATH];
    GetModuleFileNameW(nullptr, path, MAX_PATH);
    wchar_t dir[MAX_PATH];
    wcscpy_s(dir, path);
    for (int i = static_cast<int>(wcslen(dir)) - 1; i >= 0; i--) {
        if (dir[i] == L'\\') {
            dir[i] = 0;
            break;
        }
    }
    SHELLEXECUTEINFOW sei = {};
    sei.cbSize = sizeof(sei);
    sei.lpVerb = L"runas";
    sei.lpFile = path;
    sei.lpDirectory = dir;
    sei.nShow = SW_SHOW;
    std::wstring args;
    for (int i = 1; i < argc; i++) {
        if (i > 1)
            args += L" ";
        args += argv[i];
    }
    if (args.empty())
        args = L"--wininet";
    sei.lpParameters = args.c_str();
    return ShellExecuteExW(&sei) != FALSE;
}

const wchar_t* ElevationManager::GetResult() {
    return g_result;
}

void ElevationManager::SetResult(const wchar_t* result) {
    g_result = result;
}

}  // namespace webinstaller
