/* Minimal valid Windows executable for Web Installer testing */
#include <windows.h>

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    MessageBoxW(NULL, L"Download and launch successful!\n\nWeb Installer test.", L"Web Installer Test", MB_OK | MB_ICONINFORMATION);
    return 0;
}
