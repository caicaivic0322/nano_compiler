@echo off
chcp 65001 >nul
echo ==========================================
echo nanoCompiler 安装包构建脚本
echo ==========================================
echo.

:: 设置变量
set PROJECT_ROOT=%~dp0..
set BUILD_DIR=%PROJECT_ROOT%\build
set INSTALLER_DIR=%PROJECT_ROOT%\installer
set OUTPUT_DIR=%INSTALLER_DIR%\output

:: 检查必要的工具
echo [1/6] 检查构建环境...

where cmake >nul 2>nul
if %errorlevel% neq 0 (
    echo 错误: 找不到 CMake，请确保已安装 CMake 并添加到 PATH
    exit /b 1
)

where msbuild >nul 2>nul
if %errorlevel% neq 0 (
    echo 错误: 找不到 MSBuild，请确保已安装 Visual Studio
    exit /b 1
)

:: 检查 Inno Setup
set ISCC_PATH=C:\Program Files (x86)\Inno Setup 6\ISCC.exe
if not exist "%ISCC_PATH%" set ISCC_PATH=C:\Program Files\Inno Setup 6\ISCC.exe
if not exist "%ISCC_PATH%" (
    echo 错误: 找不到 Inno Setup，请安装 Inno Setup 6
    exit /b 1
)

echo 构建环境检查通过！
echo.

:: 下载编译器
echo [2/6] 下载编译器和解释器...
call "%PROJECT_ROOT%\scripts\download_compilers.bat"
if %errorlevel% neq 0 (
    echo 警告: 下载编译器时出现问题，继续构建...
)
echo.

:: 创建构建目录
echo [3/6] 准备构建目录...
if exist "%BUILD_DIR%" rmdir /s /q "%BUILD_DIR%"
mkdir "%BUILD_DIR%"
mkdir "%BUILD_DIR%\nanoC++"
mkdir "%BUILD_DIR%\nanoPython"
echo 构建目录准备完成！
echo.

:: 构建 nanoC++
echo [4/6] 构建 nanoC++...
cd /d "%BUILD_DIR%\nanoC++"

cmake -G "Visual Studio 17 2022" -A x64 "%PROJECT_ROOT%\nanoC++" -DCMAKE_BUILD_TYPE=Release
if %errorlevel% neq 0 (
    echo 错误: nanoC++ CMake 配置失败
    exit /b 1
)

cmake --build . --config Release --parallel
if %errorlevel% neq 0 (
    echo 错误: nanoC++ 构建失败
    exit /b 1
)

echo nanoC++ 构建完成！
echo.

:: 构建 nanoPython
echo [5/6] 构建 nanoPython...
cd /d "%BUILD_DIR%\nanoPython"

cmake -G "Visual Studio 17 2022" -A x64 "%PROJECT_ROOT%\nanoPython" -DCMAKE_BUILD_TYPE=Release
if %errorlevel% neq 0 (
    echo 错误: nanoPython CMake 配置失败
    exit /b 1
)

cmake --build . --config Release --parallel
if %errorlevel% neq 0 (
    echo 错误: nanoPython 构建失败
    exit /b 1
)

echo nanoPython 构建完成！
echo.

:: 复制依赖文件
echo [5.5/6] 复制依赖文件...

:: 复制 Qt 依赖
if exist "C:\Qt\6.5.0\msvc2019_64\bin\windeployqt.exe" (
    echo 部署 nanoC++ Qt 依赖...
    "C:\Qt\6.5.0\msvc2019_64\bin\windeployqt.exe" "%BUILD_DIR%\nanoC++\Release\nanoC++.exe"
    
    echo 部署 nanoPython Qt 依赖...
    "C:\Qt\6.5.0\msvc2019_64\bin\windeployqt.exe" "%BUILD_DIR%\nanoPython\Release\nanoPython.exe"
) else (
    echo 警告: 找不到 windeployqt，请手动复制 Qt 依赖
)

echo 依赖文件复制完成！
echo.

:: 创建安装包
echo [6/6] 创建安装包...
cd /d "%PROJECT_ROOT%"

"%ISCC_PATH%" "%INSTALLER_DIR%\nano_compiler_setup.iss"
if %errorlevel% neq 0 (
    echo 错误: 安装包创建失败
    exit /b 1
)

echo.
echo ==========================================
echo 构建完成！
echo ==========================================
echo.
echo 安装包位置: %OUTPUT_DIR%\nanoCompiler_setup_v1.0.0.exe
echo.
echo 用户安装后可以直接使用，无需额外配置！
echo.

pause
