#!/bin/bash

# nanoCompiler 安装包构建脚本 (Linux/macOS)
# 用法: ./build_installer.sh [linux|macos]

set -e

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# 设置变量
PROJECT_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BUILD_DIR="$PROJECT_ROOT/build"
PACKAGE_DIR="$PROJECT_ROOT/package"
PLATFORM="${1:-linux}"

echo "=========================================="
echo "nanoCompiler 安装包构建脚本"
echo "=========================================="
echo ""

# 检查参数
if [[ "$PLATFORM" != "linux" && "$PLATFORM" != "macos" ]]; then
    echo -e "${RED}错误: 不支持的平台 '$PLATFORM'${NC}"
    echo "用法: $0 [linux|macos]"
    exit 1
fi

# 检查必要的工具
echo "[1/6] 检查构建环境..."

if ! command -v cmake &> /dev/null; then
    echo -e "${RED}错误: 找不到 CMake${NC}"
    exit 1
fi

if ! command -v make &> /dev/null; then
    echo -e "${RED}错误: 找不到 make${NC}"
    exit 1
fi

# 检查 Qt
if ! command -v qmake &> /dev/null; then
    echo -e "${YELLOW}警告: 找不到 qmake，请确保 Qt 已安装${NC}"
fi

echo -e "${GREEN}构建环境检查通过！${NC}"
echo ""

# 创建构建目录
echo "[2/6] 准备构建目录..."
rm -rf "$BUILD_DIR" "$PACKAGE_DIR"
mkdir -p "$BUILD_DIR/nanoC++"
mkdir -p "$BUILD_DIR/nanoPython"
mkdir -p "$PACKAGE_DIR"
echo -e "${GREEN}构建目录准备完成！${NC}"
echo ""

# 构建 nanoC++
echo "[3/6] 构建 nanoC++..."
cd "$BUILD_DIR/nanoC++"

cmake "$PROJECT_ROOT/nanoC++" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX="$PACKAGE_DIR/nanoC++"

make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

echo -e "${GREEN}nanoC++ 构建完成！${NC}"
echo ""

# 构建 nanoPython
echo "[4/6] 构建 nanoPython..."
cd "$BUILD_DIR/nanoPython"

cmake "$PROJECT_ROOT/nanoPython" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX="$PACKAGE_DIR/nanoPython"

make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

echo -e "${GREEN}nanoPython 构建完成！${NC}"
echo ""

# 安装到包目录
echo "[5/6] 安装到包目录..."
cd "$BUILD_DIR/nanoC++"
make install

cd "$BUILD_DIR/nanoPython"
make install

# 复制资源文件
cp -r "$PROJECT_ROOT/nanoC++/resources" "$PACKAGE_DIR/nanoC++/" 2>/dev/null || true
cp -r "$PROJECT_ROOT/nanoPython/resources" "$PACKAGE_DIR/nanoPython/" 2>/dev/null || true

# 复制示例代码
mkdir -p "$PACKAGE_DIR/examples"
cp -r "$PROJECT_ROOT/examples/"* "$PACKAGE_DIR/examples/" 2>/dev/null || true

# 复制文档
cp "$PROJECT_ROOT/README.md" "$PACKAGE_DIR/" 2>/dev/null || true
cp -r "$PROJECT_ROOT/docs" "$PACKAGE_DIR/" 2>/dev/null || true

echo -e "${GREEN}安装完成！${NC}"
echo ""

# 创建安装包
echo "[6/6] 创建安装包..."
cd "$PROJECT_ROOT"

VERSION="1.0.0"
PACKAGE_NAME="nanoCompiler-${VERSION}-${PLATFORM}"

if [[ "$PLATFORM" == "linux" ]]; then
    # 创建 AppImage 或 tar.gz
    if command -v linuxdeployqt &> /dev/null; then
        echo "使用 linuxdeployqt 创建 AppImage..."
        
        # nanoC++ AppImage
        mkdir -p "$PACKAGE_DIR/nanoC++.AppDir/usr/bin"
        mkdir -p "$PACKAGE_DIR/nanoC++.AppDir/usr/share/applications"
        mkdir -p "$PACKAGE_DIR/nanoC++.AppDir/usr/share/icons/hicolor/256x256/apps"
        
        cp "$PACKAGE_DIR/nanoC++/bin/nanoC++" "$PACKAGE_DIR/nanoC++.AppDir/usr/bin/"
        cp "$PROJECT_ROOT/resources/icon.png" "$PACKAGE_DIR/nanoC++.AppDir/usr/share/icons/hicolor/256x256/apps/nanoC++.png"
        
        cat > "$PACKAGE_DIR/nanoC++.AppDir/usr/share/applications/nanoC++.desktop" << EOF
[Desktop Entry]
Name=nanoC++
Exec=nanoC++
Icon=nanoC++
Type=Application
Categories=Development;IDE;
Comment=Lightweight C++ IDE
EOF
        
        linuxdeployqt "$PACKAGE_DIR/nanoC++.AppDir/usr/share/applications/nanoC++.desktop" -appimage
        mv nanoC++*.AppImage "$PACKAGE_DIR/"
        
        # nanoPython AppImage
        mkdir -p "$PACKAGE_DIR/nanoPython.AppDir/usr/bin"
        mkdir -p "$PACKAGE_DIR/nanoPython.AppDir/usr/share/applications"
        mkdir -p "$PACKAGE_DIR/nanoPython.AppDir/usr/share/icons/hicolor/256x256/apps"
        
        cp "$PACKAGE_DIR/nanoPython/bin/nanoPython" "$PACKAGE_DIR/nanoPython.AppDir/usr/bin/"
        cp "$PROJECT_ROOT/resources/icon_python.png" "$PACKAGE_DIR/nanoPython.AppDir/usr/share/icons/hicolor/256x256/apps/nanoPython.png"
        
        cat > "$PACKAGE_DIR/nanoPython.AppDir/usr/share/applications/nanoPython.desktop" << EOF
[Desktop Entry]
Name=nanoPython
Exec=nanoPython
Icon=nanoPython
Type=Application
Categories=Development;IDE;
Comment=Lightweight Python IDE
EOF
        
        linuxdeployqt "$PACKAGE_DIR/nanoPython.AppDir/usr/share/applications/nanoPython.desktop" -appimage
        mv nanoPython*.AppImage "$PACKAGE_DIR/"
    else
        # 创建 tar.gz
        echo "创建 tar.gz 包..."
        cd "$PACKAGE_DIR"
        tar -czf "${PACKAGE_NAME}.tar.gz" nanoC++ nanoPython examples docs README.md
    fi
    
elif [[ "$PLATFORM" == "macos" ]]; then
    # 创建 macOS app bundle
    echo "创建 macOS app bundle..."
    
    # nanoC++ app
    mkdir -p "$PACKAGE_DIR/nanoC++.app/Contents/MacOS"
    mkdir -p "$PACKAGE_DIR/nanoC++.app/Contents/Resources"
    
    cp "$PACKAGE_DIR/nanoC++/bin/nanoC++" "$PACKAGE_DIR/nanoC++.app/Contents/MacOS/"
    cp "$PROJECT_ROOT/resources/Info.plist" "$PACKAGE_DIR/nanoC++.app/Contents/"
    cp "$PROJECT_ROOT/resources/icon.icns" "$PACKAGE_DIR/nanoC++.app/Contents/Resources/"
    
    # nanoPython app
    mkdir -p "$PACKAGE_DIR/nanoPython.app/Contents/MacOS"
    mkdir -p "$PACKAGE_DIR/nanoPython.app/Contents/Resources"
    
    cp "$PACKAGE_DIR/nanoPython/bin/nanoPython" "$PACKAGE_DIR/nanoPython.app/Contents/MacOS/"
    cp "$PROJECT_ROOT/resources/Info_python.plist" "$PACKAGE_DIR/nanoPython.app/Contents/Info.plist"
    cp "$PROJECT_ROOT/resources/icon_python.icns" "$PACKAGE_DIR/nanoPython.app/Contents/Resources/"
    
    # 创建 dmg
    if command -v create-dmg &> /dev/null; then
        create-dmg \
            --volname "nanoCompiler ${VERSION}" \
            --window-pos 200 120 \
            --window-size 800 400 \
            --icon-size 100 \
            --app-drop-link 600 185 \
            "$PACKAGE_DIR/${PACKAGE_NAME}.dmg" \
            "$PACKAGE_DIR/nanoC++.app" \
            "$PACKAGE_DIR/nanoPython.app"
    else
        # 简单打包
        cd "$PACKAGE_DIR"
        tar -czf "${PACKAGE_NAME}.tar.gz" nanoC++.app nanoPython.app examples docs README.md
    fi
fi

echo ""
echo "=========================================="
echo -e "${GREEN}构建完成！${NC}"
echo "=========================================="
echo ""
echo "安装包位置: $PACKAGE_DIR"
echo ""

# 列出生成的文件
echo "生成的文件:"
ls -lh "$PACKAGE_DIR"/*.{tar.gz,dmg,AppImage} 2>/dev/null || ls -lh "$PACKAGE_DIR"
echo ""
