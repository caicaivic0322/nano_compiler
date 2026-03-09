#include "main_window.h"
#include "editor/tab_widget.h"
#include "editor/code_editor.h"
#include "ui/output_panel.h"
#include "ui/project_panel.h"
#include "compiler/tcc_wrapper.h"
#include "utils/file_utils.h"

#include <QApplication>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QCloseEvent>
#include <QSplitter>
#include <QLabel>
#include <QDebug>
#include <QScreen>
#include <QFontDatabase>
#include <QTimer>
#include <QtConcurrent/QtConcurrent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_tabWidget(nullptr)
    , m_outputPanel(nullptr)
    , m_projectPanel(nullptr)
    , m_compiler(std::make_unique<TccWrapper>())
    , m_isModified(false)
    , m_statusLabel(nullptr)
{
    setupUI();
    setupConnections();
    
    // 获取屏幕信息，优化低分辨率显示
    QScreen *screen = QApplication::primaryScreen();
    QRect screenGeometry = screen->availableGeometry();
    qreal dpi = screen->logicalDotsPerInch();
    
    // 根据屏幕尺寸和DPI设置窗口大小
    int defaultWidth = qMin(1200, screenGeometry.width() - 100);
    int defaultHeight = qMin(800, screenGeometry.height() - 100);
    
    // 低分辨率屏幕使用更小的最小尺寸
    int minWidth = (dpi < 96) ? 800 : 900;
    int minHeight = (dpi < 96) ? 600 : 650;
    
    setMinimumSize(minWidth, minHeight);
    resize(defaultWidth, defaultHeight);
    
    // 居中显示
    move(screenGeometry.center() - rect().center());
    
    updateWindowTitle();
    
    // 创建初始标签页
    onNewFile();
    
    // 显示欢迎信息
    m_outputPanel->appendMessage("欢迎使用 nanoC++!", OutputPanel::Info);
    m_outputPanel->appendMessage("点击 '运行' 按钮编译并执行代码", OutputPanel::Info);
    m_outputPanel->appendMessage("提示：最多支持 3 个标签页", OutputPanel::Info);
}

MainWindow::~MainWindow() = default;

void MainWindow::setupUI()
{
    // 设置菜单栏
    setupMenuBar();
    
    // 设置工具栏
    setupToolBar();
    
    // 设置中央部件
    setupCentralWidget();
    
    // 设置状态栏
    setupStatusBar();
}

void MainWindow::setupMenuBar()
{
    QMenuBar *menuBar = this->menuBar();
    
    // 文件菜单
    QMenu *fileMenu = menuBar->addMenu("文件(&F)");
    
    m_newAction = fileMenu->addAction("新建", this, &MainWindow::onNewFile, QKeySequence::New);
    m_newAction->setIcon(QIcon::fromTheme("document-new", QIcon(":/icons/new.png")));
    
    m_openAction = fileMenu->addAction("打开", this, &MainWindow::onOpenFile, QKeySequence::Open);
    m_openAction->setIcon(QIcon::fromTheme("document-open", QIcon(":/icons/open.png")));
    
    m_saveAction = fileMenu->addAction("保存", this, &MainWindow::onSaveFile, QKeySequence::Save);
    m_saveAction->setIcon(QIcon::fromTheme("document-save", QIcon(":/icons/save.png")));
    
    m_saveAsAction = fileMenu->addAction("另存为...", this, &MainWindow::onSaveAs, QKeySequence::SaveAs);
    
    m_saveAllAction = fileMenu->addAction("保存全部", this, &MainWindow::onSaveAll, QKeySequence("Ctrl+Shift+S"));
    
    m_closeTabAction = fileMenu->addAction("关闭标签页", this, &MainWindow::onCloseTab, QKeySequence("Ctrl+W"));
    
    fileMenu->addSeparator();
    fileMenu->addAction("退出", this, &QWidget::close, QKeySequence::Quit);
    
    // 编辑菜单
    QMenu *editMenu = menuBar->addMenu("编辑(&E)");
    m_undoAction = editMenu->addAction("撤销", nullptr, QKeySequence::Undo);
    m_redoAction = editMenu->addAction("重做", nullptr, QKeySequence::Redo);
    editMenu->addSeparator();
    m_cutAction = editMenu->addAction("剪切", nullptr, QKeySequence::Cut);
    m_copyAction = editMenu->addAction("复制", nullptr, QKeySequence::Copy);
    m_pasteAction = editMenu->addAction("粘贴", nullptr, QKeySequence::Paste);
    
    // 视图菜单
    QMenu *viewMenu = menuBar->addMenu("视图(&V)");
    QAction *collapseAllAction = viewMenu->addAction("折叠全部", this, [this]() {
        if (currentEditor()) currentEditor()->collapseAll();
    }, QKeySequence("Ctrl+Shift+A"));
    
    QAction *expandAllAction = viewMenu->addAction("展开全部", this, [this]() {
        if (currentEditor()) currentEditor()->expandAll();
    }, QKeySequence("Ctrl+Shift+E"));
    
    viewMenu->addSeparator();
    QAction *toggleFoldAction = viewMenu->addAction("折叠/展开当前块", this, [this]() {
        if (currentEditor()) {
            int currentLine = currentEditor()->textCursor().blockNumber();
            currentEditor()->toggleFold(currentLine);
        }
    }, QKeySequence("Ctrl+Shift+F"));
    
    // 运行菜单
    QMenu *runMenu = menuBar->addMenu("运行(&R)");
    m_runAction = runMenu->addAction("运行", this, &MainWindow::onRun, QKeySequence("Ctrl+R"));
    m_runAction->setIcon(QIcon::fromTheme("media-playback-start", QIcon(":/icons/run.png")));
    
    m_stopAction = runMenu->addAction("停止", this, &MainWindow::onStop, QKeySequence("Ctrl+Shift+F5"));
    m_stopAction->setIcon(QIcon::fromTheme("media-playback-stop", QIcon(":/icons/stop.png")));
    m_stopAction->setEnabled(false);
    
    // 帮助菜单
    QMenu *helpMenu = menuBar->addMenu("帮助(&H)");
    helpMenu->addAction("关于", this, &MainWindow::onAbout);
}

void MainWindow::setupToolBar()
{
    QToolBar *toolBar = addToolBar("工具栏");
    toolBar->setMovable(false);
    
    // 添加动作到工具栏
    toolBar->addAction(m_newAction);
    toolBar->addAction(m_openAction);
    toolBar->addAction(m_saveAction);
    toolBar->addSeparator();
    toolBar->addAction(m_runAction);
    toolBar->addAction(m_stopAction);
}

void MainWindow::setupCentralWidget()
{
    // 创建主分割器
    QSplitter *mainSplitter = new QSplitter(Qt::Horizontal, this);
    
    // 项目面板
    m_projectPanel = new ProjectPanel(this);
    m_projectPanel->setMinimumWidth(150);
    m_projectPanel->setMaximumWidth(300);
    connect(m_projectPanel, &ProjectPanel::fileSelected, this, &MainWindow::onFileSelected);
    mainSplitter->addWidget(m_projectPanel);
    
    // 创建右侧垂直分割器
    QSplitter *rightSplitter = new QSplitter(Qt::Vertical, this);
    
    // 标签页控件
    m_tabWidget = new TabWidget(this);
    connect(m_tabWidget, &TabWidget::tabChanged, this, &MainWindow::onTabChanged);
    connect(m_tabWidget, &TabWidget::tabModified, this, &MainWindow::onTabModified);
    connect(m_tabWidget, &TabWidget::maxTabsReached, this, [this]() {
        m_statusLabel->setText("已达到最大标签页数量 (3)");
    });
    rightSplitter->addWidget(m_tabWidget);
    
    // 输出面板
    m_outputPanel = new OutputPanel(this);
    m_outputPanel->setMinimumHeight(100);
    m_outputPanel->setMaximumHeight(300);
    rightSplitter->addWidget(m_outputPanel);
    
    // 设置分割器比例
    rightSplitter->setStretchFactor(0, 3);
    rightSplitter->setStretchFactor(1, 1);
    
    mainSplitter->addWidget(rightSplitter);
    
    // 设置主分割器比例
    mainSplitter->setStretchFactor(0, 0);
    mainSplitter->setStretchFactor(1, 1);
    mainSplitter->setSizes({200, 800});
    
    setCentralWidget(mainSplitter);
}

void MainWindow::setupStatusBar()
{
    QStatusBar *statusBar = this->statusBar();
    m_statusLabel = new QLabel("就绪", this);
    statusBar->addWidget(m_statusLabel);
}

void MainWindow::setupConnections()
{
    // 连接编译器信号
    connect(m_compiler.get(), &TccWrapper::compilationStarted, this, [this]() {
        m_runAction->setEnabled(false);
        m_stopAction->setEnabled(true);
        m_statusLabel->setText("正在编译...");
    });
    
    connect(m_compiler.get(), &TccWrapper::outputReceived, this, [this](const QString &output) {
        m_outputPanel->appendMessage(output, OutputPanel::Normal);
    });
    
    connect(m_compiler.get(), &TccWrapper::compilationFinished, this, &MainWindow::onCompilationFinished);
}

void MainWindow::onNewFile()
{
    CodeEditor *editor = m_tabWidget->createNewTab();
    if (editor) {
        // 插入默认的 C++ 框架代码
        QString defaultCode = R"(#include <iostream>

using namespace std;

int main() {
    cout << "Hello, World!" << endl;
    return 0;
}
)";
        editor->setPlainText(defaultCode);
        editor->document()->setModified(false);
        
        m_statusLabel->setText(QString("已创建新标签页 (%1/3)").arg(m_tabWidget->tabCount()));
    }
}

void MainWindow::onOpenFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, 
        "打开文件", 
        QString(),
        "C/C++ 文件 (*.c *.cpp *.h *.hpp);;所有文件 (*)");
    
    if (!fileName.isEmpty()) {
        if (m_tabWidget->openFileInNewTab(fileName)) {
            m_statusLabel->setText("文件已打开: " + QFileInfo(fileName).fileName());
        } else {
            QMessageBox::warning(this, "错误", "无法打开文件: " + fileName);
        }
    }
}

void MainWindow::onSaveFile()
{
    CodeEditor *editor = currentEditor();
    if (!editor) return;
    
    int currentIndex = m_tabWidget->currentIndex();
    QString filePath = m_tabWidget->getTabFilePath(currentIndex);
    
    if (filePath.isEmpty()) {
        onSaveAs();
    } else {
        if (saveFile(filePath, editor->toPlainText())) {
            editor->document()->setModified(false);
            m_statusLabel->setText("文件已保存: " + QFileInfo(filePath).fileName());
        } else {
            QMessageBox::warning(this, "错误", "无法保存文件: " + filePath);
        }
    }
}

void MainWindow::onSaveAs()
{
    CodeEditor *editor = currentEditor();
    if (!editor) return;
    
    QString fileName = QFileDialog::getSaveFileName(this,
        "保存文件",
        QString(),
        "C++ 文件 (*.cpp);;C 文件 (*.c);;头文件 (*.h);;所有文件 (*)");
    
    if (!fileName.isEmpty()) {
        if (saveFile(fileName, editor->toPlainText())) {
            m_tabWidget->setTabFilePath(m_tabWidget->currentIndex(), fileName);
            editor->document()->setModified(false);
            
            // 设置文件类型用于语法高亮
            QString extension = QFileInfo(fileName).suffix().toLower();
            editor->setFileType(extension);
            
            m_statusLabel->setText("文件已保存: " + QFileInfo(fileName).fileName());
        } else {
            QMessageBox::warning(this, "错误", "无法保存文件: " + fileName);
        }
    }
}

void MainWindow::onSaveAll()
{
    int savedCount = 0;
    for (int i = 0; i < m_tabWidget->tabCount(); ++i) {
        CodeEditor *editor = m_tabWidget->editorAt(i);
        if (editor && editor->document()->isModified()) {
            QString filePath = m_tabWidget->getTabFilePath(i);
            if (!filePath.isEmpty()) {
                if (saveFile(filePath, editor->toPlainText())) {
                    editor->document()->setModified(false);
                    savedCount++;
                }
            }
        }
    }
    m_statusLabel->setText(QString("已保存 %1 个文件").arg(savedCount));
}

void MainWindow::onCloseTab()
{
    m_tabWidget->closeCurrentTab();
}

void MainWindow::onRun()
{
    CodeEditor *editor = currentEditor();
    if (!editor) return;
    
    // 如果有未保存的更改，先保存
    if (editor->document()->isModified()) {
        onSaveFile();
    }
    
    QString filePath = m_tabWidget->getTabFilePath(m_tabWidget->currentIndex());
    QString sourceCode = editor->toPlainText();
    
    m_outputPanel->clear();
    m_compiler->compile(filePath, sourceCode);
}

void MainWindow::onStop()
{
    m_compiler->stop();
    m_statusLabel->setText("编译已停止");
}

void MainWindow::onAbout()
{
    QMessageBox::about(this, "关于 nanoC++",
        "<h2>nanoC++ 1.0.0</h2>"
        "<p>轻量级 C++ IDE</p>"
        "<p>专为低分辨率显示器和性能一般的电脑优化</p>"
        "<p>支持多标签页（最多3个）</p>"
        "<p>内置 TinyCC 编译器，开箱即用</p>");
}

void MainWindow::onNewProject()
{
    // TODO: 实现新建项目功能
    QMessageBox::information(this, "提示", "新建项目功能即将推出");
}

void MainWindow::onOpenProject()
{
    // TODO: 实现打开项目功能
    QMessageBox::information(this, "提示", "打开项目功能即将推出");
}

void MainWindow::onFileSelected(const QString &filePath)
{
    if (QFile::exists(filePath)) {
        m_tabWidget->openFileInNewTab(filePath);
    }
}

void MainWindow::onCompilationFinished(bool success, const QString &output)
{
    m_runAction->setEnabled(true);
    m_stopAction->setEnabled(false);
    
    if (success) {
        m_statusLabel->setText("编译成功");
    } else {
        m_statusLabel->setText("编译失败");
    }
}

void MainWindow::onTabChanged(int index)
{
    Q_UNUSED(index)
    updateWindowTitle();
    updateActions();
}

void MainWindow::onTabModified(int index, bool modified)
{
    Q_UNUSED(index)
    Q_UNUSED(modified)
    updateWindowTitle();
}

bool MainWindow::maybeSave()
{
    return m_tabWidget->closeCurrentTab();
}

bool MainWindow::maybeSaveAll()
{
    if (m_tabWidget->hasUnsavedChanges()) {
        QMessageBox::StandardButton reply = QMessageBox::question(this, "保存更改",
            "有未保存的文件，是否保存所有更改？",
            QMessageBox::SaveAll | QMessageBox::Discard | QMessageBox::Cancel);
        
        if (reply == QMessageBox::SaveAll) {
            return m_tabWidget->saveAllTabs();
        } else if (reply == QMessageBox::Cancel) {
            return false;
        }
    }
    return true;
}

void MainWindow::updateWindowTitle()
{
    QString title = "nanoC++";
    
    CodeEditor *editor = currentEditor();
    if (editor) {
        int index = m_tabWidget->currentIndex();
        QString filePath = m_tabWidget->getTabFilePath(index);
        QString fileName = filePath.isEmpty() ? m_tabWidget->tabText(index) : QFileInfo(filePath).fileName();
        
        if (editor->document()->isModified()) {
            fileName = "*" + fileName;
        }
        
        title = fileName + " - " + title;
    }
    
    setWindowTitle(title);
}

void MainWindow::updateActions()
{
    CodeEditor *editor = currentEditor();
    bool hasEditor = (editor != nullptr);
    
    m_saveAction->setEnabled(hasEditor);
    m_saveAsAction->setEnabled(hasEditor);
    m_runAction->setEnabled(hasEditor);
    m_closeTabAction->setEnabled(hasEditor);
}

CodeEditor* MainWindow::currentEditor() const
{
    return m_tabWidget->currentEditor();
}

bool MainWindow::saveFile(const QString &filePath, const QString &content)
{
    bool success = FileUtils::writeFileFast(filePath, content);
    return success;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSaveAll()) {
        event->accept();
    } else {
        event->ignore();
    }
}
