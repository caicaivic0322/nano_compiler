#include "compile_task.h"

CompileTask::CompileTask(QObject *parent)
    : QObject(parent)
    , m_isRunning(false)
{
}

void CompileTask::setOptions(const CompileOptions &options)
{
    m_options = options;
}

CompileTask::CompileOptions CompileTask::options() const
{
    return m_options;
}

void CompileTask::start()
{
    if (m_isRunning) {
        return;
    }

    m_isRunning = true;
    emit started();

    // TODO: 实现编译逻辑
    // 这里将在后续版本中实现完整的编译任务管理
}

void CompileTask::stop()
{
    if (!m_isRunning) {
        return;
    }

    m_isRunning = false;
    // TODO: 实现停止逻辑
}

bool CompileTask::isRunning() const
{
    return m_isRunning;
}
