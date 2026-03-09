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

    m_textEdit->setReadOnly(true);
    m_textEdit->setLineWrapMode(QTextEdit::WidgetWidth);

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

QString OutputPanel::formatMessage(const QString &message, MessageType type)
{
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    QString color;
    QString prefix;

    switch (type) {
        case Info:
            color = "#64B5F6";
            prefix = "[信息]";
            break;
        case Success:
            color = "#81C784";
            prefix = "[成功]";
            break;
        case Error:
            color = "#E57373";
            prefix = "[错误]";
            break;
        case Warning:
            color = "#FFB74D";
            prefix = "[警告]";
            break;
        case Normal:
        default:
            color = "#ECEFF1";
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
