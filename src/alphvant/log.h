#pragma once

#include "settings.h"

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <sstream>
#include <string>

namespace alphvant {
    enum class LogLevel {
#ifndef ERROR
#ifndef DEBUG
        DEBUG = 0,
        INFO,
        WARNING,
        ERROR,
        CRITICAL,
#endif
#endif

        Debug = 0,
        Info,
        Warning,
        Error,
        Critical,
    };

    class ILogHandler {
      public:
        virtual ~ILogHandler() = default;

        virtual void log(std::string message, LogLevel level) = 0;
    };

    class CerrLogHandler : public ILogHandler {
      public:
        void log(std::string message, LogLevel level) override {
            std::string prefix;
            switch (level) {
            case LogLevel::Debug:
                prefix = "DEBUG   ";
                break;
            case LogLevel::Info:
                prefix = "INFO    ";
                break;
            case LogLevel::Warning:
                prefix = "WARNING ";
                break;
            case LogLevel::Error:
                prefix = "ERROR   ";
                break;
            case LogLevel::Critical:
                prefix = "CRITICAL";
                break;
            }
            std::cerr << std::string("(") + timestamp() +
                             std::string(") [ALPHVANT] [") + prefix +
                             std::string("] ") + message
                      << std::endl;
        }

      private:
        static std::string timestamp() {
            char date[32];
            time_t t = time(0);

            tm my_tm;

#if defined(_MSC_VER) || defined(__MINGW32__)
#ifdef ALPHVANT_USE_LOCALTIMEZONE
            localtime_s(&my_tm, &t);
#else
            gmtime_s(&my_tm, &t);
#endif
#else
#ifdef ALPHVANT_USE_LOCALTIMEZONE
            localtime_r(&t, &my_tm);
#else
            gmtime_r(&t, &my_tm);
#endif
#endif

            size_t sz =
                strftime(date, sizeof(date), "%Y-%m-%d %H:%M:%S", &my_tm);
            return std::string(date, date + sz);
        }
    };

    class logger {
      public:
        logger(LogLevel level) : level_(level) {
        }
        ~logger() {
#ifdef ALPHVANT_ENABLE_LOGGING
            if (level_ >= get_current_log_level()) {
                get_handler_ref()->log(stringstream_.str(), level_);
            }
#endif
        }

        //
        template <typename T> logger &operator<<(T const &value) {
#ifdef ALPHVANT_ENABLE_LOGGING
            if (level_ >= get_current_log_level()) {
                stringstream_ << value;
            }
#endif
            return *this;
        }

        //
        static void setLogLevel(LogLevel level) {
            get_log_level_ref() = level;
        }

        static void setHandler(ILogHandler *handler) {
            get_handler_ref() = handler;
        }

        static LogLevel get_current_log_level() {
            return get_log_level_ref();
        }

      private:
        //
        static LogLevel &get_log_level_ref() {
            static LogLevel current_level =
                static_cast<LogLevel>(ALPHVANT_LOG_LEVEL);
            return current_level;
        }
        static ILogHandler *&get_handler_ref() {
            static CerrLogHandler default_handler;
            static ILogHandler *current_handler = &default_handler;
            return current_handler;
        }

        //
        std::ostringstream stringstream_;
        LogLevel level_;
    };
} // namespace alphvant

#define ALPHVANT_LOG_CRITICAL                                                  \
    if (alphvant::logger::get_current_log_level() <=                           \
        alphvant::LogLevel::Critical)                                          \
    alphvant::logger(alphvant::LogLevel::Critical)
#define ALPHVANT_LOG_ERROR                                                     \
    if (alphvant::logger::get_current_log_level() <=                           \
        alphvant::LogLevel::Error)                                             \
    alphvant::logger(alphvant::LogLevel::Error)
#define ALPHVANT_LOG_WARNING                                                   \
    if (alphvant::logger::get_current_log_level() <=                           \
        alphvant::LogLevel::Warning)                                           \
    alphvant::logger(alphvant::LogLevel::Warning)
#define ALPHVANT_LOG_INFO                                                      \
    if (alphvant::logger::get_current_log_level() <= alphvant::LogLevel::Info) \
    alphvant::logger(alphvant::LogLevel::Info)
#define ALPHVANT_LOG_DEBUG                                                     \
    if (alphvant::logger::get_current_log_level() <=                           \
        alphvant::LogLevel::Debug)                                             \
    alphvant::logger(alphvant::LogLevel::Debug)
