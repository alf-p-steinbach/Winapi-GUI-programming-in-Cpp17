@echo off
(g++ --version >nul 2>nul) && (
    echo g++ is already available; doing nothing.
    exit /b 1
)

(type r:\nul >nul) || (
    echo No drive "r:"; please define it and run "%0" again.
    exit /b 1
)

set GCC-PATH=r:\installed\msys2\ucrt64\bin
(%GCC-PATH%\g++ --version >nul 2>nul) || (
    echo g++ not found in "%GCC-PATH%" or it was not executable.
    exit /b 1
)

set ORIGINALPATH=%PATH%
set PATH=%PATH%;%GCC-PATH%
(g++ --version | find "++") || (
    echo After `PATH` mod: unqualified `g++` not found or it was not executable.
    set PATH=%ORIGINALPATH%
    exit /b 1
)

set GCC-OPT=-std=c++17 -pedantic-errors -Wall -Wextra -Wconversion -Wno-missing-field-initializers
doskey g=g++ %%GCC-OPT%% $*

echo Successfully set up g++ environment ^(common options in %%GCC-OPT%%, alias g uses that^).
