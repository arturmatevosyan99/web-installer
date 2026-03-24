/// @file Utils.cpp
/// String conversion and URL parsing helpers.
#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "webinstaller/Utils.h"
#include <ctime>
#include <cwchar>

namespace webinstaller {

std::wstring GetTimestamp() {
    time_t now = time(nullptr);
    struct tm t;
    localtime_s(&t, &now);
    wchar_t buf[64];
    wcsftime(buf, sizeof(buf) / sizeof(wchar_t), L"%Y-%m-%dT%H:%M:%S", &t);
    return buf;
}

std::string WideToUtf8(const std::wstring& w) {
    if (w.empty())
        return "";
    int size = WideCharToMultiByte(CP_UTF8, 0, w.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string s(size - 1, 0);
    WideCharToMultiByte(CP_UTF8, 0, w.c_str(), -1, &s[0], size, nullptr, nullptr);
    return s;
}

std::wstring Utf8ToWide(const std::string& s) {
    if (s.empty())
        return L"";
    int size = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, nullptr, 0);
    std::wstring w(size - 1, 0);
    MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, &w[0], size);
    return w;
}

bool ParseUrl(const std::wstring& url, std::wstring& host, std::wstring& path, int& port, bool& https) {
    std::wstring u = url;
    https = false;
    port = 80;
    if (u.find(L"https://") == 0) {
        https = true;
        port = 443;
        u = u.substr(8);
    } else if (u.find(L"http://") == 0) {
        u = u.substr(7);
    }
    size_t slash = u.find(L'/');
    std::wstring hostPort = (slash != std::wstring::npos) ? u.substr(0, slash) : u;
    path = (slash != std::wstring::npos) ? u.substr(slash) : L"/";
    if (path.empty())
        path = L"/";
    size_t colon = hostPort.find(L':');
    if (colon != std::wstring::npos) {
        host = hostPort.substr(0, colon);
        port = _wtoi(hostPort.substr(colon + 1).c_str());
        if (port <= 0)
            port = https ? 443 : 80;
    } else {
        host = hostPort;
    }
    return !host.empty();
}

std::wstring NormalizeHost(const std::wstring& host) {
    if (_wcsicmp(host.c_str(), L"localhost") == 0)
        return L"127.0.0.1";
    return host;
}

}  // namespace webinstaller
