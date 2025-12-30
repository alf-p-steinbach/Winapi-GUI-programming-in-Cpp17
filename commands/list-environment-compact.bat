@echo off & setlocal enabledelayedexpansion
set s=
for /f "delims==" %%v in ('set') do (
    if not "!s!"=="" (set s=!s!, )
    set s=!s!%%v
)
echo %s%
