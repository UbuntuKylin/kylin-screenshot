#ifndef COMMANDLINEOPTIONS_H
#define COMMANDLINEOPTIONS_H

#include "src/core/Global.h"

class CommandLineException : public std::exception {
public:
    inline virtual const char* what() const throw() override {
        return "CommandLineException";
    }
};

class CommandLineOptions
{
private:
    QString m_settings_file;
    QString m_log_file;
    QString m_stats_file;
    bool m_systray;
    bool m_start_hidden;
    bool m_start_recording;
    bool m_activate_schedule;
    bool m_sync_diagram;
//    bool m_benchmark;
//    bool m_gui;

    static CommandLineOptions *s_instance;

public:
    CommandLineOptions();
    ~CommandLineOptions();

    void Parse();

    inline static CommandLineOptions* GetInstance() { assert(s_instance != NULL); return s_instance; }
    inline static const QString& GetSettingsFile() { return GetInstance()->m_settings_file; }
    inline static const QString& GetLogFile() { return GetInstance()->m_log_file; }
    inline static const QString& GetStatsFile() { return GetInstance()->m_stats_file; }
    inline static bool GetSysTray() { return GetInstance()->m_systray; }
    inline static bool GetStartHidden() { return GetInstance()->m_start_hidden; }
    inline static bool GetStartRecording() { return GetInstance()->m_start_recording; }
    inline static bool GetActivateSchedule() { return GetInstance()->m_activate_schedule; }
    inline static bool GetSyncDiagram() { return GetInstance()->m_sync_diagram; }
//    inline static bool GetBenchmark() { return GetInstance()->m_benchmark; }
//    inline static bool GetGui() { return GetInstance()->m_gui; }
};


QString GetVersionInfo();
QString GetApplicationSystemDir(const QString& subdir = QString());
QString GetApplicationUserDir(const QString& subdir = QString());

#endif // COMMANDLINEOPTIONS_H
