#!/bin/bash

# 下载编译器和解释器脚本 (Linux/macOS)

set -e

PROJECT_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
DOWNLOAD_DIR="$PROJECT_ROOT/third_party/downloads"
TCC_DIR="$PROJECT_ROOT/third_party/tcc"
MP_DIR="$PROJECT_ROOT/third_party/micropython"

mkdir -p "$DOWNLOAD_DIR"
mkdir -p "$TCC_DIR"
mkdir -p "$MP_DIR"

echo "=========================================="
echo "下载编译器和解释器脚本"
echo "=========================================="
echo ""

echo "[1/2] 检查/下载 TinyCC 编译器..."

if command -v tcc &> /dev/null; then
    echo "TinyCC 已通过包管理器安装: $(tcc --version | head -1)"
    # 创建符号链接
    ln -sf "$(which tcc)" "$TCC_DIR/tcc" 2>/dev/null || true
elif [[ -f "$TCC_DIR/tcc" ]]; then
    echo "TinyCC 已存在，跳过下载"
else
    echo "尝试从包管理器安装 TinyCC..."
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        if command -v apt-get &> /dev/null; then
            sudo apt-get update && sudo apt-get install -y tcc
        elif command -v yum &> /dev/null; then
            sudo yum install -y tcc
        elif command -v pacman &> /dev/null; then
            sudo pacman -S --noconfirm tcc
        fi
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        if command -v brew &> /dev/null; then
            brew install tcc
        fi
    fi
fi

echo ""
echo "[2/2] 检查/下载 MicroPython 解释器..."

if command -v micropython &> /dev/null; then
    echo "MicroPython 已通过包管理器安装: $(micropython --version)"
    # 创建符号链接
    ln -sf "$(which micropython)" "$MP_DIR/micropython" 2>/dev/null || true
elif [[ -f "$MP_DIR/micropython" ]]; then
    echo "MicroPython 已存在，跳过下载"
else
    echo "尝试从包管理器安装 MicroPython..."
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        if command -v apt-get &> /dev/null; then
            sudo apt-get update && sudo apt-get install -y micropython
        elif command -v pip3 &> /dev/null; then
            pip3 install micropython
        fi
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        if command -v brew &> /dev/null; then
            brew install micropython
        fi
    fi
fi

echo ""
echo "=========================================="
echo "完成！"
echo "=========================================="
echo ""
