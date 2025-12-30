@echo off
if not "%~2" == "" (
    echo !Just one argument, the directory name, please. 1>&2
    exit /b 1
)

if "%~1" == "" (
    echo !Please specify the directory name as argument. 1>&2
    exit /b 2
)

md %1 && cd %1
