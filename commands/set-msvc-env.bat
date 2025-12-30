@echo off
set _vs_dir=C:\Program Files\Microsoft Visual Studio\18\Community
call "%_vs_dir%\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 (
    echo vcvars64.bat failed.
) else (
    set CL=^
/nologo /utf-8 /EHsc /GR /permissive- /std:c++17 /Zc:__cplusplus /Zc:preprocessor /W4 /wd4459 ^
/D _CRT_SECURE_NO_WARNINGS=1 /D _STL_SECURE_NO_WARNINGS=1
    set LINK=/entry:mainCRTStartup
   
    echo Set up a Visual C++ x64 environment supporting standard `main`.
)
