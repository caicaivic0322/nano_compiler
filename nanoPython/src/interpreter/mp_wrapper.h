#ifndef MP_WRAPPER_H
#define MP_WRAPPER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <memory>

QT_BEGIN_NAMESPACE
class QProcess;
class QTemporaryDir;
QT_END_NAMESPACE

struct InterpreterSettings {
    QString pythonPath;
    QStringList modulePaths;
    QStringList environmentVariables;
    int timeoutSeconds = 30;
    bool enableOptimization = false;
    bool showBytecode = false;
};

class MpWrapper : public QObject
{
    Q_OBJECT

public:
    explicit MpWrapper(QObject *parent = nullptr);
    ~MpWrapper();

    void execute(const QString &filePath, const QString &sourceCode);
    void executeInteractive(const QString &code);
    void stop();
    bool isRunning() const;

    // 设置相关
    QString getPythonPath() const;
    void setPythonPath(const QString &path);
    InterpreterSettings getSettings() const;
    void saveSettings(const InterpreterSettings &settings);

    // 检查 MicroPython 是否可用
    bool isMicroPythonAvailable() const;
    QString getVersion() const;

signals:
    void executionFinished(bool success, const QString &output);
    void executionStarted();
    void outputReceived(const QString &output);
    void errorReceived(const QString &error);

private slots:
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onProcessError(QProcess::ProcessError error);
    void onReadyReadStandardOutput();
    void onReadyReadStandardError();

private:
    bool setupPython();
    QString findMicroPython() const;

    std::unique_ptr<QProcess> m_process;
    std::unique_ptr<QTemporaryDir> m_tempDir;
    QString m_currentSourceFile;
    bool m_isRunning;
    bool m_isInteractive;
};

#endif // MP_WRAPPER_H
