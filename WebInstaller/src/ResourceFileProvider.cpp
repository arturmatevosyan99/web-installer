/// @file ResourceFileProvider.cpp
/// Extracts embedded exe from RCDATA resource to temp file.
#define WIN32_LEAN_AND_MEAN
#include "webinstaller/ResourceFileProvider.h"
#include "resource.h"
#include <windows.h>

namespace webinstaller {

bool ResourceFileProvider::GetFile(std::wstring& outPath) {
    HMODULE hModule = GetModuleHandleW(nullptr);
    HRSRC hRes = FindResourceW(hModule, MAKEINTRESOURCEW(IDR_EMBEDDED_FILE), RT_RCDATA);
    if (!hRes)
        return false;
    HGLOBAL hData = LoadResource(hModule, hRes);
    if (!hData)
        return false;
    void* pData = LockResource(hData);
    DWORD size = SizeofResource(hModule, hRes);
    if (!pData || size == 0)
        return false;

    // Write resource to temp .exe file
    wchar_t tempPath[MAX_PATH];
    GetTempPathW(MAX_PATH, tempPath);
    wchar_t tempFile[MAX_PATH];
    GetTempFileNameW(tempPath, L"WI", 0, tempFile);
    DeleteFileW(tempFile);
    wcscat_s(tempFile, L".exe");
    outPath = tempFile;

    HANDLE hFile = CreateFileW(tempFile, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE)
        return false;
    DWORD written;
    WriteFile(hFile, pData, size, &written, nullptr);
    CloseHandle(hFile);
    return written == size;
}

}  // namespace webinstaller
