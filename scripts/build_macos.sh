#!/bin/bash

# ==========================================
# nanoCompiler macOS 构建脚本
# ==========================================

echo "=========================================="
echo "nanoCompiler macOS 构建脚本"
echo "=========================================="
echo ""

# 设置变量
PROJECT_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BUILD_DIR="$PROJECT_ROOT/build"

echo "项目目录: $PROJECT_ROOT"
echo ""

# 检查必要的工具
echo "[1/5] 检查构建环境..."

if ! command -v cmake &> /dev/null; then
    echo "错误: 找不到 CMake，请安装 CMake"
    echo "运行: brew install cmake"
    exit 1
fi

if ! command -v qmake &> /dev/null; then
    echo "错误: 找不到 Qt，请安装 Qt"
    echo "运行: brew install qt"
    exit 1
fi

echo "构建环境检查通过！"
echo ""

# 创建构建目录
echo "[2/5] 准备构建目录..."
if [ -d "$BUILD_DIR" ]; then
    rm -rf "$BUILD_DIR"
fi
mkdir -p "$BUILD_DIR"
mkdir -p "$BUILD_DIR/nanoC++"
mkdir -p "$BUILD_DIR/nanoPython"
echo "构建目录准备完成！"
echo ""

# 构建 nanoC++
echo "[3/5] 构建 nanoC++..."
cd "$BUILD_DIR/nanoC++"

cmake "$PROJECT_ROOT/nanoC++" -DCMAKE_BUILD_TYPE=Release
if [ $? -ne 0 ]; then
    echo "错误: nanoC++ CMake 配置失败"
    exit 1
fi

cmake --build . --parallel
if [ $? -ne 0 ]; then
    echo "错误: nanoC++ 构建失败"
    exit 1
fi

echo "nanoC++ 构建完成！"
echo ""

# 构建 nanoPython
echo "[4/5] 构建 nanoPython..."
cd "$BUILD_DIR/nanoPython"

cmake "$PROJECT_ROOT/nanoPython" -DCMAKE_BUILD_TYPE=Release
if [ $? -ne 0 ]; then
    echo "错误: nanoPython CMake 配置失败"
    exit 1
fi

cmake --build . --parallel
if [ $? -ne 0 ]; then
    echo "错误: nanoPython 构建失败"
    exit 1
fi

echo "nanoPython 构建完成！"
echo ""

# 创建应用包
echo "[5/5] 创建应用包..."

# 创建输出目录
OUTPUT_DIR="$PROJECT_ROOT/output"
mkdir -p "$OUTPUT_DIR"

# 复制可执行文件
cp "$BUILD_DIR/nanoC++/nanoC++" "$OUTPUT_DIR/"
cp "$BUILD_DIR/nanoPython/nanoPython" "$OUTPUT_DIR/"

echo ""
echo "=========================================="
echo "构建完成！"
echo "=========================================="
echo ""
echo "可执行文件位置:"
echo "  - $OUTPUT_DIR/nanoC++"
echo "  - $OUTPUT_DIR/nanoPython"
echo ""
echo "注意: macOS 版本需要手动安装 TinyCC 和 MicroPython:"
echo "  - TinyCC: brew install tcc"
echo "  - MicroPython: brew install micropython"
echo ""
