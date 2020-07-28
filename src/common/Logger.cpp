#include "Logger.h"

#include <cassert>
#include <unistd.h>
#include <fcntl.h>
#include <string>

#include <QDateTime>
#include <QString>

#include "QueueBuffer.h"

Logger *Logger::s_instance = NULL;

static QString LogFormatTime()
{
    return QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
}
Logger::Logger()
{
    assert(s_instance == NULL);
    qRegisterMetaType<enum_type>();
    m_capture_pipe[0] = -1;
    m_capture_pipe[1] = -1;
    m_original_stderr = -1;
    try {
        Init();
    } catch(...) {
        Free();
        throw;
    }
    s_instance = this;
}

Logger::~Logger()
{
    assert(s_instance == this);
    s_instance = NULL;
    Free();
}

void Logger::SetLogFile(const QString &filename)
{
    m_log_file.setFileName(filename);
    m_log_file.open(QFile::WriteOnly | QFile::Append | QFile::Text | QFile::Unbuffered);
}

void Logger::LogInfo(const QString &str)
{
    assert(s_instance != NULL);
    std::lock_guard<std::mutex> lock(s_instance->m_mutex);
    Q_UNUSED(lock);
    QByteArray buf = (str + "\n").toLocal8Bit();
    write(s_instance->m_original_stderr, buf.constData(), buf.size());
    if (s_instance->m_log_file.isOpen()) {
        s_instance->m_log_file.write((LogFormatTime() + " (I) " + str + "\n").toLocal8Bit());
    }
    emit s_instance->NewLine(TYPE_INFO, str);
}

void Logger::LogWarning(const QString& str) {
    assert(s_instance != NULL);
    std::lock_guard<std::mutex> lock(s_instance->m_mutex); Q_UNUSED(lock);
    QByteArray buf = ("\033[1;33m" + str + "\033[0m\n").toLocal8Bit();
    write(s_instance->m_original_stderr, buf.constData(), buf.size());
    if(s_instance->m_log_file.isOpen())
        s_instance->m_log_file.write((LogFormatTime() + " (W) " + str + "\n").toLocal8Bit());
    emit s_instance->NewLine(TYPE_WARNING, str);
}

void Logger::LogError(const QString& str) {
    assert(s_instance != NULL);
    std::lock_guard<std::mutex> lock(s_instance->m_mutex); Q_UNUSED(lock);
    QByteArray buf = ("\033[1;31m" + str + "\033[0m\n").toLocal8Bit();
    write(s_instance->m_original_stderr, buf.constData(), buf.size());
    if(s_instance->m_log_file.isOpen())
        s_instance->m_log_file.write((LogFormatTime() + " (E) " + str + "\n").toLocal8Bit());
    emit s_instance->NewLine(TYPE_ERROR, str);
}

void Logger::Init()
{
    if (pipe2(m_capture_pipe, O_CLOEXEC) != 0) {
        throw std::runtime_error("Failed to create capture pipe");
    }
    m_original_stderr = dup(2);	//copy stderr
    dup2(m_capture_pipe[1], 2);	//redirect stderr
    m_capture_thread = std::thread(&Logger::CaptureThread, this);
}

void Logger::Free()
{
    if (m_original_stderr != -1) {
        dup2(m_original_stderr, 2);		//restore stderr
    }
    if (m_capture_pipe[1] != -1) {
        close(m_capture_pipe[1]);		//close write end of pipe
        m_capture_pipe[1] = -1;
    }
    if (m_capture_thread.joinable()) {
        m_capture_thread.join();
    }
    if (m_capture_pipe[0] != -1) {
        close(m_capture_pipe[0]);
        m_capture_pipe[0] = -1;
    }
    if (m_original_stderr != -1) {
        close(m_original_stderr);
        m_original_stderr = -1;
    }
}

void Logger::CaptureThread()
{
    QueueBuffer<char> buffer;
    size_t pos = 0;
    for( ; ; ) {
        ssize_t num;
        do {
            num = read(m_capture_pipe[0], buffer.Reserve(PIPE_BUF), PIPE_BUF);
        } while (num == -1 && errno == EINTR);
        if (num <= 0)
            break;
        buffer.Push(num);
        while (pos < buffer.GetSize()) {
            if (buffer[pos] == '\n') {
                std::lock_guard<std::mutex> lock(s_instance->m_mutex); Q_UNUSED(lock);
                std::string buf = "\033[2m" + std::string(buffer.GetData(), pos) + "\033[0m\n";
                write(s_instance->m_original_stderr, buf.data(), buf.size());
                QString str = QString::fromLocal8Bit(buffer.GetData(), pos);
                if (s_instance->m_log_file.isOpen())
                    s_instance->m_log_file.write((LogFormatTime() + " (S) " + str + "\n").toLocal8Bit());
                emit s_instance->NewLine(TYPE_STDERR, str);
                buffer.Pop(pos + 1);
                pos = 0;
            } else {
                ++pos;
            }
        }
     }
}



































