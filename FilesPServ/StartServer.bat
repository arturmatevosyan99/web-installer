@echo off
cd /d "%~dp0"
filepserv_env\Scripts\activate.bat & py -m pip install -r requirements.txt & "%~dp0filepserv_env\Scripts\python.exe" -m src.main