#include <Windows.h>
#include <fstream>
#include "log.h"

LogLevel g_level = LogLevel::debug;
std::wofstream g_logfile;

void log_set_level(LogLevel level)
{
	g_level = level;
}

LogLevel log_get_level()
{
	return g_level;
}

static void log_file(const std::wstring& msg, LogLevel level)
{
	std::wstring prefix;
	switch(level)
	{
	case LogLevel::error:
		prefix = L"ERROR: ";
		break;
	case LogLevel::warn:
		prefix = L"WARNING: ";
		break;
	case LogLevel::info:
		prefix = L"INFO: ";
		break;
	case LogLevel::debug:
		prefix = L"DEBUG: ";
		break;
	default:
		prefix = L"ERROR: ";
		break;
	}

	if(!g_logfile.is_open())
		g_logfile.open("TPDPHook.log", std::ios::trunc);
	g_logfile << prefix << msg << L"\n\n";
	g_logfile.flush();
}

void log_fatal(const std::wstring& msg)
{
	log_file(msg, LogLevel::error);
	MessageBoxW(GetActiveWindow(), msg.c_str(), L"TPDPHook Fatal Error", MB_ICONERROR | MB_OK);
	abort();
}

void log_error(const std::wstring& msg)
{
	log_file(msg, LogLevel::error);
	MessageBoxW(GetActiveWindow(), msg.c_str(), L"TPDPHook Error", MB_ICONERROR | MB_OK);
}

void log_warn(const std::wstring& msg, bool show_msgbox)
{
	if(g_level >= LogLevel::warn)
	{
		log_file(msg, LogLevel::warn);
		if(show_msgbox)
			MessageBoxW(GetActiveWindow(), msg.c_str(), L"TPDPHook", MB_ICONEXCLAMATION | MB_OK);
	}
}

void log_info(const std::wstring& msg, bool show_msgbox)
{
	if(g_level >= LogLevel::info)
	{
		log_file(msg, LogLevel::info);
		if(show_msgbox)
			MessageBoxW(GetActiveWindow(), msg.c_str(), L"TPDPHook", MB_ICONINFORMATION | MB_OK);
	}
}

void log_debug(const std::wstring& msg)
{
	if(g_level >= LogLevel::debug)
	{
		log_file(msg, LogLevel::debug);
	}
}
