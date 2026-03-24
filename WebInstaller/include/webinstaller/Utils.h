#pragma once

#include <string>

namespace webinstaller {

/// @return ISO 8601 timestamp (e.g. 2025-03-19T12:00:00)
std::wstring GetTimestamp();

/// @param w Wide string to convert
/// @return UTF-8 encoded string
std::string WideToUtf8(const std::wstring& w);

/// @param s UTF-8 string to convert
/// @return Wide string
std::wstring Utf8ToWide(const std::string& s);

/** Parse http(s)://host[:port]/path into components.
 *  @return true if host is non-empty */
bool ParseUrl(const std::wstring& url, std::wstring& host, std::wstring& path, int& port, bool& https);

/// Maps localhost to 127.0.0.1 for WinINet compatibility.
std::wstring NormalizeHost(const std::wstring& host);

}  // namespace webinstaller
