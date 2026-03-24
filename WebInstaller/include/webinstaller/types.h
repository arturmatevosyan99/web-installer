#pragma once

#include <string>

namespace webinstaller {

/// @brief Result type for operations that can fail.
template <typename T>
struct Result {
    bool ok = false;
    T value{};
    std::wstring error;

    static Result Success(T v) {
        Result r;
        r.ok = true;
        r.value = std::move(v);
        return r;
    }
    static Result Fail(std::wstring err) {
        Result r;
        r.ok = false;
        r.error = std::move(err);
        return r;
    }
};

/// @brief Download operation result.
struct DownloadResult {
    bool success = false;
    bool networkError = false;
    std::wstring filePath;
    std::wstring errorMessage;
};

/// @brief Report POST result.
struct ReportResult {
    bool success = false;
    bool networkError = false;
};

}  // namespace webinstaller
