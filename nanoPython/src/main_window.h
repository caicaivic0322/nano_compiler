#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QSplitter>
#include <memory>

QT_BEGIN_NAMESPACE
class QTextEdit;
class QTreeView;
class QFileSystemModel;
class QToolBar;
class QAction;
class QLabel;
class QCloseEvent;
QT_END_NAMESPACE

class TabWidget;
class CodeEditor;
class OutputPanel;
class ProjectPanel;
class ReplPanel;
class MpWrapper;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onNewFile();
    void onOpenFile();
    void onSaveFile();
    void onSaveAs();
    void onSaveAll();
    void onCloseTab();
    void onRun();
    void onStop();
    void onAbout();
    void onNewProject();
    void onOpenProject();
    void onFileSelected(const QString &filePath);
    void onExecutionFinished(bool success, const QString &output);
    void onTabChanged(int index);
    void onTabModified(int index, bool modified);

private:
    void setupUI();
    void setupMenuBar();
    void setupToolBar();
    void setupCentralWidget();
    void setupStatusBar();
    void setupConnections();
    void loadTemplates();

    bool maybeSave();
    bool maybeSaveAll();
    void setCurrentFile(const QString &fileName);
    void updateWindowTitle();
    void updateActions();
    CodeEditor* currentEditor() const;
    bool saveFile(const QString &filePath, const QString &content);

protected:
    void closeEvent(QCloseEvent *event) override;

    // UI 组件
    TabWidget *m_tabWidget;
    OutputPanel *m_outputPanel;
    ProjectPanel *m_projectPanel;
    ReplPanel *m_replPanel;
    
    // 工具栏动作
    QAction *m_newAction;
    QAction *m_openAction;
    QAction *m_saveAction;
    QAction *m_saveAsAction;
    QAction *m_saveAllAction;
    QAction *m_closeTabAction;
    QAction *m_runAction;
    QAction *m_stopAction;
    QAction *m_undoAction;
    QAction *m_redoAction;
    QAction *m_cutAction;
    QAction *m_copyAction;
    QAction *m_pasteAction;
    
    // 解释器
    std::unique_ptr<MpWrapper> m_interpreter;
    
    // 状态
    QString m_currentFile;
    QString m_currentProject;
    bool m_isModified;
    bool m_isRunning;
    
    // 状态栏标签
    QLabel *m_statusLabel;
    QLabel *m_fileTypeLabel;
};

#endif // MAIN_WINDOW_H
