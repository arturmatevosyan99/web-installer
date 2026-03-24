#pragma once

namespace webinstaller {

/// Application configuration constants.
namespace Config {
    constexpr int NETWORK_TIMEOUT_MS = 30000;
    constexpr int RETRY_DELAY_MS = 2000;
    constexpr int PROGRESS_BUFFER_SIZE = 8192;
    constexpr wchar_t DEFAULT_URL[] = L"http://localhost:8000";
    constexpr wchar_t USER_AGENT[] = L"WebInstaller/1.0";
}

}  // namespace webinstaller
