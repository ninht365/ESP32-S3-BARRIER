@echo off
chcp 65001 > nul
title CONG CU NAP MACH MOI FIRMWARE BARRIER V2.0 (4 BARRIER)
color 0C

echo ========================================================
echo   CONG CU NAP MACH MOI XUAT XUONG ESP32-S3 BARRIER V2.0
echo   (Xoa full Flash va nap Bootloader + Partitions + App)
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

echo Dang xoa sach Flash cua mach...
"%PYTHON_CMD%" -m esptool --chip esp32s3 erase_flash

echo.
echo Dang nap Bootloader, Partitions va Firmware.bin...
"%PYTHON_CMD%" -m esptool --chip esp32s3 write_flash 0x0 bootloader.bin 0x8000 partitions.bin 0xe000 boot_app0.bin 0x10000 firmware.bin

if %errorlevel% equ 0 (
    echo.
    echo ========================================================
    echo   [THANH CONG] DA NAP MACH MOI V2.0 (4 BARRIER) THANH CONG!
    echo   IP mặc định: 192.168.1.200
    echo ========================================================
) else (
    echo.
    echo [LOI] Nap thất bại! Kiem tra cap USB Type-C va Driver CH340/CP210x.
)

echo.
pause
