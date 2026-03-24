# Web Installer (C++ Client)

Web-installer клиент для тестового задания. Скачивает файл с сервера и запускает его.

## Требования

- Visual Studio 2022
- Windows 10/11 (x86 или x64)

## Сборка

1. Открыть `WebInstaller.sln` в Visual Studio 2022
2. Выбрать конфигурацию: Release, платформа Win32 (x86) или x64
3. Сборка → Собрать решение (Ctrl+Shift+B)

Результат: `bin\Win32\Release\WebInstaller.exe` или `bin\x64\Release\WebInstaller.exe`

## Запуск

1. Запустить сервер FilesPServ (см. `../FilesPServ/README.md`)
2. Запустить WebInstaller.exe
3. Ввести URL сервера (по умолчанию http://localhost:8000)
4. Нажать "Download and Run"

## Режимы

- `--wininet` (по умолчанию) — сеть через WinINet
- `--curl` — зарезервировано; фактически сеть через WinINet (libcurl из исходников — вне объёма текущей сдачи)

## Elevation

При запуске без прав администратора приложение запросит elevation. При отказе продолжит работу без прав.

## Ресурсы

Файл `file.exe` встроен в exe как fallback при ошибке скачивания. Путь к нему в resource.rc: `..\FilesPServ\file\file.exe`.
