@echo off
for /f "tokens=2,*" %%m in ('net user %username% ^| find ^"Full Name^"') do @set _fullname=%%n
echo Hello %_fullname%...
echo Greetings from some dirty batch file code!
