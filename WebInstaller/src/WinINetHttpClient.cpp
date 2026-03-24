/// @file WinINetHttpClient.cpp
/// WinINet-based HTTP client for download and report POST.
#define WIN32_LEAN_AND_MEAN
#include "webinstaller/WinINetHttpClient.h"
#include "webinstaller/Utils.h"
#include "webinstaller/Config.h"
#include "resource.h"
#include <windows.h>
#include <wininet.h>

#pragma comment(lib, "wininet.lib")

namespace webinstaller {

DownloadResult WinINetHttpClient::Download(const std::wstring& baseUrl, IProgressCallback* callback) {
    DownloadResult result;
    std::wstring fileUrl = baseUrl;
    // Resolve file endpoint (baseUrl/file or baseUrl + /file)
    if (fileUrl.back() == L'/')
        fileUrl += L"file";
    else
        fileUrl += L"/file";

    std::wstring host, path;
    int port;
    bool https;
    if (!ParseUrl(fileUrl, host, path, port, https)) {
        result.errorMessage = L"Invalid URL";
        return result;
    }
    host = NormalizeHost(host);

    HINTERNET hInternet = InternetOpenW(Config::USER_AGENT, INTERNET_OPEN_TYPE_PRECONFIG, nullptr, nullptr, 0);
    if (!hInternet) {
        result.networkError = true;
        result.errorMessage = L"InternetOpen failed";
        return result;
    }

    DWORD timeout = Config::NETWORK_TIMEOUT_MS;
    InternetSetOptionW(hInternet, INTERNET_OPTION_CONNECT_TIMEOUT, &timeout, sizeof(timeout));
    InternetSetOptionW(hInternet, INTERNET_OPTION_RECEIVE_TIMEOUT, &timeout, sizeof(timeout));

    HINTERNET hConnect = InternetConnectW(hInternet, host.c_str(), static_cast<INTERNET_PORT>(port), nullptr, nullptr, INTERNET_SERVICE_HTTP, 0, 0);
    if (!hConnect) {
        InternetCloseHandle(hInternet);
        result.networkError = true;
        result.errorMessage = L"InternetConnect failed";
        return result;
    }

    DWORD flags = INTERNET_FLAG_RELOAD;
    if (https)
        flags |= INTERNET_FLAG_SECURE;
    HINTERNET hRequest = HttpOpenRequestW(hConnect, L"GET", path.c_str(), nullptr, nullptr, nullptr, flags, 0);
    if (!hRequest) {
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);
        result.networkError = true;
        result.errorMessage = L"HttpOpenRequest failed";
        return result;
    }

    if (!HttpSendRequestW(hRequest, nullptr, 0, nullptr, 0)) {
        InternetCloseHandle(hRequest);
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);
        result.networkError = true;
        result.errorMessage = L"HttpSendRequest failed";
        return result;
    }

    DWORD statusCode = 0;
    DWORD statusSize = sizeof(statusCode);
    HttpQueryInfoW(hRequest, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &statusCode, &statusSize, nullptr);
    if (statusCode != 200) {
        InternetCloseHandle(hRequest);
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);
        result.errorMessage = L"HTTP error " + std::to_wstring(statusCode);
        return result;
    }

    DWORD contentLength = 0;
    DWORD headerSize = sizeof(contentLength);
    HttpQueryInfoW(hRequest, HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER, &contentLength, &headerSize, nullptr);

    wchar_t tempPath[MAX_PATH];
    GetTempPathW(MAX_PATH, tempPath);
    wchar_t tempFile[MAX_PATH];
    GetTempFileNameW(tempPath, L"WI", 0, tempFile);
    DeleteFileW(tempFile);
    wcscat_s(tempFile, L".exe");
    result.filePath = tempFile;

    HANDLE hFile = CreateFileW(tempFile, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE) {
        InternetCloseHandle(hRequest);
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);
        result.errorMessage = L"CreateFile failed";
        return result;
    }

    DWORD totalRead = 0;
    char buffer[Config::PROGRESS_BUFFER_SIZE];
    DWORD readSize;
    while (InternetReadFile(hRequest, buffer, sizeof(buffer), &readSize) && readSize > 0) {
        DWORD written;
        WriteFile(hFile, buffer, readSize, &written, nullptr);
        totalRead += readSize;
        if (contentLength > 0 && callback) {
            int pct = static_cast<int>((totalRead * 100) / contentLength);
            callback->OnProgress(pct);
        }
    }
    CloseHandle(hFile);
    InternetCloseHandle(hRequest);
    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);

    if (callback)
        callback->OnProgress(100);
    result.success = true;
    return result;
}

ReportResult WinINetHttpClient::PostReport(
    const std::wstring& baseUrl,
    const std::wstring& startTime,
    const std::wstring& mode,
    const std::wstring& elevationResult,
    const std::wstring& launchResult) {
    ReportResult result;
    std::wstring reportUrl = baseUrl;
    if (reportUrl.back() == L'/')
        reportUrl += L"report";
    else
        reportUrl += L"/report";

    std::wstring host, path;
    int port;
    bool https;
    if (!ParseUrl(reportUrl, host, path, port, https))
        return result;
    host = NormalizeHost(host);

    // Build JSON payload for report endpoint
    std::string json = "{\"start_time\":\"" + WideToUtf8(startTime) + "\",\"mode\":\"" + WideToUtf8(mode) +
        "\",\"elevation_result\":\"" + WideToUtf8(elevationResult) + "\",\"launch_result\":\"" + WideToUtf8(launchResult) + "\"}";

    HINTERNET hInternet = InternetOpenW(Config::USER_AGENT, INTERNET_OPEN_TYPE_DIRECT, nullptr, nullptr, 0);
    if (!hInternet) {
        result.networkError = true;
        return result;
    }

    DWORD timeout = Config::NETWORK_TIMEOUT_MS;
    InternetSetOptionW(hInternet, INTERNET_OPTION_CONNECT_TIMEOUT, &timeout, sizeof(timeout));
    InternetSetOptionW(hInternet, INTERNET_OPTION_SEND_TIMEOUT, &timeout, sizeof(timeout));
    InternetSetOptionW(hInternet, INTERNET_OPTION_RECEIVE_TIMEOUT, &timeout, sizeof(timeout));

    HINTERNET hConnect = InternetConnectW(hInternet, host.c_str(), static_cast<INTERNET_PORT>(port), nullptr, nullptr, INTERNET_SERVICE_HTTP, 0, 0);
    if (!hConnect) {
        InternetCloseHandle(hInternet);
        result.networkError = true;
        return result;
    }

    DWORD reqFlags = INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE;
    if (https)
        reqFlags |= INTERNET_FLAG_SECURE;
    HINTERNET hRequest = HttpOpenRequestW(hConnect, L"POST", path.c_str(), nullptr, nullptr, nullptr, reqFlags, 0);
    if (!hRequest) {
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);
        result.networkError = true;
        return result;
    }

    std::wstring headers = L"Content-Type: application/json\r\nContent-Length: " + std::to_wstring(json.size()) + L"\r\nConnection: close\r\n";
    if (!HttpSendRequestW(hRequest, headers.c_str(), static_cast<DWORD>(headers.size()),
            reinterpret_cast<LPVOID>(const_cast<char*>(json.c_str())), static_cast<DWORD>(json.size()))) {
        InternetCloseHandle(hRequest);
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);
        result.networkError = true;
        return result;
    }

    char buf[256];
    DWORD read;
    while (InternetReadFile(hRequest, buf, sizeof(buf), &read) && read > 0) {}
    InternetCloseHandle(hRequest);
    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);
    result.success = true;
    return result;
}

}  // namespace webinstaller
