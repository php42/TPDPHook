#pragma once
typedef int(__cdecl*SkillCall)(int);

void tpdp_install_hooks();
void register_custom_skill(unsigned int effect_id, SkillCall skill_func);
void register_custom_skill_activator(unsigned int effect_id, SkillCall activator_func);
