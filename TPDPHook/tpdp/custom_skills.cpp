#include "hook_tpdp.h"
#include "custom_skills.h"
#include "tpdp_data.h"
#include "../hook.h"
#include"../log.h"
#include "../ini_parser.h"

// Implementations for custom skills go here.
// Register skill functions to their effect ids in init_custom_skills()
// at the bottom of this file.



static int frame_count = 0; // timekeeping for skills

// called to see if the skill should activate or not
int __cdecl example_skill_activator(int player)
{
	// skill fails if other puppet has weather resist
	if(get_battle_state(!player)->active_ability == 117)
	{
		skill_succeeded() = false;
		return 0;
	}

	skill_succeeded() = true;
	return 1;
}

// Example of a custom skill function, based on the disassembly of original skills.
// The game will call your function once every frame until you return zero.
// Use get_skill_state() to maintain state between calls (it will be zero on first call).
// The 'player' parameter will be 0 for the local player, and 1 for the opponent.
// See tpdp_data.h for known game data/functions you can access from here.
int __cdecl example_skill(int player)
{
	auto& skill_state = get_skill_state();	// get_skill_state() returns writable reference type
	auto battle_data = get_battle_data();	// pointer to global battle state

	PartyPuppet *user_puppet = battle_data[player].state.active_puppet;		// the puppet using the skill
	PartyPuppet *other_puppet = battle_data[!player].state.active_puppet;	// the puppet on the opposing side

	// puppets in the battle data need to be decrypted.
	// only the level, hp, sp, status_effects, and stopped_turns fields of party puppets are unencrypted and may be accessed directly.
	PartyPuppet decrypted_user_puppet = decrypt_puppet(user_puppet);
	PartyPuppet decrypted_other_puppet = decrypt_puppet(other_puppet);

	// stat_index = hp, f.atk, f.def, s.atk, s.def, speed starting from 0.
	// here we pass 0 to calculate the HP stat for both puppets.
	auto user_max_hp = (ushort)calculate_stat(0, &decrypted_user_puppet);
	auto other_max_hp = (ushort)calculate_stat(0, &decrypted_other_puppet);

	switch(skill_state)
	{
	case 0: // initial state, first call
		frame_count = 0;

		if(skill_succeeded())
		{
			// set weather to 5 turns of dust storm
			auto state = get_terrain_state();
			state->weather_type = WEATHER_DUSTSTORM;
			state->weather_duration = 5;

			reset_weather_msg();
			skill_state = 1;
			return 1;
		}

		skill_state = 3;
		break;

	case 1: // show weather message
		if(do_weather_msg() == 0)
		{
			// set terrain to 5 turns of genbu
			auto state = get_terrain_state();
			state->terrain_type = TERRAIN_GENBU;
			state->terrain_duration = 5;

			reset_terrain_msg();
			clear_battle_text();
			skill_state = 2;
			return 1;
		}
		break;

	case 2: // show terrain message
		if(do_terrain_msg() == 0)
		{
			clear_battle_text();
			skill_state = 4;
			return 1;
		}
		break;

	case 3: // skill failed
		if(set_battle_text("But the skill failed.", false, false) != 1)
		{
			if(++frame_count > 60)
			{
				clear_battle_text();
				skill_state = 10;
				frame_count = 0;
				return 0;
			}
		}
		break;

	case 4: // show some custom text
		if(set_battle_text("...", false, false) != 1)
		{
			if(++frame_count > 60)
			{
				clear_battle_text();
				skill_state = 5;
				frame_count = 0;
				return 1;
			}
		}
		break;

	case 5: // more custom text
		if(set_battle_text("I'm in control now :)", false, false) != 1)
		{
			if(++frame_count > 60)
			{
				clear_battle_text();
				skill_state = 6;
				frame_count = 0;
				return 1;
			}
		}
		break;

	case 6: // set both puppets hp to 1
		if(frame_count == 0)
		{
			user_puppet->hp = 1;
			other_puppet->hp = 1;
		}
		if(++frame_count > 60)
		{
			skill_state = 7;
			frame_count = 0;
			return 1;
		}
		break;

	case 7: // set both puppets back to max hp
		if(frame_count == 0)
		{
			user_puppet->hp = user_max_hp;
			other_puppet->hp = other_max_hp;
		}
		if(++frame_count > 60)
		{
			skill_state = 8;
			frame_count = 0;
			return 1;
		}
		break;

	case 8: // apply burn and weak on the user
		if(apply_status_effect(player, STATUS_BURN, STATUS_WEAK) == 0)
		{
			skill_state = 9;
			reset_status_anim();
			return 1;
		}
		break;

	case 9: // apply poison and fear on the opponent
		if(apply_status_effect(!player, STATUS_POIS, STATUS_FEAR) == 0)
		{
			skill_state = 10;
			return 0;
		}
		break;

	case 10:
		return 0; // we're done, return 0 to end the skill

	default:
		break;
	}

	return 1; // return 1 by default to keep the skill going
}

// Realm skill modified to work with timegazer/stargazer.
// Invoked from wrapper function below.
int skill_realm(int player, int terrain, int weather)
{
	auto& skill_state = get_skill_state();
	auto battle_state = get_battle_state(player);

	bool godstone_hack = IniFile::global["general"]["realm_godstone_hack"] == "true";
	bool stargazer_hack = IniFile::global["general"]["realm_stargazer_hack"] == "true";
	bool timegazer_hack = IniFile::global["general"]["realm_timegazer_hack"] == "true";
	bool has_stargazer = battle_state->active_ability == 318;
	bool has_timegazer = battle_state->active_ability == 319;

	auto held_item = decrypt_puppet(battle_state->active_puppet).held_item;

	int min_duration = 5;
	if(godstone_hack && ((held_item == 303) || ((weather + 303) == held_item)))
		min_duration = 8;

	int weather_duration = (stargazer_hack && has_stargazer) ? 0x7ffffff5 : min_duration; // value apparently used by weather skills with stargazer
	int terrain_duration = (timegazer_hack && has_timegazer) ? 8 : 5;

	switch(skill_state)
	{
	case 0: // initial state, first call
		frame_count = 0;
		if(skill_succeeded())
		{
			// set weather
			auto state = get_terrain_state();
			state->weather_type = (uint8_t)weather;
			state->weather_duration = weather_duration;

			reset_weather_msg();
			skill_state = 1;
			return 1;
		}
		skill_state = 3;
		break;

	case 1: // show weather message
		if(do_weather_msg() == 0)
		{
			// set terrain
			auto state = get_terrain_state();
			state->terrain_type = (uint8_t)terrain;
			state->terrain_duration = terrain_duration;

			reset_terrain_msg();
			clear_battle_text();
			skill_state = 2;
			return 1;
		}
		break;

	case 2: // show terrain message
		if(do_terrain_msg() == 0)
		{
			skill_state = 10;
			return 0;
		}
		break;

	case 3: // skill failed
		if(set_battle_text("But the skill failed.", 0, 0) != 1)
		{
			if(++frame_count > (is_game_60fps() ? 60 : 30))
			{
				clear_battle_text();
				skill_state = 10;
				return 0;
			}
		}
		break;

	case 10: // done
		// return 0 to end the skill
		return 0;
	}

	// return 1 to keep the skill going
	return 1;
}

// Template for generating wrapper functions to invoke
// realm skills with different arguments.
template<Terrain T, Weather U>
int __cdecl realm_dispatch(int player)
{
	return skill_realm(player, T, U);
}

// Bind skill functions to effect ids.
// You can overwrite existing ids or you can edit
// the game data to use new ids.
void init_custom_skills()
{
	// enable example skill
	// edit the game data and make a skill that uses effect id 500 to use it
	if(IniFile::global["general"]["enable_example_skill"] == "true")
	{
		register_custom_skill(500, example_skill);
		register_custom_skill_activator(500, example_skill_activator);

		// alternatively
		//register_custom_skill_activator(500, get_default_activator());
	}

	// register modified realm skills for stargazer/timegazer
	bool godstone_hack = IniFile::global["general"]["realm_godstone_hack"] == "true";
	bool stargazer_hack = IniFile::global["general"]["realm_stargazer_hack"] == "true";
	bool timegazer_hack = IniFile::global["general"]["realm_timegazer_hack"] == "true";
	if(godstone_hack || stargazer_hack || timegazer_hack)
	{
		register_custom_skill(247, realm_dispatch<TERRAIN_SUZAKU, WEATHER_HEAVYFOG>);
		register_custom_skill(248, realm_dispatch<TERRAIN_BYAKKO, WEATHER_AURORA>);
		register_custom_skill(249, realm_dispatch<TERRAIN_SEIRYU, WEATHER_CALM>);
		register_custom_skill(250, realm_dispatch<TERRAIN_KOHRYU, WEATHER_DUSTSTORM>);
		register_custom_skill(251, realm_dispatch<TERRAIN_GENBU, WEATHER_SUNSHOWER>);
	}
}
