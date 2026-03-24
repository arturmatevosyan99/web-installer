#pragma once

#include <string>
#include "types.h"
#include "IProgressCallback.h"

namespace webinstaller {

/// Abstract HTTP client interface.
class IHttpClient {
public:
    virtual ~IHttpClient() = default;

    /// Download file from baseUrl/file. Returns path on success.
    virtual DownloadResult Download(const std::wstring& baseUrl, IProgressCallback* callback) = 0;

    /// POST report to baseUrl/report.
    virtual ReportResult PostReport(
        const std::wstring& baseUrl,
        const std::wstring& startTime,
        const std::wstring& mode,
        const std::wstring& elevationResult,
        const std::wstring& launchResult) = 0;
};

}  // namespace webinstaller
