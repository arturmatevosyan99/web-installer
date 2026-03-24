# FilesPServ — тестовый стенд

## Запуск

1. `InitEnv.bat` — создаёт venv и ставит зависимости (`py -m pip install -r requirements.txt`).
2. `StartServer.bat` — запуск сервера (`python -m src.main`).

По умолчанию: `http://0.0.0.0:8000` (клиенту указывать `http://127.0.0.1:8000`).

## API

| Метод | Путь | Описание |
|-------|------|----------|
| GET | `/file` | Скачать `file/file.exe` (при ошибке по настройкам — 404) |
| POST | `/report` | JSON: `start_time`, `mode`, `elevation_result`, `launch_result` |

Настройки через переменные окружения с префиксом `FILESPSERV_` (см. `src/config/settings.py`).

## Статистика

Из каталога `FilesPServ`:

```bat
py -m scripts.calc_stats
```

Вывод в консоль и файл `stats_results.csv` (не коммитится).

## Файл для раздачи

Положите исполняемый файл в `file/file.exe`. Клиент WebInstaller при сборке может встраивать тот же файл в ресурсы (см. `WebInstaller/resource.rc`).
