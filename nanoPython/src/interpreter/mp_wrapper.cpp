#include "mp_wrapper.h"

#include <QProcess>
#include <QTemporaryDir>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QDir>
#include <QCoreApplication>
#include <QSettings>
#include <QStandardPaths>

MpWrapper::MpWrapper(QObject *parent)
    : QObject(parent)
    , m_process(std::make_unique<QProcess>(this))
    , m_isRunning(false)
    , m_isInteractive(false)
{
    setupPython();
    
    // 连接进程信号
    connect(m_process.get(), QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &MpWrapper::onProcessFinished);
    connect(m_process.get(), &QProcess::errorOccurred,
            this, &MpWrapper::onProcessError);
    connect(m_process.get(), &QProcess::readyReadStandardOutput,
            this, &MpWrapper::onReadyReadStandardOutput);
    connect(m_process.get(), &QProcess::readyReadStandardError,
            this, &MpWrapper::onReadyReadStandardError);
}

MpWrapper::~MpWrapper()
{
    stop();
}

bool MpWrapper::setupPython()
{
    // 使用相对于应用程序的固定路径
    QString appDir = QCoreApplication::applicationDirPath();
    QString pythonPath;
    
#ifdef Q_OS_WIN
    // Windows: 使用 bundled 的 MicroPython
    pythonPath = appDir + "/micropython/micropython.exe";
    
    // 如果找不到，尝试上级目录（开发环境）
    if (!QFile::exists(pythonPath)) {
        pythonPath = appDir + "/../third_party/micropython/micropython.exe";
    }
#else
    // Linux/macOS
    pythonPath = appDir + "/micropython/micropython";
    if (!QFile::exists(pythonPath)) {
        pythonPath = appDir + "/../third_party/micropython/micropython";
    }
    // 如果 bundled 不存在，使用系统安装的
    if (!QFile::exists(pythonPath)) {
        pythonPath = "/usr/bin/micropython";
    }
#endif
    
    // 保存到设置
    QSettings settings("nanoPython", "Interpreter");
    settings.setValue("pythonPath", pythonPath);
    
    return QFile::exists(pythonPath);
}

QString MpWrapper::getPythonPath() const
{
    QSettings settings("nanoPython", "Interpreter");
    QString path = settings.value("pythonPath").toString();
    
    // 如果设置的路径不存在，重新检测
    if (path.isEmpty() || !QFile::exists(path)) {
        const_cast<MpWrapper*>(this)->setupPython();
        path = settings.value("pythonPath").toString();
    }
    
    return path;
}

void MpWrapper::setPythonPath(const QString &path)
{
    QSettings settings("nanoPython", "Interpreter");
    settings.setValue("pythonPath", path);
}

bool MpWrapper::isMicroPythonAvailable() const
{
    QString path = getPythonPath();
    return !path.isEmpty() && QFile::exists(path);
}

QString MpWrapper::getVersion() const
{
    QString pythonPath = getPythonPath();
    if (pythonPath.isEmpty() || !QFile::exists(pythonPath)) {
        return QString();
    }
    
    QProcess process;
    process.start(pythonPath, QStringList() << "--version");
    process.waitForFinished(5000);
    
    QString output = QString::fromLocal8Bit(process.readAllStandardOutput());
    QString error = QString::fromLocal8Bit(process.readAllStandardError());
    
    if (!output.isEmpty()) {
        return output.trimmed();
    } else if (!error.isEmpty()) {
        return error.trimmed();
    }
    
    return QString();
}

void MpWrapper::execute(const QString &filePath, const QString &sourceCode)
{
    if (m_isRunning) {
        emit errorReceived("错误：解释器正在运行，请先停止当前执行");
        return;
    }
    
    QString pythonPath = getPythonPath();
    if (pythonPath.isEmpty() || !QFile::exists(pythonPath)) {
        emit executionFinished(false, "错误：找不到 MicroPython 解释器\n"
                                     "请重新安装 nanoPython");
        return;
    }
    
    m_isRunning = true;
    m_isInteractive = false;
    m_currentSourceFile = filePath;
    
    // 创建临时目录
    m_tempDir = std::make_unique<QTemporaryDir>();
    if (!m_tempDir->isValid()) {
        emit executionFinished(false, "错误：无法创建临时目录");
        m_isRunning = false;
        return;
    }
    
    // 写入源代码到临时文件
    QString tempSourcePath;
    if (filePath.isEmpty()) {
        tempSourcePath = m_tempDir->filePath("temp_script.py");
    } else {
        QFileInfo fileInfo(filePath);
        tempSourcePath = m_tempDir->filePath(fileInfo.fileName());
    }
    
    QFile sourceFile(tempSourcePath);
    if (!sourceFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        emit executionFinished(false, "错误：无法创建临时源文件");
        m_isRunning = false;
        return;
    }
    
    QTextStream stream(&sourceFile);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    stream.setCodec("UTF-8");
#endif
    stream << sourceCode;
    sourceFile.close();
    
    // 构建执行命令
    QStringList arguments;
    arguments << tempSourcePath;
    
    emit executionStarted();
    emit outputReceived("开始执行...");
    emit outputReceived("命令: " + pythonPath + " " + arguments.join(" "));
    emit outputReceived("");
    
    // 启动执行进程
    m_process->setWorkingDirectory(m_tempDir->path());
    m_process->start(pythonPath, arguments);
    
    if (!m_process->waitForStarted(5000)) {
        emit executionFinished(false, "错误：无法启动解释器进程");
        m_isRunning = false;
    }
}

void MpWrapper::executeInteractive(const QString &code)
{
    if (m_isRunning) {
        emit errorReceived("错误：解释器正在运行");
        return;
    }
    
    QString pythonPath = getPythonPath();
    if (pythonPath.isEmpty() || !QFile::exists(pythonPath)) {
        emit errorReceived("错误：找不到 MicroPython 解释器");
        return;
    }
    
    m_isRunning = true;
    m_isInteractive = true;
    
    // 交互式执行，使用 -c 参数
    QStringList arguments;
    arguments << "-c" << code;
    
    m_process->start(pythonPath, arguments);
    
    if (!m_process->waitForStarted(5000)) {
        emit errorReceived("错误：无法启动解释器进程");
        m_isRunning = false;
    }
}

void MpWrapper::stop()
{
    if (m_process->state() != QProcess::NotRunning) {
        m_process->terminate();
        if (!m_process->waitForFinished(3000)) {
            m_process->kill();
        }
    }
    m_isRunning = false;
}

bool MpWrapper::isRunning() const
{
    return m_isRunning;
}

void MpWrapper::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    m_isRunning = false;
    
    if (exitStatus == QProcess::CrashExit) {
        emit executionFinished(false, "解释器进程崩溃");
        return;
    }
    
    if (exitCode == 0) {
        emit outputReceived("\n执行完成");
        emit executionFinished(true, "执行成功");
    } else {
        emit executionFinished(false, "执行失败 (退出码: " + QString::number(exitCode) + ")");
    }
}

void MpWrapper::onProcessError(QProcess::ProcessError error)
{
    m_isRunning = false;
    
    QString errorMsg;
    switch (error) {
        case QProcess::FailedToStart:
            errorMsg = "错误：解释器无法启动\n请检查 MicroPython 是否正确安装";
            break;
        case QProcess::Crashed:
            errorMsg = "错误：解释器进程崩溃";
            break;
        case QProcess::Timedout:
            errorMsg = "错误：执行超时";
            break;
        case QProcess::WriteError:
            errorMsg = "错误：写入解释器失败";
            break;
        case QProcess::ReadError:
            errorMsg = "错误：读取解释器输出失败";
            break;
        default:
            errorMsg = "错误：解释器发生未知错误";
    }
    
    emit executionFinished(false, errorMsg);
}

void MpWrapper::onReadyReadStandardOutput()
{
    QString output = QString::fromLocal8Bit(m_process->readAllStandardOutput());
    if (!output.isEmpty()) {
        emit outputReceived(output);
    }
}

void MpWrapper::onReadyReadStandardError()
{
    QString error = QString::fromLocal8Bit(m_process->readAllStandardError());
    if (!error.isEmpty()) {
        emit errorReceived(error);
    }
}

InterpreterSettings MpWrapper::getSettings() const
{
    QSettings settings("nanoPython", "Interpreter");
    InterpreterSettings is;
    is.pythonPath = settings.value("pythonPath").toString();
    is.modulePaths = settings.value("modulePaths").toStringList();
    is.environmentVariables = settings.value("environmentVariables").toStringList();
    is.timeoutSeconds = settings.value("timeoutSeconds", 30).toInt();
    is.enableOptimization = settings.value("enableOptimization", false).toBool();
    is.showBytecode = settings.value("showBytecode", false).toBool();
    return is;
}

void MpWrapper::saveSettings(const InterpreterSettings &settings)
{
    QSettings qs("nanoPython", "Interpreter");
    qs.setValue("pythonPath", settings.pythonPath);
    qs.setValue("modulePaths", settings.modulePaths);
    qs.setValue("environmentVariables", settings.environmentVariables);
    qs.setValue("timeoutSeconds", settings.timeoutSeconds);
    qs.setValue("enableOptimization", settings.enableOptimization);
    qs.setValue("showBytecode", settings.showBytecode);
}
