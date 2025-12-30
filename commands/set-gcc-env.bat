@echo off

set GCC-PATH=r:\installed\msys2\ucrt64\bin
set PATH=%PATH%;%GCC-PATH%
g++ --version | find "++"

set GCC-OPT=-std=c++17 -pedantic-errors -Wall -Wextra -Wconversion -Wno-missing-field-initializers
doskey g=g++ %%GCC-OPT%% $*

echo Hopefully set up a g++ environment ^(common options in `%%GCC-OPT%%`, alias `g` uses that^).
