#ifndef OUTPUT_PANEL_H
#define OUTPUT_PANEL_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QTextEdit;
class QVBoxLayout;
QT_END_NAMESPACE

class OutputPanel : public QWidget
{
    Q_OBJECT

public:
    enum MessageType {
        Normal,
        Info,
        Success,
        Error,
        Warning
    };

    explicit OutputPanel(QWidget *parent = nullptr);

    void appendMessage(const QString &message, MessageType type = Normal);
    void clear();
    void setFont(const QFont &font);

private:
    QString formatMessage(const QString &message, MessageType type);

    QTextEdit *m_textEdit;
};

#endif // OUTPUT_PANEL_H
