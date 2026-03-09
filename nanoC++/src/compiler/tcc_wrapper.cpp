#include "tcc_wrapper.h"

#include <QProcess>
#include <QTemporaryDir>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QDir>
#include <QCoreApplication>
#include <QSettings>

TccWrapper::TccWrapper(QObject *parent)
    : QObject(parent)
    , m_process(std::make_unique<QProcess>(this))
    , m_isCompiling(false)
{
    setupTcc();
    
    // 连接进程信号
    connect(m_process.get(), QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &TccWrapper::onProcessFinished);
    connect(m_process.get(), &QProcess::errorOccurred,
            this, &TccWrapper::onProcessError);
    connect(m_process.get(), &QProcess::readyReadStandardOutput,
            this, &TccWrapper::onReadyReadStandardOutput);
    connect(m_process.get(), &QProcess::readyReadStandardError,
            this, &TccWrapper::onReadyReadStandardError);
}

TccWrapper::~TccWrapper()
{
    stop();
}

bool TccWrapper::setupTcc()
{
    // 使用相对于应用程序的固定路径
    QString appDir = QCoreApplication::applicationDirPath();
    QString tccPath;
    
#ifdef Q_OS_WIN
    // Windows: 使用 bundled 的 TCC
    tccPath = appDir + "/tcc/tcc.exe";
    
    // 如果找不到，尝试上级目录（开发环境）
    if (!QFile::exists(tccPath)) {
        tccPath = appDir + "/../third_party/tcc/tcc.exe";
    }
#else
    // Linux/macOS
    tccPath = appDir + "/tcc/tcc";
    if (!QFile::exists(tccPath)) {
        tccPath = appDir + "/../third_party/tcc/tcc";
    }
    // 如果 bundled 不存在，使用系统安装的
    if (!QFile::exists(tccPath)) {
        tccPath = "/usr/bin/tcc";
    }
#endif
    
    // 保存到设置
    QSettings settings("nanoC++", "Compiler");
    settings.setValue("tccPath", tccPath);
    
    return QFile::exists(tccPath);
}

QString TccWrapper::getTccPath() const
{
    QSettings settings("nanoC++", "Compiler");
    QString path = settings.value("tccPath").toString();
    
    // 如果设置的路径不存在，重新检测
    if (path.isEmpty() || !QFile::exists(path)) {
        const_cast<TccWrapper*>(this)->setupTcc();
        path = settings.value("tccPath").toString();
    }
    
    return path;
}

void TccWrapper::setTccPath(const QString &path)
{
    QSettings settings("nanoC++", "Compiler");
    settings.setValue("tccPath", path);
}

void TccWrapper::compile(const QString &filePath, const QString &sourceCode)
{
    if (m_isCompiling) {
        emit outputReceived("错误：编译器正在运行，请先停止当前编译");
        return;
    }
    
    QString tccPath = getTccPath();
    if (!QFile::exists(tccPath)) {
        emit compilationFinished(false, "错误：找不到 TCC 编译器\n"
                                       "请重新安装 nanoC++");
        return;
    }
    
    m_isCompiling = true;
    m_currentSourceFile = filePath;
    
    // 创建临时目录
    m_tempDir = std::make_unique<QTemporaryDir>();
    if (!m_tempDir->isValid()) {
        emit compilationFinished(false, "错误：无法创建临时目录");
        m_isCompiling = false;
        return;
    }
    
    // 写入源代码到临时文件
    QString tempSourcePath;
    if (filePath.isEmpty()) {
        tempSourcePath = m_tempDir->filePath("temp_source.cpp");
    } else {
        QFileInfo fileInfo(filePath);
        tempSourcePath = m_tempDir->filePath(fileInfo.fileName());
    }
    
    QFile sourceFile(tempSourcePath);
    if (!sourceFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        emit compilationFinished(false, "错误：无法创建临时源文件");
        m_isCompiling = false;
        return;
    }
    
    QTextStream stream(&sourceFile);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    stream.setCodec("UTF-8");
#endif
    stream << sourceCode;
    sourceFile.close();
    
    // 设置输出路径
    QString outputPath = getOutputPath(tempSourcePath);
    m_lastOutputPath = outputPath;
    
    // 构建编译命令
    QStringList arguments;
    arguments << "-Wall"                    // 显示所有警告
              << "-Wextra"                 // 显示额外警告
              << "-g"                      // 包含调试信息
              << "-o" << outputPath        // 输出文件
              << tempSourcePath;           // 源文件
    
    emit compilationStarted();
    emit outputReceived("开始编译...");
    emit outputReceived("命令: " + tccPath + " " + arguments.join(" "));
    emit outputReceived("");
    
    // 启动编译进程
    m_process->setWorkingDirectory(m_tempDir->path());
    m_process->start(tccPath, arguments);
    
    if (!m_process->waitForStarted(5000)) {
        emit compilationFinished(false, "错误：无法启动编译器进程");
        m_isCompiling = false;
    }
}

void TccWrapper::runCompiledProgram()
{
    if (!m_lastOutputPath.isEmpty() && QFile::exists(m_lastOutputPath)) {
        emit outputReceived("\n========== 程序输出 ==========\n");
        
        QProcess *runProcess = new QProcess(this);
        connect(runProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                [this, runProcess](int exitCode, QProcess::ExitStatus) {
                    emit outputReceived("\n========== 程序结束 (退出码: " + 
                                       QString::number(exitCode) + ") ==========\n");
                    runProcess->deleteLater();
                });
        connect(runProcess, &QProcess::readyReadStandardOutput,
                [this, runProcess]() {
                    emit outputReceived(QString::fromLocal8Bit(runProcess->readAllStandardOutput()));
                });
        connect(runProcess, &QProcess::readyReadStandardError,
                [this, runProcess]() {
                    emit outputReceived(QString::fromLocal8Bit(runProcess->readAllStandardError()));
                });
        
        runProcess->start(m_lastOutputPath);
    }
}

void TccWrapper::stop()
{
    if (m_process->state() != QProcess::NotRunning) {
        m_process->terminate();
        if (!m_process->waitForFinished(3000)) {
            m_process->kill();
        }
    }
    m_isCompiling = false;
}

bool TccWrapper::isCompiling() const
{
    return m_isCompiling;
}

void TccWrapper::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    m_isCompiling = false;
    
    if (exitStatus == QProcess::CrashExit) {
        emit compilationFinished(false, "编译器进程崩溃");
        return;
    }
    
    if (exitCode == 0) {
        emit outputReceived("\n编译成功！");
        
        // 运行编译后的程序
        runCompiledProgram();
        
        emit compilationFinished(true, "编译成功");
    } else {
        emit compilationFinished(false, "编译失败 (退出码: " + QString::number(exitCode) + ")");
    }
}

void TccWrapper::onProcessError(QProcess::ProcessError error)
{
    m_isCompiling = false;
    
    QString errorMsg;
    switch (error) {
        case QProcess::FailedToStart:
            errorMsg = "错误：编译器无法启动\n请检查 TCC 是否正确安装";
            break;
        case QProcess::Crashed:
            errorMsg = "错误：编译器进程崩溃";
            break;
        case QProcess::Timedout:
            errorMsg = "错误：编译超时";
            break;
        case QProcess::WriteError:
            errorMsg = "错误：写入编译器失败";
            break;
        case QProcess::ReadError:
            errorMsg = "错误：读取编译器输出失败";
            break;
        default:
            errorMsg = "错误：编译器发生未知错误";
    }
    
    emit compilationFinished(false, errorMsg);
}

void TccWrapper::onReadyReadStandardOutput()
{
    QString output = QString::fromLocal8Bit(m_process->readAllStandardOutput());
    if (!output.isEmpty()) {
        emit outputReceived(output);
    }
}

void TccWrapper::onReadyReadStandardError()
{
    QString error = QString::fromLocal8Bit(m_process->readAllStandardError());
    if (!error.isEmpty()) {
        emit outputReceived(error);
    }
}

QString TccWrapper::getOutputPath(const QString &sourcePath) const
{
    QFileInfo fileInfo(sourcePath);
    QString baseName = fileInfo.baseName();
    
#ifdef Q_OS_WIN
    return m_tempDir->filePath(baseName + ".exe");
#else
    return m_tempDir->filePath(baseName);
#endif
}

CompilerSettings TccWrapper::getSettings() const
{
    QSettings settings("nanoC++", "Compiler");
    CompilerSettings cs;
    cs.tccPath = settings.value("tccPath").toString();
    cs.includePaths = settings.value("includePaths").toStringList();
    cs.libPaths = settings.value("libPaths").toStringList();
    cs.libraries = settings.value("libraries").toStringList();
    cs.optimizationLevel = settings.value("optimizationLevel", 0).toInt();
    cs.enableWarnings = settings.value("enableWarnings", true).toBool();
    cs.treatWarningsAsErrors = settings.value("treatWarningsAsErrors", false).toBool();
    return cs;
}

void TccWrapper::saveSettings(const CompilerSettings &settings)
{
    QSettings qs("nanoC++", "Compiler");
    qs.setValue("tccPath", settings.tccPath);
    qs.setValue("includePaths", settings.includePaths);
    qs.setValue("libPaths", settings.libPaths);
    qs.setValue("libraries", settings.libraries);
    qs.setValue("optimizationLevel", settings.optimizationLevel);
    qs.setValue("enableWarnings", settings.enableWarnings);
    qs.setValue("treatWarningsAsErrors", settings.treatWarningsAsErrors);
}
