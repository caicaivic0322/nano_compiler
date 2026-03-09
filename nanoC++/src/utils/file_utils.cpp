#include "file_utils.h"

#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <QDir>
#include <QSaveFile>
#include <QDebug>

namespace FileUtils {

QString readFileFast(const QString &filePath, bool *ok)
{
    if (ok) *ok = false;
    
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Cannot open file for reading:" << filePath;
        return QString();
    }
    
    // 获取文件大小
    qint64 fileSize = file.size();
    if (fileSize > 100 * 1024 * 1024) { // 100MB 限制
        qWarning() << "File too large:" << filePath;
        return QString();
    }
    
    // 使用内存映射加速大文件读取
    if (fileSize > 1024 * 1024) { // 大于 1MB 使用内存映射
        uchar *memory = file.map(0, fileSize);
        if (memory) {
            QTextCodec::ConverterState state;
            QTextCodec *codec = QTextCodec::codecForName("UTF-8");
            QString text = codec->toUnicode(reinterpret_cast<const char*>(memory), 
                                           static_cast<int>(fileSize), &state);
            file.unmap(memory);
            
            if (state.invalidChars > 0) {
                // 尝试其他编码
                file.seek(0);
                QTextStream stream(&file);
                stream.setAutoDetectUnicode(true);
                text = stream.readAll();
            }
            
            if (ok) *ok = true;
            return text;
        }
    }
    
    // 普通读取方式
    QTextStream stream(&file);
    stream.setAutoDetectUnicode(true);
    
    // 预分配容量
    QString content;
    content.reserve(static_cast<int>(fileSize));
    
    content = stream.readAll();
    file.close();
    
    if (ok) *ok = true;
    return content;
}

bool writeFileFast(const QString &filePath, const QString &content)
{
    // 使用 QSaveFile 确保原子写入
    QSaveFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Cannot open file for writing:" << filePath;
        return false;
    }
    
    // 写入内容
    QByteArray utf8Data = content.toUtf8();
    qint64 written = file.write(utf8Data);
    
    if (written != utf8Data.size()) {
        qWarning() << "Failed to write complete file:" << filePath;
        file.cancelWriting();
        return false;
    }
    
    // 提交更改
    if (!file.commit()) {
        qWarning() << "Failed to commit file:" << filePath;
        return false;
    }
    
    return true;
}

bool fileExists(const QString &filePath)
{
    return QFileInfo::exists(filePath);
}

QString getFileExtension(const QString &filePath)
{
    return QFileInfo(filePath).suffix().toLower();
}

QString getFileName(const QString &filePath)
{
    return QFileInfo(filePath).fileName();
}

QString getFileDir(const QString &filePath)
{
    return QFileInfo(filePath).path();
}

bool createDirIfNotExists(const QString &dirPath)
{
    QDir dir(dirPath);
    if (!dir.exists()) {
        return dir.mkpath(".");
    }
    return true;
}

QStringList getSourceFiles(const QString &dirPath, bool recursive)
{
    QStringList filters;
    filters << "*.c" << "*.cpp" << "*.cc" << "*.cxx" << "*.h" << "*.hpp";
    
    QDir dir(dirPath);
    QStringList files;
    
    if (recursive) {
        files = dir.entryList(filters, QDir::Files | QDir::NoSymLinks, QDir::Name);
        QStringList subdirs = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        for (const QString &subdir : subdirs) {
            files.append(getSourceFiles(dirPath + "/" + subdir, true));
        }
    } else {
        files = dir.entryList(filters, QDir::Files | QDir::NoSymLinks, QDir::Name);
    }
    
    return files;
}

QString formatFileSize(qint64 size)
{
    const QStringList units = {"B", "KB", "MB", "GB"};
    int unitIndex = 0;
    double fileSize = static_cast<double>(size);
    
    while (fileSize >= 1024.0 && unitIndex < units.size() - 1) {
        fileSize /= 1024.0;
        ++unitIndex;
    }
    
    return QString("%1 %2").arg(fileSize, 0, 'f', 2).arg(units[unitIndex]);
}

bool isTextFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }
    
    // 读取前 1024 字节检测
    QByteArray data = file.read(1024);
    file.close();
    
    // 检查是否包含空字节（通常是二进制文件）
    if (data.contains('\0')) {
        return false;
    }
    
    // 检查是否包含大量非打印字符
    int nonPrintable = 0;
    for (char c : data) {
        if (c < 32 && c != '\n' && c != '\r' && c != '\t') {
            ++nonPrintable;
        }
    }
    
    // 如果非打印字符超过 10%，认为是二进制文件
    return (nonPrintable * 100 / data.size()) < 10;
}

QString getSafeFileName(const QString &fileName)
{
    QString safe = fileName;
    // 替换不安全的字符
    safe.replace(QRegularExpression("[<>:\"/\\\\|?*]"), "_");
    // 移除控制字符
    safe.remove(QRegularExpression("[\\x00-\\x1f\\x7f]"));
    return safe;
}

} // namespace FileUtils
