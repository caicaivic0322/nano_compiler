#include "toolbar.h"

#include <QAction>
#include <QStyle>

ToolBar::ToolBar(QWidget *parent)
    : QToolBar(parent)
    , m_newAction(nullptr)
    , m_openAction(nullptr)
    , m_saveAction(nullptr)
    , m_runAction(nullptr)
    , m_stopAction(nullptr)
    , m_undoAction(nullptr)
    , m_redoAction(nullptr)
{
    setupActions();
    setupUI();
}

void ToolBar::setupActions()
{
    // 新建文件
    m_newAction = new QAction("新建", this);
    m_newAction->setShortcut(QKeySequence::New);
    m_newAction->setStatusTip("创建新文件 (Ctrl+N)");
    
    // 打开文件
    m_openAction = new QAction("打开", this);
    m_openAction->setShortcut(QKeySequence::Open);
    m_openAction->setStatusTip("打开文件 (Ctrl+O)");
    
    // 保存文件
    m_saveAction = new QAction("保存", this);
    m_saveAction->setShortcut(QKeySequence::Save);
    m_saveAction->setStatusTip("保存文件 (Ctrl+S)");
    
    // 运行
    m_runAction = new QAction("▶ 运行", this);
    m_runAction->setShortcut(QKeySequence("Ctrl+R"));
    m_runAction->setStatusTip("编译并运行 (Ctrl+R)");
    m_runAction->setObjectName("runButton");
    
    // 停止
    m_stopAction = new QAction("■ 停止", this);
    m_stopAction->setShortcut(QKeySequence("Ctrl+Shift+F5"));
    m_stopAction->setStatusTip("停止编译 (Ctrl+Shift+F5)");
    m_stopAction->setEnabled(false);
    m_stopAction->setObjectName("stopButton");
    
    // 撤销
    m_undoAction = new QAction("撤销", this);
    m_undoAction->setShortcut(QKeySequence::Undo);
    m_undoAction->setStatusTip("撤销 (Ctrl+Z)");
    
    // 重做
    m_redoAction = new QAction("重做", this);
    m_redoAction->setShortcut(QKeySequence::Redo);
    m_redoAction->setStatusTip("重做 (Ctrl+Y)");
}

void ToolBar::setupUI()
{
    setMovable(false);
    setFloatable(false);
    
    // 添加文件操作按钮
    addAction(m_newAction);
    addAction(m_openAction);
    addAction(m_saveAction);
    
    addSeparator();
    
    // 添加编辑按钮
    addAction(m_undoAction);
    addAction(m_redoAction);
    
    addSeparator();
    
    // 添加运行按钮
    addAction(m_runAction);
    addAction(m_stopAction);
    
    // 设置样式
    setStyleSheet(
        "QToolBar {"
        "  background-color: #ffffff;"
        "  border-bottom: 1px solid #e0e0e0;"
        "  padding: 8px;"
        "  spacing: 8px;"
        "}"
        "QToolButton {"
        "  background-color: transparent;"
        "  border: none;"
        "  border-radius: 6px;"
        "  padding: 8px 16px;"
        "  font-size: 13px;"
        "  font-weight: 500;"
        "}"
        "QToolButton:hover {"
        "  background-color: #E3F2FD;"
        "}"
        "QToolButton:pressed {"
        "  background-color: #BBDEFB;"
        "}"
    );
}
