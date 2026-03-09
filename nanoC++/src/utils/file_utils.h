#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <QString>
#include <QStringList>
#include <QRegularExpression>

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include <QTextCodec>
#endif

namespace FileUtils {

// 快速读取文件内容
QString readFileFast(const QString &filePath, bool *ok = nullptr);

// 快速写入文件内容（原子写入）
bool writeFileFast(const QString &filePath, const QString &content);

// 检查文件是否存在
bool fileExists(const QString &filePath);

// 获取文件扩展名
QString getFileExtension(const QString &filePath);

// 获取文件名（不含路径）
QString getFileName(const QString &filePath);

// 获取文件所在目录
QString getFileDir(const QString &filePath);

// 创建目录（如果不存在）
bool createDirIfNotExists(const QString &dirPath);

// 获取源代码文件列表
QStringList getSourceFiles(const QString &dirPath, bool recursive = false);

// 格式化文件大小
QString formatFileSize(qint64 size);

// 检查是否为文本文件
bool isTextFile(const QString &filePath);

// 获取安全的文件名（移除非法字符）
QString getSafeFileName(const QString &fileName);

} // namespace FileUtils

#endif // FILE_UTILS_H
