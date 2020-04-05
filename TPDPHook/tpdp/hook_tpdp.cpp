#include <memory>
#include <sstream>
#include <unordered_map>
#include "hook_tpdp.h"
#include "../ini_parser.h"
#include "../hook.h"
#include "../log.h"
#include "../version.h"
#include "custom_skills.h"

typedef void(*VoidCall)(void);

VoidCall g_anim_init_func = nullptr;
VoidCall g_effect_init_func = nullptr;

const char *g_wnd_title = "%s ver%.3f (TPDPHook v" VERSION_STRING ") [%04.1ffps]";

void register_custom_skill(unsigned int effect_id, SkillCall skill_func)
{
	auto table = RVA(0x942fb0).ptr<uint32_t*>();
	table[effect_id] = (uint32_t)skill_func;
}

void register_custom_skill_activator(unsigned int effect_id, SkillCall activator_func)
{
	auto table = RVA(0x93ffa0).ptr<uint32_t*>();
	table[effect_id] = (uint32_t)activator_func;
}

void hook_init_anims(void)
{
	g_anim_init_func();

	std::unordered_map<std::size_t, std::size_t> map;
	for(auto& it : IniFile::global["skill_anims"])
	{
		std::size_t k, v;

		try
		{
			k = std::stoul(it.first);
			v = std::stoul(it.second);
		}
		catch(...)
		{
			continue;
		}

		map[k] = v;
	}

	// skill animation function pointer tables
	uint32_t *tables[] = {
		RVA(0x946500).ptr<uint32_t*>(),
		RVA(0x947528).ptr<uint32_t*>(),
		RVA(0x94b938).ptr<uint32_t*>(),
		RVA(0x94c9f8).ptr<uint32_t*>(),
		RVA(0x94dad0).ptr<uint32_t*>()
	};

	auto buf = std::make_unique<uint32_t[]>(1024);
	for(auto table : tables)
	{
		auto sz = 1024 * sizeof(uint32_t);
		memcpy(buf.get(), table, sz);

		for(auto it = map.begin(); it != map.end();)
		{
			auto k = it->first;
			auto v = it->second;
			if(k >= 1024)
			{
				log_debug(L"Skill value out of range: " + std::to_wstring(k));
				it = map.erase(it);
				continue;
			}
			if(v >= 1024)
			{
				log_debug(L"Skill value out of range: " + std::to_wstring(v));
				it = map.erase(it);
				continue;
			}

			buf[k] = table[v];
			++it;
		}

		memcpy(table, buf.get(), sz);
	}

	log_debug(L"Skill hook successful.\nPatched " + std::to_wstring(map.size()) + L" skills.");
}

void hook_init_effects(void)
{
	g_effect_init_func();

	std::unordered_map<std::size_t, std::size_t> map;
	for(auto& it : IniFile::global["effects"])
	{
		std::size_t k, v;

		try
		{
			k = std::stoul(it.first);
			v = std::stoul(it.second);
		}
		catch(...)
		{
			continue;
		}

		map[k] = v;
	}

	uint32_t *tables[] = {
		RVA(0x93df98).ptr<uint32_t*>(),
		RVA(0x93efa0).ptr<uint32_t*>(),
		RVA(0x93ffa0).ptr<uint32_t*>(),
		RVA(0x940fa0).ptr<uint32_t*>(),
		RVA(0x941fb0).ptr<uint32_t*>(),
		RVA(0x942fb0).ptr<uint32_t*>()
	};

	auto buf = std::make_unique<uint32_t[]>(1024);
	for(auto table : tables)
	{
		auto sz = 1024 * sizeof(uint32_t);
		memcpy(buf.get(), table, sz);

		for(auto it = map.begin(); it != map.end();)
		{
			auto k = it->first;
			auto v = it->second;
			if(k >= 1024)
			{
				log_debug(L"Effect value out of range: " + std::to_wstring(k));
				it = map.erase(it);
				continue;
			}
			if(v >= 1024)
			{
				log_debug(L"Effect value out of range: " + std::to_wstring(v));
				it = map.erase(it);
				continue;
			}

			buf[k] = table[v];
			++it;
		}

		memcpy(table, buf.get(), sz);
	}

	init_custom_skills();

	log_debug(L"Effect hook successful.\nPatched " + std::to_wstring(map.size()) + L" effects.");
}

void tpdp_install_hooks()
{
	g_anim_init_func = RVA(0x1402f0).ptr<VoidCall>();
	g_effect_init_func = RVA(0x469c0).ptr<VoidCall>();
	patch_call(RVA(0x17e3ef), &hook_init_anims);
	patch_call(RVA(0x17e3f4), &hook_init_effects);

	if(IniFile::global["general"]["set_window_title"] == "true")
	{
		auto mem = RVA(0x1af5f1).ptr<uint8_t*>();
		unsigned int opcode = *mem;
		if(opcode == 0x68u)
		{
			auto ptr = g_wnd_title;
			patch_memory(RVA(0x1af5f2), &ptr, 4);
		}
		else
		{
			std::wstringstream msg;
			msg << std::hex << std::showbase << L"Failed to patch memory, unexpected opcode: " << opcode << L"\n"
				<< L"address: " << (uintptr_t)mem << L"\n";

			log_fatal(msg.str());
		}
	}
}
