@echo off
set _vs_dir=C:\Program Files\Microsoft Visual Studio\18\Community
call "%_vs_dir%\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 (
    echo vcvars64.bat failed.
) else (
    echo Set up a Visual C++ x64 environment.
)
