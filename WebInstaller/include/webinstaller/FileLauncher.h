#pragma once

#include <string>

namespace webinstaller {

/// Launches executable files.
class FileLauncher {
public:
    /// Execute file. Returns true on success.
    static bool Launch(const std::wstring& path);
};

}  // namespace webinstaller
