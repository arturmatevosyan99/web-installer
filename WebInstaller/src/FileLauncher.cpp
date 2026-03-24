/// @file FileLauncher.cpp
/// Launches exe via ShellExecuteEx.
#define WIN32_LEAN_AND_MEAN
#include "webinstaller/FileLauncher.h"
#include <windows.h>
#include <shellapi.h>

#pragma comment(lib, "shell32.lib")

namespace webinstaller {

bool FileLauncher::Launch(const std::wstring& path) {
    SHELLEXECUTEINFOW sei = {};
    sei.cbSize = sizeof(sei);
    sei.fMask = SEE_MASK_NOCLOSEPROCESS;
    sei.lpVerb = L"open";
    sei.lpFile = path.c_str();
    sei.nShow = SW_SHOW;
    return ShellExecuteExW(&sei) != FALSE;
}

}  // namespace webinstaller
