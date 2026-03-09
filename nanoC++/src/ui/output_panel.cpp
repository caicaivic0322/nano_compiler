#include "output_panel.h"

#include <QTextEdit>
#include <QVBoxLayout>
#include <QDateTime>

OutputPanel::OutputPanel(QWidget *parent)
    : QWidget(parent)
    , m_textEdit(new QTextEdit(this))
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_textEdit);
    
    // 设置文本编辑器属性
    m_textEdit->setReadOnly(true);
    m_textEdit->setLineWrapMode(QTextEdit::WidgetWidth);
    
    // 设置样式
    m_textEdit->setStyleSheet(
        "QTextEdit {"
        "  background-color: #263238;"
        "  color: #aed581;"
        "  border: 1px solid #37474F;"
        "  border-radius: 8px;"
        "  padding: 12px;"
        "  font-family: 'Consolas', 'Monaco', monospace;"
        "  font-size: 13px;"
        "}"
    );
    
    // 设置对象名称以便样式表定位
    m_textEdit->setObjectName("outputPanel");
}

void OutputPanel::appendMessage(const QString &message, MessageType type)
{
    QString formattedMessage = formatMessage(message, type);
    m_textEdit->append(formattedMessage);
}

void OutputPanel::clear()
{
    m_textEdit->clear();
}

void OutputPanel::setFont(const QFont &font)
{
    m_textEdit->setFont(font);
}

QString OutputPanel::formatMessage(const QString &message, MessageType type)
{
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    QString color;
    QString prefix;
    
    switch (type) {
        case Info:
            color = "#64B5F6"; // 浅蓝色
            prefix = "[信息]";
            break;
        case Success:
            color = "#81C784"; // 浅绿色
            prefix = "[成功]";
            break;
        case Error:
            color = "#E57373"; // 浅红色
            prefix = "[错误]";
            break;
        case Warning:
            color = "#FFB74D"; // 浅橙色
            prefix = "[警告]";
            break;
        case Normal:
        default:
            color = "#ECEFF1"; // 浅灰色
            prefix = "";
            break;
    }
    
    QString formatted;
    if (!prefix.isEmpty()) {
        formatted = QString("<span style='color: #78909C;'>[%1]</span> <span style='color: %2; font-weight: bold;'>%3</span> %4")
                    .arg(timestamp, color, prefix, message.toHtmlEscaped());
    } else {
        formatted = QString("<span style='color: #78909C;'>[%1]</span> <span style='color: %2;'>%3</span>")
                    .arg(timestamp, color, message.toHtmlEscaped());
    }
    
    return formatted;
}
