#pragma once

#include <string>

namespace webinstaller {

/// Abstract interface for obtaining the executable file.
class IFileProvider {
public:
    virtual ~IFileProvider() = default;

    /// Extract/copy file to outPath. Returns true on success.
    virtual bool GetFile(std::wstring& outPath) = 0;
};

}  // namespace webinstaller
