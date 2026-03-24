#pragma once

#include "IHttpClient.h"

namespace webinstaller {

/// WinINet-based HTTP client implementation.
class WinINetHttpClient : public IHttpClient {
public:
    DownloadResult Download(const std::wstring& baseUrl, IProgressCallback* callback) override;
    ReportResult PostReport(
        const std::wstring& baseUrl,
        const std::wstring& startTime,
        const std::wstring& mode,
        const std::wstring& elevationResult,
        const std::wstring& launchResult) override;
};

}  // namespace webinstaller
