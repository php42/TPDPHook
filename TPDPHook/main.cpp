#include <Windows.h>
#include <filesystem>
#include <sstream>
#include "ini_parser.h"
#include "hook.h"
#include "log.h"
#include "tpdp/hook_tpdp.h"

HMODULE g_hmodule = nullptr;

static void read_config()
{
	IniFile::global.read("TPDPHook.ini");
	if(IniFile::global["general"]["logging"] == "debug")
		log_set_level(LogLevel::debug);
	else if(IniFile::global["general"]["logging"] == "info")
		log_set_level(LogLevel::info);
	else if(IniFile::global["general"]["logging"] == "warning")
		log_set_level(LogLevel::warn);
	else if(IniFile::global["general"]["logging"] == "error")
		log_set_level(LogLevel::error);
	else if(IniFile::global["general"]["debug"] == "true") // backwards compatibility
		log_set_level(LogLevel::debug);
	else
		log_set_level(LogLevel::error);
}

BOOL WINAPI DllMain(HMODULE hModule, DWORD fdwReason, [[maybe_unused]] LPVOID lpvReserved)
{
	switch(fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		g_hmodule = hModule;
		RVA::base((uintptr_t)GetModuleHandleW(nullptr));

		// global config
		read_config();

		if(log_get_level() >= LogLevel::debug)
		{
			std::wstringstream msg;
			msg << std::hex << std::showbase << L"Image Base: " << RVA::base();
			log_debug(msg.str());
		}

		tpdp_install_hooks();
		FlushInstructionCache(GetModuleHandleW(nullptr), nullptr, 0); // just in case
		break;

	case DLL_PROCESS_DETACH:
		// ...
		break;

	default:
		break;
	}

	return TRUE;
}
