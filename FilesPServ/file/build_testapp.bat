@echo off
cd /d "%~dp0"
where cl.exe >nul 2>&1
if errorlevel 1 (
    echo Searching for Visual Studio...
    for %%i in ("C:\Program Files\Microsoft Visual Studio\2022\*\VC\Auxiliary\Build\vcvarsall.bat") do (
        if exist "%%i" (call "%%i" x64 & goto :build)
    )
    for %%i in ("C:\Program Files\Microsoft Visual Studio\18\*\VC\Auxiliary\Build\vcvarsall.bat") do (
        if exist "%%i" (call "%%i" x64 & goto :build)
    )
    for %%i in ("C:\Program Files (x86)\Microsoft Visual Studio\2022\*\VC\Auxiliary\Build\vcvarsall.bat") do (
        if exist "%%i" (
            call "%%i" x64
            goto :build
        )
    )
    echo Visual Studio not found. Run from Developer Command Prompt: cl testapp.c /Fe:file.exe user32.lib
    exit /b 1
)
:build
cl.exe testapp.c /Fe:file.exe user32.lib /link /SUBSYSTEM:WINDOWS /ENTRY:wWinMainCRTStartup
if errorlevel 1 exit /b 1
del testapp.obj 2>nul
echo Built file.exe successfully.
exit /b 0
