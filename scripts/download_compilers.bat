@echo off
chcp 65001 >nul
echo ==========================================
echo 下载编译器和解释器脚本
echo ==========================================
echo.

set PROJECT_ROOT=%~dp0..
set DOWNLOAD_DIR=%PROJECT_ROOT%\third_party\downloads
set TCC_DIR=%PROJECT_ROOT%\third_party\tcc
set MP_DIR=%PROJECT_ROOT%\third_party\micropython

mkdir "%DOWNLOAD_DIR%" 2>nul
mkdir "%TCC_DIR%" 2>nul
mkdir "%MP_DIR%" 2>nul

echo [1/2] 下载 TinyCC 编译器...

if exist "%TCC_DIR%\tcc.exe" (
    echo TinyCC 已存在，跳过下载
) else (
    echo 正在下载 TinyCC...
    
    :: TinyCC 0.9.27 Windows 64-bit
    set TCC_URL=https://download.savannah.gnu.org/releases/tinycc/tcc-0.9.27-win64-bin.zip
    set TCC_ZIP=%DOWNLOAD_DIR%\tcc.zip
    
    powershell -Command "Invoke-WebRequest -Uri '%TCC_URL%' -OutFile '%TCC_ZIP%'"
    
    if exist "%TCC_ZIP%" (
        echo 下载完成，正在解压...
        powershell -Command "Expand-Archive -Path '%TCC_ZIP%' -DestinationPath '%DOWNLOAD_DIR%' -Force"
        
        :: 移动文件到正确位置
        xcopy /E /I /Y "%DOWNLOAD_DIR%\tcc" "%TCC_DIR%"
        
        echo TinyCC 安装完成！
    ) else (
        echo 错误：下载失败
        echo 请手动下载 TinyCC 并解压到 %TCC_DIR%
        echo 下载地址：https://download.savannah.gnu.org/releases/tinycc/
    )
)

echo.
echo [2/2] 下载 MicroPython 解释器...

if exist "%MP_DIR%\micropython.exe" (
    echo MicroPython 已存在，跳过下载
) else (
    echo 正在下载 MicroPython...
    
    :: MicroPython Windows 最新版
    set MP_URL=https://micropython.org/resources/win32/micropython.exe
    
    powershell -Command "Invoke-WebRequest -Uri '%MP_URL%' -OutFile '%MP_DIR%\micropython.exe'"
    
    if exist "%MP_DIR%\micropython.exe" (
        echo MicroPython 安装完成！
    ) else (
        echo 错误：下载失败
        echo 请手动下载 MicroPython 并放到 %MP_DIR%
        echo 下载地址：https://micropython.org/download/
    )
)

echo.
echo ==========================================
echo 下载完成！
echo ==========================================
echo.

pause
