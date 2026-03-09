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
QT_END_NAMESPACE

class TabWidget;
class CodeEditor;
class OutputPanel;
class ProjectPanel;
class TccWrapper;

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
    void onCompilationFinished(bool success, const QString &output);
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

    // UI 组件
    TabWidget *m_tabWidget;
    OutputPanel *m_outputPanel;
    ProjectPanel *m_projectPanel;
    
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
    
    // 编译器
    std::unique_ptr<TccWrapper> m_compiler;
    
    // 状态
    QString m_currentFile;
    bool m_isModified;
    QLabel *m_statusLabel;
};

#endif // MAIN_WINDOW_H
