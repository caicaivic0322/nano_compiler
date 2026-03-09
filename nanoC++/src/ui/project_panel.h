#ifndef PROJECT_PANEL_H
#define PROJECT_PANEL_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QTreeView;
class QFileSystemModel;
class QVBoxLayout;
class QLabel;
QT_END_NAMESPACE

class ProjectPanel : public QWidget
{
    Q_OBJECT

public:
    explicit ProjectPanel(QWidget *parent = nullptr);

    void setProjectPath(const QString &path);
    QString projectPath() const;

signals:
    void fileSelected(const QString &filePath);

private slots:
    void onItemClicked(const QModelIndex &index);
    void onItemDoubleClicked(const QModelIndex &index);

private:
    void setupUI();
    void setupModel();

    QTreeView *m_treeView;
    QFileSystemModel *m_fileModel;
    QLabel *m_titleLabel;
    QString m_projectPath;
};

#endif // PROJECT_PANEL_H
