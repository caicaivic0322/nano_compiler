#ifndef REPL_PANEL_H
#define REPL_PANEL_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QTextEdit;
class QLineEdit;
QT_END_NAMESPACE

class ReplPanel : public QWidget
{
    Q_OBJECT

public:
    explicit ReplPanel(QWidget *parent = nullptr);

    void appendOutput(const QString &text);
    void clear();

private slots:
    void onCommandEntered();

private:
    void setupUI();
    void executeCommand(const QString &command);

    QTextEdit *m_outputEdit;
    QLineEdit *m_inputEdit;
};

#endif // REPL_PANEL_H
