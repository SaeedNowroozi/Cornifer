#include "logger.h"
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

std::shared_ptr<spdlog::logger> Logger::logger_;
QStringListModel* Logger::model_ = nullptr;

void Logger::init(const std::string& logFile)
{
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    if (!logFile.empty()) {
        auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logFile, true);
        logger_ = std::make_shared<spdlog::logger>("multi_sink", spdlog::sinks_init_list{ console_sink, file_sink });
    } else {
        logger_ = std::make_shared<spdlog::logger>("console_logger", console_sink);
    }
    spdlog::set_default_logger(logger_);
    spdlog::set_level(spdlog::level::debug);
    spdlog::flush_on(spdlog::level::info);
}

void Logger::setModel(QStringListModel* model)
{
    model_ = model;
}

void Logger::appendToModel(const QString& msg)
{
    if (!model_) return;
    auto list = model_->stringList();
    list.append(msg);
    model_->setStringList(list);
}

void Logger::info(const std::string& msg)
{
    logger_->info(msg);
    appendToModel(QString::fromStdString(msg));
}

void Logger::warn(const std::string& msg)
{
    logger_->warn(msg);
    appendToModel("[WARN] " + QString::fromStdString(msg));
}

void Logger::error(const std::string& msg)
{
    logger_->error(msg);
    appendToModel("[ERROR] " + QString::fromStdString(msg));
}

void Logger::debug(const std::string& msg)
{
    logger_->debug(msg);
    appendToModel("[DEBUG] " + QString::fromStdString(msg));
}

void Logger::log(const std::string& msg, Level level)
{
    switch (level) {
    case Level::Info:
        info(msg);
        break;
    case Level::Warn:
        warn(msg);
        break;
    case Level::Error:
        error(msg);
        break;
    case Level::Debug:
        debug(msg);
        break;
    }
}
