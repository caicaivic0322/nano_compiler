# nanoCompiler

轻量级 C++ 和 Python IDE，专为低分辨率显示器和性能一般的电脑优化。

## 项目简介

nanoCompiler 包含两个独立的 IDE：
- **nanoC++**: C++ 集成开发环境，内置 TinyCC 编译器
- **nanoPython**: Python 集成开发环境，内置 MicroPython 解释器

## 特性

### 通用特性
- ✅ **多标签页支持** - 最多同时打开 3 个文件
- ✅ **代码折叠** - 支持代码块折叠/展开
- ✅ **语法高亮** - C++ 和 Python 语法高亮
- ✅ **自动缩进** - 智能自动缩进
- ✅ **快速文件操作** - 优化的文件打开/保存速度
- ✅ **低分辨率优化** - 专为 1024x768 等低分辨率显示器优化
- ✅ **性能优化** - 适合性能一般的电脑
- ✅ **开箱即用** - 内置编译器/解释器，无需额外配置

### nanoC++ 特性
- 内置 TinyCC 编译器
- C++ 语法高亮
- 编译错误提示
- 新建文件自动包含标准框架代码

### nanoPython 特性
- 内置 MicroPython 解释器
- Python 语法高亮
- 运行时错误提示
- 新建文件自动包含标准框架代码

## 系统要求

- **操作系统**: Windows 10/11 (64位)
- **内存**: 2GB 或更多
- **显示器**: 支持 1024x768 或更高分辨率
- **存储空间**: 200MB 可用空间

## 安装

### 方法 1: 使用安装包（推荐）

1. 下载 `nanoCompiler_setup_v1.0.0.exe`
2. 双击运行安装程序
3. 按照向导完成安装
4. 从开始菜单或桌面快捷方式启动

### 方法 2: 从源码构建

#### 前提条件
- Visual Studio 2022 或更高版本
- CMake 3.16 或更高版本
- Qt 5.15 或 Qt 6.x
- Inno Setup 6（用于创建安装包）

#### 构建步骤

```bash
# 克隆仓库
git clone https://github.com/caicaivic0322/nano_compiler.git
cd nano_compiler

# 运行构建脚本
scripts\build_installer.bat
```

构建完成后，安装包位于 `installer/output/nanoCompiler_setup_v1.0.0.exe`

## 使用指南

### 快捷键

| 快捷键 | 功能 |
|--------|------|
| Ctrl+N | 新建文件（新标签页） |
| Ctrl+O | 打开文件 |
| Ctrl+S | 保存当前文件 |
| Ctrl+Shift+S | 保存所有文件 |
| Ctrl+W | 关闭当前标签页 |
| Ctrl+R | 运行代码 |
| Ctrl+Shift+F5 | 停止运行 |
| Ctrl+Shift+A | 折叠全部代码块 |
| Ctrl+Shift+E | 展开全部代码块 |
| Ctrl+Shift+F | 折叠/展开当前代码块 |

### 多标签页使用

- 最多可同时打开 **3 个标签页**
- 达到上限时会提示关闭部分标签页
- 未保存的文件会在标签页标题显示 `*`
- 关闭标签页时会提示保存未保存的更改

### 代码折叠

- 点击行号区域的 `[-]` 或 `[+]` 折叠/展开代码块
- 使用菜单或快捷键批量折叠/展开

## 项目结构

```
nano_compiler/
├── nanoC++/              # C++ IDE 源码
│   ├── src/             # 源代码
│   ├── resources/       # 资源文件
│   └── CMakeLists.txt   # CMake 配置
├── nanoPython/          # Python IDE 源码
│   ├── src/             # 源代码
│   ├── resources/       # 资源文件
│   └── CMakeLists.txt   # CMake 配置
├── installer/           # 安装程序配置
├── scripts/             # 构建脚本
│   ├── build_installer.bat
│   ├── download_compilers.bat
│   └── setup_dev_env.bat
├── third_party/         # 第三方库
│   ├── tcc/            # TinyCC 编译器
│   └── micropython/    # MicroPython 解释器
└── README.md           # 本文件
```

## 技术栈

- **框架**: Qt 5.15+ / Qt 6.x
- **语言**: C++17
- **构建工具**: CMake 3.16+
- **编译器**: TinyCC (C++), MicroPython (Python)
- **安装程序**: Inno Setup

## 开发计划

- [x] 基础 IDE 功能
- [x] 语法高亮
- [x] 代码折叠
- [x] 多标签页支持
- [x] 文件操作优化
- [x] Windows 安装包
- [ ] 代码自动补全
- [ ] 搜索替换功能
- [ ] 断点调试
- [ ] 主题切换
- [ ] 插件系统

## 贡献

欢迎提交 Issue 和 Pull Request！

## 许可证

本项目采用 MIT 许可证 - 详见 [LICENSE](LICENSE) 文件

## 致谢

- [TinyCC](https://bellard.org/tcc/) - 轻量级 C 编译器
- [MicroPython](https://micropython.org/) - 精简版 Python 解释器
- [Qt](https://www.qt.io/) - 跨平台应用程序框架

## 联系方式

- GitHub: https://github.com/caicaivic0322/nano_compiler
- 邮箱: [your-email@example.com]

---

**注意**: 本项目专为教育用途设计，适合初学者学习 C++ 和 Python 编程。
