#include "toolbar.h"

#include <QAction>

ToolBar::ToolBar(QWidget *parent)
    : QToolBar(parent)
    , m_newAction(nullptr)
    , m_openAction(nullptr)
    , m_saveAction(nullptr)
    , m_runAction(nullptr)
    , m_stopAction(nullptr)
{
    setupActions();
    setupUI();
}

void ToolBar::setupActions()
{
    m_newAction = new QAction("新建", this);
    m_newAction->setShortcut(QKeySequence::New);

    m_openAction = new QAction("打开", this);
    m_openAction->setShortcut(QKeySequence::Open);

    m_saveAction = new QAction("保存", this);
    m_saveAction->setShortcut(QKeySequence::Save);

    m_runAction = new QAction("▶ 运行", this);
    m_runAction->setShortcut(QKeySequence("Ctrl+R"));
    m_runAction->setObjectName("runButton");

    m_stopAction = new QAction("■ 停止", this);
    m_stopAction->setShortcut(QKeySequence("Ctrl+Shift+F5"));
    m_stopAction->setEnabled(false);
    m_stopAction->setObjectName("stopButton");
}

void ToolBar::setupUI()
{
    setMovable(false);
    setFloatable(false);

    addAction(m_newAction);
    addAction(m_openAction);
    addAction(m_saveAction);
    addSeparator();
    addAction(m_runAction);
    addAction(m_stopAction);

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
        "  background-color: #E8F5E9;"
        "}"
        "QToolButton:pressed {"
        "  background-color: #C8E6C9;"
        "}"
    );
}
