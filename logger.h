#ifndef LOGGER_H
#define LOGGER_H

#include <QString>
#include <QStringListModel>
#include <memory>
#include <spdlog/spdlog.h>

class Logger
{
public:
    enum class Level {
        Info,
        Warn,
        Error,
        Debug
    };

    static void init(const std::string& logFile = "");
    static void setModel(QStringListModel* model);

    static void info(const std::string& msg);
    static void warn(const std::string& msg);
    static void error(const std::string& msg);
    static void debug(const std::string& msg);

    static void log(const std::string& msg, Level level); // متد اصلی لاگ

private:
    static void appendToModel(const QString& msg);
    static std::shared_ptr<spdlog::logger> logger_;
    static QStringListModel* model_;
};

#endif // LOGGER_H
