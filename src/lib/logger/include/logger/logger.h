#pragma once

#include <utility>

namespace physika {
namespace logger {

enum class LogLevel {
    kTrace = 1 << 0,
    kDebug = 1 << 1,
    kInfo  = 1 << 2,
    kWarn  = 1 << 3,
    kError = 1 << 4,
    kFatal = 1 << 5,
};

/**
 * @brief Set the Application Name to show in log out
 *
 * @param applicationName A null terminated character string
 */
void SetApplicationName(char const* applicationName);

/**
 * @brief Set the Logging Level object
 *
 * @param level A user defined log level settings
 *              to control log output
 */
void SetLoggingLevel(LogLevel level);

/**
 * @brief Log a message to the standard output
 *        with a specified log level and
 *        std printf formatting options
 *
 * @note Use the handy LOG_* functions which are
 *       clean and easy to use instead for most cases.
 *
 * @param level Specify a log level for this message
 * @param format Specify message formatting similar to printf
 * @param ... Variable number of output arguments
 */
void LogMessage(LogLevel level, const char* format, ...);

template <typename... Targs>
inline void LOG_FATAL(char const* message, Targs&&... args)
{
    LogMessage(LogLevel::kFatal, message, std::forward<Targs&&>(args)...);
}

template <typename... Targs>
inline void LOG_ERROR(char const* message, Targs&&... args)
{
    LogMessage(LogLevel::kError, message, std::forward<Targs&&>(args)...);
}

template <typename... Targs>
inline void LOG_WARN(char const* message, Targs&&... args)
{
    LogMessage(LogLevel::kWarn, message, std::forward<Targs&&>(args)...);
}

template <typename... Targs>
inline void LOG_INFO(const char* message, Targs&&... args)
{
    LogMessage(LogLevel::kInfo, message, std::forward<Targs&&>(args)...);
}

template <typename... Targs>
inline void LOG_DEBUG(char const* message, Targs&&... args)
{
    LogMessage(LogLevel::kDebug, message, std::forward<Targs&&>(args)...);
}
template <typename... Targs>
inline void LOG_TRACE(char const* message, Targs&&... args)
{
    LogMessage(LogLevel::kTrace, message, std::forward<Targs&&>(args)...);
}

}  // namespace logger
}  // namespace physika
