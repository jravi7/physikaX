#include "core/logger.h"

#include <stdarg.h>
#include <stdio.h>

namespace {

using namespace physika::core::logger;

LogLevel sLevel;
char     sApplicationName[1024] = { '\0' };

const char* StringifyLogLevel(LogLevel level)
{
    switch (level) {
    case LogLevel::kTrace:
        return "TRACE";
    case LogLevel::kDebug:
        return "DEBUG";
    case LogLevel::kInfo:
        return "INFO";
    case LogLevel::kWarn:
        return "WARN";
    case LogLevel::kError:
        return "ERROR";
    case LogLevel::kFatal:
        return "FATAL";
    default:
        return "";
    }
}
}  // namespace

namespace physika::core::logger {

void SetLoggingLevel(LogLevel level)
{
    sLevel = level;
}

void SetApplicationName(char const* applicationName)
{
    if (!applicationName) {
        return;
    }
    snprintf(sApplicationName, sizeof(sApplicationName) - 1, applicationName);
}

void LogMessage(LogLevel level, char const* format...)
{
    if (level < sLevel) {
        return;
    }
    int const kBufferSize         = 2048;
    char      buffer[kBufferSize] = {};
    va_list   args;
    va_start(args, format);
    vsnprintf(buffer, kBufferSize - 1, format, args);
    va_end(args);

    printf("[%s] [%s]: %s\n", sApplicationName, StringifyLogLevel(level), buffer);
}

}  // namespace physika::core::logger