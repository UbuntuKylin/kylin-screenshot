#ifndef LOGGER_H
#define LOGGER_H

#include <mutex>
#include <thread>
#include <cassert>

#include <QWidget>
#include <QFile>


class Logger: public QObject
{
    Q_OBJECT
public:
    enum enum_type {
        TYPE_INFO,
        TYPE_WARNING,
        TYPE_ERROR,
        TYPE_STDERR
    };
private:
    std::mutex m_mutex;
    QFile m_log_file;

    std::thread m_capture_thread;
    int m_capture_pipe[2], m_shutdown_pipe[2], m_original_stderr;

    static Logger *s_instance;

public:
    Logger();
    ~Logger();

    void SetLogFile(const QString& filename);

    static void LogInfo(const QString& str);
    static void LogWarning(const QString& str);
    static void LogError(const QString& str);

    inline static Logger* GetInstance() {assert(s_instance != NULL); return s_instance; }

signals:
    void NewLine(Logger::enum_type type, QString str);

private:
    void Init();
    void Free();
    void CaptureThread();
};

Q_DECLARE_METATYPE(Logger::enum_type)

#endif // LOGGER_H
