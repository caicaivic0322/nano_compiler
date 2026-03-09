#ifndef TOOLBAR_H
#define TOOLBAR_H

#include <QToolBar>

QT_BEGIN_NAMESPACE
class QAction;
QT_END_NAMESPACE

class ToolBar : public QToolBar
{
    Q_OBJECT

public:
    explicit ToolBar(QWidget *parent = nullptr);

    QAction* newAction() const { return m_newAction; }
    QAction* openAction() const { return m_openAction; }
    QAction* saveAction() const { return m_saveAction; }
    QAction* runAction() const { return m_runAction; }
    QAction* stopAction() const { return m_stopAction; }

private:
    void setupActions();
    void setupUI();

    QAction *m_newAction;
    QAction *m_openAction;
    QAction *m_saveAction;
    QAction *m_runAction;
    QAction *m_stopAction;
};

#endif // TOOLBAR_H
