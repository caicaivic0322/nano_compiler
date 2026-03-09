#ifndef COMPILE_TASK_H
#define COMPILE_TASK_H

#include <QObject>
#include <QString>

class CompileTask : public QObject
{
    Q_OBJECT

public:
    struct CompileOptions
    {
        QString sourceFile;
        QString outputFile;
        QStringList includePaths;
        QStringList libraryPaths;
        QStringList libraries;
        QStringList defines;
        bool optimize = false;
        bool debug = true;
    };

    explicit CompileTask(QObject *parent = nullptr);

    void setOptions(const CompileOptions &options);
    CompileOptions options() const;

    void start();
    void stop();
    bool isRunning() const;

signals:
    void started();
    void finished(bool success, const QString &output);
    void error(const QString &errorMessage);
    void output(const QString &message);

private:
    CompileOptions m_options;
    bool m_isRunning;
};

#endif // COMPILE_TASK_H
