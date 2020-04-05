
/* AUTO GENERATED classes for tpdp memory structures */

#pragma once

typedef unsigned char       undefined;
typedef unsigned char       byte;
typedef unsigned int        dword;
typedef long long           longlong;
typedef char                sbyte;
typedef unsigned char       uchar;
typedef unsigned int        uint;
typedef unsigned long       ulong;
typedef unsigned char       undefined1;
typedef unsigned short      undefined2;
typedef unsigned int        undefined4;
typedef unsigned long long  undefined6;
typedef unsigned long long  undefined8;
typedef unsigned short      ushort;
typedef unsigned short      word;

#pragma pack(push, 1)
typedef struct BattleData BattleData, *PBattleData;

typedef struct PartyPuppet PartyPuppet, *PPartyPuppet;

typedef struct BattleState BattleState, *PBattleState;

struct BattleState {
    undefined field_0x0[9];
    ushort active_puppet_id;
    byte active_style_index;
    byte active_level;
    char active_nickname[32];
    byte active_costume_index;
    undefined field_0x2e[69];
    struct PartyPuppet * active_puppet;
    short base_puppet_id;
    undefined field_0x79;
    short active_ability;
    undefined field_0x7c[21];
    byte puppet_sp[4];
    undefined field_0x95[6];
    short active_held_item;
    undefined field_0x9d[76];
    undefined2 unk_hazard1;
    int trap_turns;
    undefined4 unk_hazard2;
    undefined field_0xf3[24];
    bool drain_seed;
    undefined field_0x10c[40];
    int num_attacks;
    undefined field_0x138[14];
    short trap_skill_id;
    undefined field_0x148[77];
    bool stealth_trap;
    bool bind_trap;
    int num_poison_trap;
    int num_mine_trap;
};

struct PartyPuppet {
    uint trainer_id;
    uint secret_id;
    char trainer_name[32];
    ushort caught_location;
    byte caught_year;
    byte caught_month;
    byte caught_day;
    byte caught_hour;
    byte caught_minute;
    char puppet_nickname[32];
    ushort puppet_id;
    byte style_index;
    byte ability_index;
    byte mark;
    byte ivs[3];
    undefined field_0x57;
    uint experience;
    ushort happiness;
    ushort pp;
    byte costume_index;
    byte evs[6];
    ushort held_item;
    ushort skills[4];
    bool heart_mark;
    undefined field_0x72[31];
    uint checksum;
    byte level;
    ushort hp;
    byte sp[4];
    byte status_effects[2];
    byte stopped_turns;
    undefined field_0x9f[6];
};

struct BattleData {
    undefined field_0x0;
    char player_name[32];
    int field_0x21;
    byte active_puppet_index;
    struct PartyPuppet puppets[6];
    struct BattleState state;
};

typedef struct EncryptedPuppet EncryptedPuppet, *PEncryptedPuppet;

struct EncryptedPuppet {
    uint trainer_id;
    uint secret_id;
    char trainer_name[32];
    ushort caught_location;
    byte caught_year;
    byte caught_month;
    byte caught_day;
    byte caught_hour;
    byte caught_minute;
    char puppet_nickname[32];
    ushort puppet_id;
    byte style_index;
    byte ability_index;
    byte mark;
    byte ivs[3];
    undefined field_0x57;
    uint experience;
    ushort happiness;
    ushort pp;
    byte costume_index;
    byte evs[6];
    ushort held_item;
    ushort skills[4];
    bool heart_mark;
    undefined field_0x72[31];
};

typedef struct PuppetData PuppetData, *PPuppetData;

typedef struct PuppetStyleData PuppetStyleData, *PPuppetStyleData;

struct PuppetStyleData {
    byte style_type;
    byte element1;
    byte element2;
    byte base_stats[6];
    ushort abilities[2];
    undefined field_0xd[4];
    ushort style_skills[11];
    undefined field_0x27[6];
    ushort lvl_100_skill;
    undefined field_0x2f[2];
    byte compat_bitfield[16];
    ushort level_70_skills[8];
    undefined field_0x51[20];
};

struct PuppetData {
    char name[32];
    byte cost;
    ushort base_skills[5];
    ushort item_drop_table[4];
    ushort puppetdex_index;
    undefined field_0x35[40];
    struct PuppetStyleData styles[4];
};

typedef struct SkillData SkillData, *PSkillData;

struct SkillData {
    char name[32];
    byte element;
    byte power;
    byte accuracy;
    byte sp;
    sbyte priority;
    byte type;
    undefined field_0x26;
    ushort effect_id;
    byte effect_chance;
    byte classification;
    byte target;
    ushort id;
    undefined field_0x2e[73];
};

typedef struct TerrainState TerrainState, *PTerrainState;

struct TerrainState {
    byte weather_type;
    byte terrain_type;
    undefined field_0x2;
    undefined field_0x3;
    uint weather_duration;
    uint terrain_duration;
};
#pragma pack(pop)

