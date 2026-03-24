#pragma once

#include <string>
#include "types.h"
#include "IHttpClient.h"
#include "IFileProvider.h"
#include "IProgressCallback.h"

namespace webinstaller {

/// Orchestrates download, launch, fallback, and report.
class InstallerController {
public:
    InstallerController(
        IHttpClient* httpClient,
        IFileProvider* fallbackProvider,
        IProgressCallback* callback,
        const std::wstring& startTime,
        const std::wstring& elevationResult,
        bool useWinINet);

    /// Run full flow: download -> launch -> report. Returns (success, errorMessage).
    std::pair<bool, std::wstring> Run(const std::wstring& baseUrl);

private:
    IHttpClient* m_httpClient;
    IFileProvider* m_fallbackProvider;
    IProgressCallback* m_callback;
    std::wstring m_startTime;
    std::wstring m_elevationResult;
    bool m_useWinINet;
};

}  // namespace webinstaller
