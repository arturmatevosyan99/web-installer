#pragma once

#include "IFileProvider.h"

namespace webinstaller {

/// Extracts embedded file from executable resources.
class ResourceFileProvider : public IFileProvider {
public:
    bool GetFile(std::wstring& outPath) override;
};

}  // namespace webinstaller
