#pragma once
#include <string>

enum class LogLevel
{
    error,
    warn,
    info,
    debug,
};

void log_set_level(LogLevel level);
LogLevel log_get_level();

[[noreturn]] void log_fatal(const std::wstring& msg);
void log_error(const std::wstring& msg);
void log_warn(const std::wstring& msg, bool show_msgbox = false);
void log_info(const std::wstring& msg, bool show_msgbox = false);
void log_debug(const std::wstring& msg);
