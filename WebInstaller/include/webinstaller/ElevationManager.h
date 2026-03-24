#pragma once

namespace webinstaller {

/// Manages UAC elevation.
class ElevationManager {
public:
    /// Check if process is elevated.
    static bool IsElevated();

    /// Attempt to restart with elevation. Returns true if elevated process was started.
    static bool TryElevate(int argc, wchar_t** argv);

    /** Get result: "granted" or "denied". */
    static const wchar_t* GetResult();

    /// Set result (called after elevation attempt).
    static void SetResult(const wchar_t* result);
};

}  // namespace webinstaller
