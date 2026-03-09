#include "project_panel.h"

#include <QTreeView>
#include <QFileSystemModel>
#include <QVBoxLayout>
#include <QLabel>
#include <QHeaderView>

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

    m_titleLabel->setStyleSheet(
        "QLabel {"
        "  font-size: 14px;"
        "  font-weight: bold;"
        "  color: #4CAF50;"
        "  padding: 8px;"
        "  border-bottom: 2px solid #E8F5E9;"
        "}"
    );
    layout->addWidget(m_titleLabel);

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
        "  background-color: #E8F5E9;"
        "  color: #4CAF50;"
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

    connect(m_treeView, &QTreeView::clicked, this, &ProjectPanel::onItemClicked);
    connect(m_treeView, &QTreeView::doubleClicked, this, &ProjectPanel::onItemDoubleClicked);
}

void ProjectPanel::setupModel()
{
    QStringList filters;
    filters << "*.py";
    m_fileModel->setNameFilters(filters);
    m_fileModel->setNameFilterDisables(false);

    m_treeView->setModel(m_fileModel);

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

    QModelIndex rootIndex = m_fileModel->setRootPath(path);
    m_treeView->setRootIndex(rootIndex);
    m_treeView->expand(rootIndex);

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
        setProjectPath(filePath);
    }
}
