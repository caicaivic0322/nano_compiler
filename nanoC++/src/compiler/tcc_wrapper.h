#ifndef TCC_WRAPPER_H
#define TCC_WRAPPER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <memory>

QT_BEGIN_NAMESPACE
class QProcess;
class QTemporaryDir;
QT_END_NAMESPACE

struct CompilerSettings {
    QString tccPath;
    QStringList includePaths;
    QStringList libPaths;
    QStringList libraries;
    int optimizationLevel = 0;
    bool enableWarnings = true;
    bool treatWarningsAsErrors = false;
};

class TccWrapper : public QObject
{
    Q_OBJECT

public:
    explicit TccWrapper(QObject *parent = nullptr);
    ~TccWrapper();

    void compile(const QString &filePath, const QString &sourceCode);
    void stop();
    bool isCompiling() const;

    // 设置相关
    QString getTccPath() const;
    void setTccPath(const QString &path);
    CompilerSettings getSettings() const;
    void saveSettings(const CompilerSettings &settings);

signals:
    void compilationFinished(bool success, const QString &output);
    void compilationStarted();
    void outputReceived(const QString &output);

private slots:
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onProcessError(QProcess::ProcessError error);
    void onReadyReadStandardOutput();
    void onReadyReadStandardError();

private:
    bool setupTcc();
    QString getOutputPath(const QString &sourcePath) const;
    void runCompiledProgram();

    std::unique_ptr<QProcess> m_process;
    std::unique_ptr<QTemporaryDir> m_tempDir;
    QString m_currentSourceFile;
    QString m_lastOutputPath;
    bool m_isCompiling;
};

#endif // TCC_WRAPPER_H
