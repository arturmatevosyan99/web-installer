#pragma once

namespace webinstaller {

/// Abstract interface for progress and status updates (UI thread-safe).
class IProgressCallback {
public:
    virtual ~IProgressCallback() = default;
    virtual void OnProgress(int percent) = 0;
    virtual void OnStatus(const wchar_t* text) = 0;
};

}  // namespace webinstaller
