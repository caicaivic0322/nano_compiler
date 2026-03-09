#include "run_task.h"

RunTask::RunTask(QObject *parent)
    : QObject(parent)
    , m_isRunning(false)
{
}

void RunTask::setOptions(const RunOptions &options)
{
    m_options = options;
}

RunTask::RunOptions RunTask::options() const
{
    return m_options;
}

void RunTask::start()
{
    if (m_isRunning) {
        return;
    }

    m_isRunning = true;
    emit started();
}

void RunTask::stop()
{
    if (!m_isRunning) {
        return;
    }

    m_isRunning = false;
}

bool RunTask::isRunning() const
{
    return m_isRunning;
}
