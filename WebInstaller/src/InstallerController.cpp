/// @file InstallerController.cpp
/// Orchestrates download, fallback, launch, and report with retry logic.
#include "webinstaller/InstallerController.h"
#include <utility>
#include "webinstaller/FileLauncher.h"
#include "webinstaller/Config.h"
#include <windows.h>

namespace webinstaller {

InstallerController::InstallerController(
    IHttpClient* httpClient,
    IFileProvider* fallbackProvider,
    IProgressCallback* callback,
    const std::wstring& startTime,
    const std::wstring& elevationResult,
    bool useWinINet)
    : m_httpClient(httpClient)
    , m_fallbackProvider(fallbackProvider)
    , m_callback(callback)
    , m_startTime(startTime)
    , m_elevationResult(elevationResult)
    , m_useWinINet(useWinINet) {
}

std::pair<bool, std::wstring> InstallerController::Run(const std::wstring& baseUrl) {
    if (m_callback)
        m_callback->OnStatus(L"Downloading...");

    std::wstring filePath;
    auto result = m_httpClient->Download(baseUrl, m_callback);

    // Retry once on network error, then fallback to embedded file
    if (!result.success) {
        if (result.networkError) {
            Sleep(Config::RETRY_DELAY_MS);
            result = m_httpClient->Download(baseUrl, m_callback);
        }
        if (!result.success) {
            if (m_callback)
                m_callback->OnStatus(L"Using embedded file...");
            if (!m_fallbackProvider->GetFile(filePath)) {
                if (m_callback)
                    m_callback->OnStatus(L"Error");
                return {false, L"Download failed and no embedded file"};
            }
        } else {
            filePath = result.filePath;
        }
    } else {
        filePath = result.filePath;
    }

    if (m_callback)
        m_callback->OnStatus(L"Launching...");
    bool launchOk = FileLauncher::Launch(filePath);
    std::wstring launchResult = launchOk ? L"success" : L"failed";

    if (m_callback)
        m_callback->OnStatus(L"Sending report...");
    auto reportResult = m_httpClient->PostReport(
        baseUrl, m_startTime,
        m_useWinINet ? L"wininet" : L"curl",
        m_elevationResult, launchResult);

    if (!reportResult.success && reportResult.networkError) {
        Sleep(Config::RETRY_DELAY_MS);
        reportResult = m_httpClient->PostReport(
            baseUrl, m_startTime,
            m_useWinINet ? L"wininet" : L"curl",
            m_elevationResult, launchResult);
    }

    if (!reportResult.success && reportResult.networkError) {
        if (m_callback)
            m_callback->OnStatus(L"Error");
        return {false, L"Network error sending report. Exiting."};
    }

    if (m_callback)
        m_callback->OnStatus(L"Done");
    return {true, L""};
}

}  // namespace webinstaller
