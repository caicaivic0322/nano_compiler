#include "tab_widget.h"
#include "code_editor.h"

#include <QMessageBox>
#include <QFileInfo>
#include <QFile>
#include <QTextStream>

TabWidget::TabWidget(QWidget *parent)
    : QTabWidget(parent)
    , m_newTabCounter(1)
{
    // 设置标签页样式
    setTabsClosable(true);
    setMovable(true);
    setDocumentMode(true);
    
    // 连接信号
    connect(this, &QTabWidget::tabCloseRequested, this, &TabWidget::onTabCloseRequested);
    connect(this, &QTabWidget::currentChanged, this, &TabWidget::onCurrentChanged);
}

TabWidget::~TabWidget() = default;

CodeEditor* TabWidget::createNewTab(const QString &fileName)
{
    // 检查是否达到最大标签页数
    if (count() >= MAX_TABS) {
        emit maxTabsReached();
        QMessageBox::information(this, "提示", 
            QString("最多只能打开 %1 个标签页，请先关闭一些标签页。").arg(MAX_TABS));
        return nullptr;
    }
    
    // 创建新的代码编辑器
    CodeEditor *editor = new CodeEditor(this);
    
    // 确定标签页标题
    QString tabName = fileName.isEmpty() ? generateNewTabName() : QFileInfo(fileName).fileName();
    
    // 添加标签页
    int index = addTab(editor, tabName);
    
    // 保存标签页信息
    TabInfo info;
    info.filePath = fileName;
    info.fileName = tabName;
    info.isModified = false;
    info.index = index;
    m_tabInfo.append(info);
    
    // 连接修改信号
    connect(editor, &QPlainTextEdit::modificationChanged, this, &TabWidget::onTextModified);
    
    // 切换到新标签页
    setCurrentIndex(index);
    
    emit tabCreated(index);
    
    return editor;
}

bool TabWidget::closeTab(int index)
{
    if (index < 0 || index >= count()) {
        return false;
    }
    
    CodeEditor *editor = editorAt(index);
    if (!editor) {
        return false;
    }
    
    // 检查是否有未保存的更改
    if (editor->document()->isModified()) {
        QString fileName = tabText(index);
        if (fileName.startsWith("*")) {
            fileName = fileName.mid(1);
        }
        
        QMessageBox::StandardButton reply = QMessageBox::question(this, "保存更改",
            QString("文件 '%1' 有未保存的更改，是否保存？").arg(fileName),
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        
        if (reply == QMessageBox::Save) {
            if (!saveTab(index)) {
                return false; // 保存失败，不关闭
            }
        } else if (reply == QMessageBox::Cancel) {
            return false; // 用户取消
        }
    }
    
    // 移除标签页信息
    if (index < m_tabInfo.size()) {
        m_tabInfo.removeAt(index);
    }
    
    // 移除标签页
    QWidget *tabWidget = this->widget(index);
    removeTab(index);
    delete tabWidget;
    
    // 更新剩余标签页的索引
    for (int i = 0; i < m_tabInfo.size(); ++i) {
        m_tabInfo[i].index = i;
    }
    
    emit tabClosed(index);
    
    // 如果所有标签页都关闭了，创建一个新的空标签页
    if (count() == 0) {
        m_newTabCounter = 1;
        createNewTab();
        emit allTabsClosed();
    }
    
    return true;
}

bool TabWidget::closeCurrentTab()
{
    return closeTab(currentIndex());
}

CodeEditor* TabWidget::currentEditor() const
{
    return qobject_cast<CodeEditor*>(currentWidget());
}

CodeEditor* TabWidget::editorAt(int index) const
{
    return qobject_cast<CodeEditor*>(widget(index));
}

bool TabWidget::openFileInNewTab(const QString &filePath)
{
    // 检查文件是否已经在某个标签页中打开
    for (int i = 0; i < count(); ++i) {
        if (m_tabInfo[i].filePath == filePath) {
            setCurrentIndex(i);
            return true;
        }
    }
    
    // 创建新标签页
    CodeEditor *editor = createNewTab(filePath);
    if (!editor) {
        return false;
    }
    
    // 读取文件内容
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }
    
    QTextStream stream(&file);
    stream.setAutoDetectUnicode(true);
    QString content = stream.readAll();
    file.close();
    
    // 设置编辑器内容
    editor->setPlainText(content);
    editor->document()->setModified(false);
    
    // 设置文件类型用于语法高亮
    QString extension = QFileInfo(filePath).suffix().toLower();
    editor->setFileType(extension);
    
    // 更新标签页信息
    int index = currentIndex();
    m_tabInfo[index].filePath = filePath;
    m_tabInfo[index].fileName = QFileInfo(filePath).fileName();
    updateTabTitle(index);
    
    return true;
}

bool TabWidget::saveCurrentTab()
{
    return saveTab(currentIndex());
}

bool TabWidget::saveTab(int index)
{
    CodeEditor *editor = editorAt(index);
    if (!editor) {
        return false;
    }
    
    QString filePath = m_tabInfo[index].filePath;
    
    // 如果是新文件，需要选择保存位置
    if (filePath.isEmpty()) {
        // 这里应该调用文件对话框，暂时返回 false
        // 实际实现中应该使用 QFileDialog
        return false;
    }
    
    // 保存文件
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }
    
    QTextStream stream(&file);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    stream.setCodec("UTF-8");
#endif
    stream << editor->toPlainText();
    file.close();
    
    // 标记为已保存
    editor->document()->setModified(false);
    m_tabInfo[index].isModified = false;
    updateTabTitle(index);
    
    return true;
}

bool TabWidget::saveAllTabs()
{
    bool allSaved = true;
    for (int i = 0; i < count(); ++i) {
        if (editorAt(i) && editorAt(i)->document()->isModified()) {
            if (!saveTab(i)) {
                allSaved = false;
            }
        }
    }
    return allSaved;
}

bool TabWidget::hasUnsavedChanges() const
{
    for (int i = 0; i < count(); ++i) {
        CodeEditor *editor = editorAt(i);
        if (editor && editor->document()->isModified()) {
            return true;
        }
    }
    return false;
}

QString TabWidget::getTabFilePath(int index) const
{
    if (index >= 0 && index < m_tabInfo.size()) {
        return m_tabInfo[index].filePath;
    }
    return QString();
}

void TabWidget::setTabFilePath(int index, const QString &filePath)
{
    if (index >= 0 && index < m_tabInfo.size()) {
        m_tabInfo[index].filePath = filePath;
        m_tabInfo[index].fileName = QFileInfo(filePath).fileName();
        updateTabTitle(index);
    }
}

void TabWidget::onTabCloseRequested(int index)
{
    closeTab(index);
}

void TabWidget::onCurrentChanged(int index)
{
    if (index >= 0) {
        emit tabChanged(index);
    }
}

void TabWidget::onTextModified()
{
    CodeEditor *editor = qobject_cast<CodeEditor*>(sender());
    if (!editor) return;
    
    int index = indexOf(editor);
    if (index < 0) return;
    
    bool modified = editor->document()->isModified();
    m_tabInfo[index].isModified = modified;
    
    updateTabTitle(index);
    
    emit tabModified(index, modified);
}

void TabWidget::updateTabTitle(int index)
{
    if (index < 0 || index >= m_tabInfo.size()) return;
    
    QString title = m_tabInfo[index].fileName;
    if (m_tabInfo[index].isModified) {
        title = "*" + title;
    }
    
    setTabText(index, title);
    setTabToolTip(index, m_tabInfo[index].filePath.isEmpty() ? title : m_tabInfo[index].filePath);
}

QString TabWidget::generateNewTabName()
{
    return QString("未命名 %1").arg(m_newTabCounter++);
}
