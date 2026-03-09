#include "repl_panel.h"

#include <QTextEdit>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QProcess>

ReplPanel::ReplPanel(QWidget *parent)
    : QWidget(parent)
    , m_outputEdit(new QTextEdit(this))
    , m_inputEdit(new QLineEdit(this))
{
    setupUI();
}

void ReplPanel::setupUI()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(4);

    m_outputEdit->setReadOnly(true);
    m_outputEdit->setStyleSheet(
        "QTextEdit {"
        "  background-color: #1B5E20;"
        "  color: #ffffff;"
        "  border: 1px solid #2E7D32;"
        "  border-radius: 8px;"
        "  padding: 12px;"
        "  font-family: 'Consolas', 'Monaco', monospace;"
        "  font-size: 13px;"
        "}"
    );
    m_outputEdit->setObjectName("replPanel");

    m_inputEdit->setPlaceholderText(">>> 输入 Python 命令...");
    m_inputEdit->setStyleSheet(
        "QLineEdit {"
        "  background-color: #2E7D32;"
        "  color: #ffffff;"
        "  border: 1px solid #4CAF50;"
        "  border-radius: 6px;"
        "  padding: 8px 12px;"
        "  font-family: 'Consolas', 'Monaco', monospace;"
        "  font-size: 13px;"
        "}"
    );

    layout->addWidget(m_outputEdit);
    layout->addWidget(m_inputEdit);

    connect(m_inputEdit, &QLineEdit::returnPressed, this, &ReplPanel::onCommandEntered);

    // 显示欢迎信息
    m_outputEdit->append("Python 交互式解释器");
    m_outputEdit->append("输入命令并按回车执行\n");
}

void ReplPanel::appendOutput(const QString &text)
{
    m_outputEdit->append(text);
}

void ReplPanel::clear()
{
    m_outputEdit->clear();
}

void ReplPanel::onCommandEntered()
{
    QString command = m_inputEdit->text();
    if (command.isEmpty()) return;

    m_outputEdit->append(">>> " + command);
    executeCommand(command);
    m_inputEdit->clear();
}

void ReplPanel::executeCommand(const QString &command)
{
    // 简单的命令执行示例
    if (command == "help") {
        m_outputEdit->append("可用命令:");
        m_outputEdit->append("  help - 显示帮助");
        m_outputEdit->append("  clear - 清屏");
        m_outputEdit->append("");
    } else if (command == "clear") {
        m_outputEdit->clear();
        m_outputEdit->append("Python 交互式解释器\n");
    } else {
        m_outputEdit->append("(交互式功能开发中...)\n");
    }
}
