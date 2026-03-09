cmake_minimum_required(VERSION 3.16)
project(nanoPython VERSION 1.0.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 查找 Qt6 或 Qt5
find_package(Qt6 COMPONENTS Widgets Core Gui Concurrent QUIET)
if(NOT Qt6_FOUND)
    find_package(Qt5 5.15 COMPONENTS Widgets Core Gui Concurrent REQUIRED)
    set(QT_VERSION_MAJOR 5)
else()
    set(QT_VERSION_MAJOR 6)
endif()

# 平台检测
if(WIN32)
    set(PLATFORM_WINDOWS TRUE)
    add_definitions(-DPLATFORM_WINDOWS)
elseif(APPLE)
    set(PLATFORM_MAC TRUE)
    add_definitions(-DPLATFORM_MAC)
else()
    set(PLATFORM_LINUX TRUE)
    add_definitions(-DPLATFORM_LINUX)
endif()

# 源文件
set(SOURCES
    src/main.cpp
    src/main_window.cpp
    src/editor/code_editor.cpp
    src/editor/syntax_highlighter.cpp
    src/editor/code_folding.cpp
    src/editor/tab_widget.cpp
    src/interpreter/mp_wrapper.cpp
    src/interpreter/run_task.cpp
    src/ui/toolbar.cpp
    src/ui/output_panel.cpp
    src/ui/project_panel.cpp
    src/ui/repl_panel.cpp
    src/ui/splash_dialog.cpp
    src/utils/file_utils.cpp
)

set(HEADERS
    src/main_window.h
    src/editor/code_editor.h
    src/editor/syntax_highlighter.h
    src/editor/code_folding.h
    src/editor/tab_widget.h
    src/interpreter/mp_wrapper.h
    src/interpreter/run_task.h
    src/ui/toolbar.h
    src/ui/output_panel.h
    src/ui/project_panel.h
    src/ui/repl_panel.h
    src/ui/splash_dialog.h
    src/utils/file_utils.h
)

# 资源文件
set(RESOURCES
    resources/nanoPython.qrc
)

# 可执行文件
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${RESOURCES})

# 链接 Qt
target_link_libraries(${PROJECT_NAME} 
    Qt::Widgets 
    Qt::Core 
    Qt::Gui
    Qt::Concurrent
)

# 包含目录
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/micropython
)

# Windows 特定设置
if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
    
    # 复制 Qt 依赖
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_PREFIX_PATH}/bin/windeployqt.exe $<TARGET_FILE:${PROJECT_NAME}>
        COMMENT "Deploying Qt dependencies..."
    )
endif()

# 安装规则
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
    BUNDLE DESTINATION .
)

# 复制资源文件
install(DIRECTORY resources/
    DESTINATION share/${PROJECT_NAME}/resources
)
