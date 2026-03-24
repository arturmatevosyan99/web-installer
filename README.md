# Тестовое задание: Web Installer + тестовый стенд

Структура репозитория:

| Каталог | Описание |
|---------|----------|
| `WebInstaller/` | Клиент на C++ (Visual Studio 2022): GUI, WinINet, elevation, отчёт на сервер, fallback из ресурсов |
| `FilesPServ/` | Тестовый сервер (Python / FastAPI), SQLite, выдача `file.exe`, приём отчётов |
| `task.txt` | Текст задания (для справки) |

## Быстрый старт

1. **Сервер:** `FilesPServ/InitEnv.bat` → `FilesPServ/StartServer.bat`
2. **Клиент:** собрать `WebInstaller/WebInstaller.sln` (Release, x64 или Win32), запустить exe, указать URL (например `http://127.0.0.1:8000`).

Подробности: `WebInstaller/README.md`, `FilesPServ/README.md`.

## Статистика по запускам (требование задания)

После накопления записей в БД:

```bat
cd FilesPServ
py -m scripts.calc_stats
```

Создаётся `stats_results.csv` с процентами по elevation и успешным запускам.

## Git: отправка на удалённый репозиторий

После первого коммита (уже сделан в этом шаблоне — выполните у себя):

```bat
cd путь\к\TestProject
git remote add origin https://github.com/ВАШ_ЛОГИН/ВАШ_РЕПО.git
git branch -M main
git push -u origin main
```

Замените URL на свой (GitHub, GitLab и т.д.).
