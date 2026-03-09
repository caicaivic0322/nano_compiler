#ifndef RUN_TASK_H
#define RUN_TASK_H

#include <QObject>
#include <QString>

class RunTask : public QObject
{
    Q_OBJECT

public:
    struct RunOptions
    {
        QString sourceFile;
        QString workingDirectory;
    };

    explicit RunTask(QObject *parent = nullptr);

    void setOptions(const RunOptions &options);
    RunOptions options() const;

    void start();
    void stop();
    bool isRunning() const;

signals:
    void started();
    void finished(bool success, const QString &output);
    void error(const QString &errorMessage);

private:
    RunOptions m_options;
    bool m_isRunning;
};

#endif // RUN_TASK_H
