#pragma once
#include <string>
#include "../hook.h"
#include "mem_structs.h"

// Definitions for various data types and accessors for
// known TPDP functions and memory structures.

typedef int(__cdecl*SkillCall)(int);

enum PuppetStyleType
{
	STYLE_NONE = 0,
	STYLE_NORMAL,
	STYLE_POWER,
	STYLE_DEFENSE,
	STYLE_ASSIST,
	STYLE_SPEED,
	STYLE_EXTRA,
	STYLE_MAX
};

enum ElementType
{
	ELEMENT_NONE = 0,
	ELEMENT_VOID,
	ELEMENT_FIRE,
	ELEMENT_WATER,
	ELEMENT_NATURE,
	ELEMENT_EARTH,
	ELEMENT_STEEL,
	ELEMENT_WIND,
	ELEMENT_ELECTRIC,
	ELEMENT_LIGHT,
	ELEMENT_DARK,
	ELEMENT_NETHER,
	ELEMENT_POISON,
	ELEMENT_FIGHTING,
	ELEMENT_ILLUSION,
	ELEMENT_SOUND,
	ELEMENT_DREAM,
	ELEMENT_WARPED,
	ELEMENT_MAX
};

enum SkillType
{
	SKILL_TYPE_FOCUS = 0,
	SKILL_TYPE_SPREAD,
	SKILL_TYPE_STATUS,
	SKILL_TYPE_MAX
};

enum PuppetMark
{
	MARK_NONE = 0,
	MARK_RED,
	MARK_BLUE,
	MARK_BLACK,
	MARK_WHITE,
	MARK_GREEN,
	MARK_MAX
};

enum CostumeType
{
	COSTUME_NORMAL = 0,
	COSTUME_ALT_COLOR,
	COSTUME_ALT_OUTFIT,
	COSTUME_WEDDING_DRESS,
	COSTUME_MAX
};

enum Terrain
{
	TERRAIN_NONE = 0,
	TERRAIN_SEIRYU,
	TERRAIN_SUZAKU,
	TERRAIN_BYAKKO,
	TERRAIN_GENBU,
	TERRAIN_KOHRYU
};

enum Weather
{
	WEATHER_NONE = 0,
	WEATHER_CALM,
	WEATHER_AURORA,
	WEATHER_HEAVYFOG,
	WEATHER_DUSTSTORM,
	WEATHER_SUNSHOWER
};

enum Status
{
	STATUS_NONE = 0,
	STATUS_POIS,
	STATUS_BURN,
	STATUS_BLIND,
	STATUS_FEAR,
	STATUS_WEAK,
	STATUS_PARA,
	STATUS_HVYPOIS,
	STATUS_HVYBURN,
	STATUS_STOP,
	STATUS_FAINT,
	STATUS_SHOCK,
	STATUS_HVYWEAK,
	STATUS_CONFUSE
};

// Returns pointer to array of 2 BattleData structures representing the each players side of the field.
// battle_data[0] is the local player, battle_data[1] is the opponent.
//
// IMPORTANT: the players party may be accessed from here (battle_data.puppets[6]), but the puppets must be decrypted first.
// The level, hp, sp, status_effects, and stopped_turns fields of party puppets are unencrypted and may be accessed directly.
static inline BattleData* get_battle_data()
{
	return RVA(0xc59b60).ptr<BattleData*>();
}

// Returns state member of BattleData for the given player.
static inline BattleState* get_battle_state(int player)
{
	auto data = get_battle_data();
	return &data[player].state;
}

// Show message that weather has changed.
// Call each frame until zero is returned.
static inline int do_weather_msg()
{
	auto func = RVA(0x202f0).ptr<int(*)(void)>();
	return func();
}

// Call once before beginning a new weather message.
static inline void reset_weather_msg()
{
	*RVA(0x93C06C).ptr<uint32_t*>() = 0;
}

// Show message that terrain has changed.
// Call each frame until zero is returned.
static inline int do_terrain_msg()
{
	auto func = RVA(0x20590).ptr<int(*)(void)>();
	return func();
}

// Call once before beginning a new terrain message.
static inline void reset_terrain_msg()
{
	*RVA(0x93c704).ptr<uint32_t*>() = 0;
}

// Show arbitrary message during battle.
// Call each frame for at least a second.
static inline int set_battle_text(const char* msg, bool unknown_arg = false, bool unknown_arg2 = false)
{
	auto func = RVA(0x1b0ee0).ptr<int(__cdecl*)(const char*, bool, bool)>();
	return func(msg, unknown_arg, unknown_arg2);
}
static inline int set_battle_text(std::string msg, bool unknown_arg = false, bool unknown_arg2 = false)
{
	return set_battle_text(msg.c_str(), unknown_arg, unknown_arg2);
}

// Call once before starting a new message.
static inline void clear_battle_text()
{
	auto func = RVA(0x287c0).ptr<void*(*)(void)>();
	func();
}

// Returns true if game is set to 60 fps, false if 30.
static inline bool is_game_60fps()
{
	return *RVA(0xc5960b).ptr<bool*>();
}

// Returns pointer to struct containing weather/terrain state.
// Has side effects aside from just returning the pointer, call each time you
// want to modify weather/terrain state.
static inline TerrainState *get_terrain_state()
{
	auto func = RVA(0x9690).ptr<TerrainState*(*)(void)>();
	return func();
}

// True if the current skill activated successfully.
static inline bool& skill_succeeded()
{
	return *RVA(0x93c897).ptr<bool*>();
}

// Returns state value for the current skill.
// Value is writable and persists between calls.
// Custom skills should use this to maintain their internal state.
static inline uint32_t& get_skill_state()
{
	return *RVA(0x941fa4).ptr<uint32_t*>();
}

// Returns pointer to array of 1024 SkillData.
static inline SkillData* get_skill_data()
{
	return RVA(0x8b64c0).ptr<SkillData*>();
}

// Returns pointer to array of 512 PuppetData.
static inline PuppetData* get_puppet_data()
{
	return RVA(0x8f20e8).ptr<PuppetData*>();
}

PartyPuppet decrypt_puppet(const PartyPuppet *puppet);
PartyPuppet encrypt_puppet(const PartyPuppet *puppet);

// Apply status effect(s) and display the animation for them.
// Call each frame until zero is returned.
int apply_status_effect(int player, Status status1, Status status2);

// Resets animation when setting multiple statuses on the same turn.
static inline void reset_status_anim()
{
	*RVA(0x93c890).ptr<uint32_t*>() = 0;
}

// stat_index = hp, f.atk, f.def, s.atk, s.def, speed starting from 0.
unsigned int calculate_stat(unsigned int level, unsigned int stat_index, EncryptedPuppet *puppet, unsigned int alt_form = 0);
static inline unsigned int calculate_stat(unsigned int stat_index, PartyPuppet *puppet, unsigned int alt_form = 0)
{
	return calculate_stat(puppet->level, stat_index, (EncryptedPuppet*)puppet, alt_form);
}

// returns activator function for normal skills
static inline SkillCall get_default_activator()
{
	return RVA(0x1a2ee0).ptr<SkillCall>();
}
