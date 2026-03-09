@echo off
chcp 65001 >nul
echo ==========================================
echo nanoCompiler 开发环境设置脚本
echo ==========================================
echo.

set PROJECT_ROOT=%~dp0..

echo [1/4] 创建目录结构...
mkdir "%PROJECT_ROOT%\third_party\tcc" 2>nul
mkdir "%PROJECT_ROOT%\third_party\micropython" 2>nul
mkdir "%PROJECT_ROOT%\examples\c++" 2>nul
mkdir "%PROJECT_ROOT%\examples\python" 2>nul
mkdir "%PROJECT_ROOT%\docs" 2>nul
mkdir "%PROJECT_ROOT%\resources" 2>nul
echo 目录结构创建完成！
echo.

echo [2/4] 下载 TinyCC 编译器...
echo 请访问 https://download.savannah.gnu.org/releases/tinycc/ 下载 Windows 版本
echo 解压到: %PROJECT_ROOT%\third_party\tcc\
echo.

echo [3/4] 下载 MicroPython...
echo 请访问 https://micropython.org/download/ 下载 Windows 版本
echo 解压到: %PROJECT_ROOT%\third_party\micropython\
echo.

echo [4/4] 创建示例文件...

:: C++ 示例
echo #include ^<iostream^> > "%PROJECT_ROOT%\examples\c++\hello.cpp"
echo. >> "%PROJECT_ROOT%\examples\c++\hello.cpp"
echo using namespace std; >> "%PROJECT_ROOT%\examples\c++\hello.cpp"
echo. >> "%PROJECT_ROOT%\examples\c++\hello.cpp"
echo int main() { >> "%PROJECT_ROOT%\examples\c++\hello.cpp"
echo     cout ^<^< "Hello, nanoC++!" ^<^< endl; >> "%PROJECT_ROOT%\examples\c++\hello.cpp"
echo     return 0; >> "%PROJECT_ROOT%\examples\c++\hello.cpp"
echo } >> "%PROJECT_ROOT%\examples\c++\hello.cpp"

:: Python 示例
echo print("Hello, nanoPython!") > "%PROJECT_ROOT%\examples\python\hello.py"
echo. >> "%PROJECT_ROOT%\examples\python\hello.py"
echo def greet(name): >> "%PROJECT_ROOT%\examples\python\hello.py"
echo     return f"Hello, {name}!" >> "%PROJECT_ROOT%\examples\python\hello.py"
echo. >> "%PROJECT_ROOT%\examples\python\hello.py"
echo if __name__ == "__main__": >> "%PROJECT_ROOT%\examples\python\hello.py"
echo     print(greet("World")) >> "%PROJECT_ROOT%\examples\python\hello.py"

echo 示例文件创建完成！
echo.

echo ==========================================
echo 开发环境设置完成！
echo ==========================================
echo.
echo 下一步:
echo 1. 下载并安装 TinyCC 到 third_party\tcc\
echo 2. 下载并安装 MicroPython 到 third_party\micropython\
echo 3. 运行 build_installer.bat 构建安装包
echo.

pause
