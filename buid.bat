@echo off
echo ====== Building StoreMadeByPD ======
gcc storemadebypd.c unittest.c e2etest.c -o storemadebypd

if %errorlevel% neq 0 (
    echo Build failed! Please check your code.
    pause
    exit /b
)

echo Build success! Running program...
echo =====================================
storemadebypd
pause