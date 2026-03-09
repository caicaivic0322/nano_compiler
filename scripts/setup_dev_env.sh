#!/bin/bash

# nanoCompiler 开发环境设置脚本 (Linux/macOS)

set -e

PROJECT_ROOT="$(cd "$(dirname "$0")/.." && pwd)"

echo "=========================================="
echo "nanoCompiler 开发环境设置脚本"
echo "=========================================="
echo ""

echo "[1/4] 创建目录结构..."
mkdir -p "$PROJECT_ROOT/third_party/tcc"
mkdir -p "$PROJECT_ROOT/third_party/micropython"
mkdir -p "$PROJECT_ROOT/examples/c++"
mkdir -p "$PROJECT_ROOT/examples/python"
mkdir -p "$PROJECT_ROOT/docs"
mkdir -p "$PROJECT_ROOT/resources"
echo "目录结构创建完成！"
echo ""

echo "[2/4] 检查/安装 TinyCC..."
if command -v tcc &> /dev/null; then
    echo "TinyCC 已安装: $(tcc --version | head -1)"
else
    echo "尝试安装 TinyCC..."
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        if command -v apt-get &> /dev/null; then
            sudo apt-get update && sudo apt-get install -y tcc
        elif command -v yum &> /dev/null; then
            sudo yum install -y tcc
        elif command -v pacman &> /dev/null; then
            sudo pacman -S --noconfirm tcc
        else
            echo "请手动安装 TinyCC"
        fi
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        if command -v brew &> /dev/null; then
            brew install tcc
        else
            echo "请先安装 Homebrew，然后运行: brew install tcc"
        fi
    fi
fi
echo ""

echo "[3/4] 检查/安装 MicroPython..."
if command -v micropython &> /dev/null; then
    echo "MicroPython 已安装: $(micropython --version)"
else
    echo "尝试安装 MicroPython..."
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        if command -v apt-get &> /dev/null; then
            sudo apt-get update && sudo apt-get install -y micropython
        elif command -v pip3 &> /dev/null; then
            pip3 install micropython
        else
            echo "请手动安装 MicroPython"
        fi
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        if command -v brew &> /dev/null; then
            brew install micropython
        else
            echo "请先安装 Homebrew，然后运行: brew install micropython"
        fi
    fi
fi
echo ""

echo "[4/4] 创建示例文件..."

# C++ 示例
cat > "$PROJECT_ROOT/examples/c++/hello.cpp" << 'EOF'
#include <iostream>

using namespace std;

int main() {
    cout << "Hello, nanoC++!" << endl;
    return 0;
}
EOF

# Python 示例
cat > "$PROJECT_ROOT/examples/python/hello.py" << 'EOF'
print("Hello, nanoPython!")

def greet(name):
    return f"Hello, {name}!"

if __name__ == "__main__":
    print(greet("World"))
EOF

echo "示例文件创建完成！"
echo ""

echo "=========================================="
echo "开发环境设置完成！"
echo "=========================================="
echo ""
echo "下一步:"
echo "1. 确保 Qt 已安装"
echo "2. 运行 ./build_installer.sh linux 或 ./build_installer.sh macos 构建安装包"
echo ""
