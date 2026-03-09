#include "project_panel.h"

#include <QTreeView>
#include <QFileSystemModel>
#include <QVBoxLayout>
#include <QLabel>
#include <QHeaderView>
#include <QDebug>

ProjectPanel::ProjectPanel(QWidget *parent)
    : QWidget(parent)
    , m_treeView(new QTreeView(this))
    , m_fileModel(new QFileSystemModel(this))
    , m_titleLabel(new QLabel("📁 项目文件", this))
{
    setupUI();
    setupModel();
}

void ProjectPanel::setupUI()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(8, 8, 8, 8);
    layout->setSpacing(8);
    
    // 标题标签
    m_titleLabel->setStyleSheet(
        "QLabel {"
        "  font-size: 14px;"
        "  font-weight: bold;"
        "  color: #2196F3;"
        "  padding: 8px;"
        "  border-bottom: 2px solid #E3F2FD;"
        "}"
    );
    layout->addWidget(m_titleLabel);
    
    // 树形视图
    m_treeView->setStyleSheet(
        "QTreeView {"
        "  background-color: #ffffff;"
        "  border: 1px solid #e0e0e0;"
        "  border-radius: 8px;"
        "  padding: 4px;"
        "  outline: none;"
        "}"
        "QTreeView::item {"
        "  padding: 6px;"
        "  border-radius: 4px;"
        "}"
        "QTreeView::item:selected {"
        "  background-color: #E3F2FD;"
        "  color: #2196F3;"
        "}"
        "QTreeView::item:hover {"
        "  background-color: #f5f5f5;"
        "}"
    );
    
    m_treeView->setHeaderHidden(true);
    m_treeView->setAnimated(true);
    m_treeView->setIndentation(20);
    m_treeView->setSortingEnabled(true);
    
    layout->addWidget(m_treeView);
    
    // 连接信号
    connect(m_treeView, &QTreeView::clicked, this, &ProjectPanel::onItemClicked);
    connect(m_treeView, &QTreeView::doubleClicked, this, &ProjectPanel::onItemDoubleClicked);
}

void ProjectPanel::setupModel()
{
    // 设置过滤器，只显示代码文件
    QStringList filters;
    filters << "*.c" << "*.cpp" << "*.h" << "*.hpp" << "*.cc" << "*.cxx";
    m_fileModel->setNameFilters(filters);
    m_fileModel->setNameFilterDisables(false);
    
    m_treeView->setModel(m_fileModel);
    
    // 隐藏不需要的列，只显示文件名
    for (int i = 1; i < m_fileModel->columnCount(); ++i) {
        m_treeView->hideColumn(i);
    }
}

void ProjectPanel::setProjectPath(const QString &path)
{
    if (path.isEmpty()) {
        return;
    }
    
    m_projectPath = path;
    
    // 设置根索引
    QModelIndex rootIndex = m_fileModel->setRootPath(path);
    m_treeView->setRootIndex(rootIndex);
    
    // 展开根节点
    m_treeView->expand(rootIndex);
    
    // 更新标题
    QString folderName = QFileInfo(path).fileName();
    m_titleLabel->setText(QString("📁 %1").arg(folderName));
}

QString ProjectPanel::projectPath() const
{
    return m_projectPath;
}

void ProjectPanel::onItemClicked(const QModelIndex &index)
{
    if (!index.isValid()) {
        return;
    }
    
    QString filePath = m_fileModel->filePath(index);
    QFileInfo fileInfo(filePath);
    
    if (fileInfo.isFile()) {
        emit fileSelected(filePath);
    }
}

void ProjectPanel::onItemDoubleClicked(const QModelIndex &index)
{
    if (!index.isValid()) {
        return;
    }
    
    QString filePath = m_fileModel->filePath(index);
    QFileInfo fileInfo(filePath);
    
    if (fileInfo.isFile()) {
        emit fileSelected(filePath);
    } else if (fileInfo.isDir()) {
        // 切换项目路径
        setProjectPath(filePath);
    }
}
