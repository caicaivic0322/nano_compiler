#include "file_utils.h"

#include <QFile>
#include <QTextStream>
#include <QFileInfo>

bool FileUtils::isPythonFile(const QString &filePath)
{
    return getFileExtension(filePath).toLower() == "py";
}

QString FileUtils::getFileExtension(const QString &filePath)
{
    return QFileInfo(filePath).suffix();
}

QString FileUtils::readFile(const QString &filePath, bool *ok)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        if (ok) *ok = false;
        return QString();
    }

    QTextStream stream(&file);
    QString content = stream.readAll();
    file.close();

    if (ok) *ok = true;
    return content;
}

bool FileUtils::writeFile(const QString &filePath, const QString &content)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream stream(&file);
    stream << content;
    file.close();

    return true;
}

QString FileUtils::getPythonFileFilter()
{
    return "Python 文件 (*.py)";
}
