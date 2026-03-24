/// @file main.cpp
/// Web Installer entry point. Handles UAC elevation, then shows main window.
#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include "webinstaller/MainWindow.h"
#include "webinstaller/ElevationManager.h"
#include "webinstaller/Utils.h"

int WINAPI wWinMain(HINSTANCE, HINSTANCE, PWSTR, int nCmdShow) {
    std::wstring startTime = webinstaller::GetTimestamp();
    bool useWinINet = true;  // --curl switches to curl backend if implemented
    for (int i = 1; i < __argc; i++) {
        if (_wcsicmp(__wargv[i], L"--curl") == 0) 
            useWinINet = false;
    }

    // Try UAC elevation if not already elevated
    if (!webinstaller::ElevationManager::IsElevated()) {
        if (webinstaller::ElevationManager::TryElevate(__argc, __wargv)) {
            return 0;
        }
        webinstaller::ElevationManager::SetResult(L"denied");
    } else {
        webinstaller::ElevationManager::SetResult(L"granted");
    }

    webinstaller::MainWindow window;
    window.SetStartTime(startTime);
    if (!window.Create(nCmdShow)) return 1;

    return window.MessageLoop();
}
