#ifndef TAB_WIDGET_H
#define TAB_WIDGET_H

#include <QTabWidget>
#include <QVector>
#include <memory>

class CodeEditor;
class QString;

struct TabInfo {
    QString filePath;
    QString fileName;
    bool isModified;
    int index;
};

class TabWidget : public QTabWidget
{
    Q_OBJECT

public:
    explicit TabWidget(QWidget *parent = nullptr);
    ~TabWidget();

    // 标签页管理
    CodeEditor* createNewTab(const QString &fileName = QString());
    bool closeTab(int index);
    bool closeCurrentTab();
    CodeEditor* currentEditor() const;
    CodeEditor* editorAt(int index) const;
    
    // 文件操作
    bool openFileInNewTab(const QString &filePath);
    bool saveCurrentTab();
    bool saveTab(int index);
    bool saveAllTabs();
    
    // 查询
    int tabCount() const { return count(); }
    bool hasUnsavedChanges() const;
    bool canCreateNewTab() const { return count() < MAX_TABS; }
    QString getTabFilePath(int index) const;
    void setTabFilePath(int index, const QString &filePath);
    
    // 常量
    static const int MAX_TABS = 3;

signals:
    void tabCreated(int index);
    void tabClosed(int index);
    void tabChanged(int index);
    void tabModified(int index, bool modified);
    void allTabsClosed();
    void maxTabsReached();

private slots:
    void onTabCloseRequested(int index);
    void onCurrentChanged(int index);
    void onTextModified();

private:
    void updateTabTitle(int index);
    QString generateNewTabName();
    
    QVector<TabInfo> m_tabInfo;
    int m_newTabCounter;
};

#endif // TAB_WIDGET_H
