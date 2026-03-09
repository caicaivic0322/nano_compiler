#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <QString>

class FileUtils
{
public:
    static bool isPythonFile(const QString &filePath);
    static QString getFileExtension(const QString &filePath);
    static QString readFile(const QString &filePath, bool *ok = nullptr);
    static bool writeFile(const QString &filePath, const QString &content);
    static QString getPythonFileFilter();
};

#endif // FILE_UTILS_H
