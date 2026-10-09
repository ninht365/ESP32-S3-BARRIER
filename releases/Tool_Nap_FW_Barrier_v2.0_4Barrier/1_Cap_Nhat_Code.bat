@echo off
chcp 65001 > nul
title CONG CU CAP NHAT FIRMWARE BARRIER V2.0 (4 BARRIER)
color 0A

echo ========================================================
echo   CONG CU CAP NHAT CODE MOI CHO ESP32-S3 BARRIER V2.0
echo   (Chi nap de App Firmware, GIU NGUYEN IP VA CAU HINH)
echo ========================================================
echo.

set PYTHON_CMD=python
where python >nul 2>nul
if %errorlevel% neq 0 (
    if exist "%USERPROFILE%\.platformio\penv\Scripts\python.exe" (
        set "PYTHON_CMD=%USERPROFILE%\.platformio\penv\Scripts\python.exe"
    ) else (
        echo [LOI] Khong tim thay Python tren may tinh!
        echo Vui long cai dat Python hoac PlatformIO.
        pause
        exit /b 1
    )
)

echo Dang do tim cong COM va bat dau nap firmware.bin (0x10000)...
echo.

"%PYTHON_CMD%" -m esptool --chip esp32s3 write_flash 0x10000 firmware.bin

if %errorlevel% equ 0 (
    echo.
    echo ========================================================
    echo   [THANH CONG] DA CAP NHAT FIRMWARE V2.0 THANH CONG!
    echo ========================================================
) else (
    echo.
    echo [LOI] Nap thieu hoac khong tim thay cong COM! Kiem tra cap USB Type-C.
)

echo.
pause
