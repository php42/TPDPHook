#include <cstring>
#include "tpdp_data.h"

static inline void decrypt_puppet(void *src, const void *rand_data)
{
	uint8_t *buf = (uint8_t*)src;
	const uint8_t *randbuf = (const uint8_t*)rand_data;
	constexpr std::size_t len = 0x91;

	for(unsigned int i = 0; i < (len / 3); ++i)
	{
		int index = (i * 3) % len;
		uint32_t crypto = *(uint32_t*)(&randbuf[(i * 4) & 0x3fff]);

		/* need the higher half of this multiplication, right shifted 1 */
		uint32_t temp = (uint64_t(uint64_t(0xAAAAAAABu) * uint64_t(crypto)) >> 33);
		temp *= 3;

		if(crypto - temp == 0)
			buf[index] = ~buf[index];
		buf[index] -= uint8_t(crypto);
	}
}

static inline void encrypt_puppet(void *src, const void *rand_data)
{
	uint8_t *buf = (uint8_t*)src;
	const uint8_t *randbuf = (const uint8_t*)rand_data;
	constexpr std::size_t len = 0x91;

	for(unsigned int i = 0; i < (len / 3); ++i)
	{
		int index = (i * 3) % len;
		uint32_t crypto = *(uint32_t*)(&randbuf[(i * 4) & 0x3fff]);

		uint32_t temp = (uint64_t(uint64_t(0xAAAAAAABu) * uint64_t(crypto)) >> 33);
		temp *= 3;

		buf[index] += uint8_t(crypto);
		if(crypto - temp == 0)
			buf[index] = ~buf[index];
	}
}

PartyPuppet decrypt_puppet(const PartyPuppet *puppet)
{
	PartyPuppet tmp;
	auto rand_data = RVA(0x8d40c0).ptr<void*>(); // efile.bin
	memcpy(&tmp, puppet, sizeof(PartyPuppet));
	decrypt_puppet(&tmp, rand_data);
	return tmp;
}

PartyPuppet encrypt_puppet(const PartyPuppet *puppet)
{
	PartyPuppet tmp;
	auto rand_data = RVA(0x8d40c0).ptr<void*>(); // efile.bin
	memcpy(&tmp, puppet, sizeof(PartyPuppet));
	encrypt_puppet(&tmp, rand_data);
	return tmp;
}

int apply_status_effect(int player, Status status1, Status status2)
{
	auto do_status = (uintptr_t)RVA(0x27560);
	int tmp = 0;

	// fix-up for calling convention clobbered by
	// compiler optimizations (param passed in esi)
	__asm
	{
		push status1
		mov ecx, status2
		mov esi, player
		call do_status
		add esp, 4
		mov tmp, eax
	}

	return tmp;
}

unsigned int calculate_stat(unsigned int level, unsigned int stat_index, EncryptedPuppet *puppet, unsigned int alt_form)
{
	//just delegate this to the in-game function for now
	auto func = (uintptr_t)RVA(0x22f0);
	int tmp = 0;

	__asm
	{
		push alt_form
		push stat_index
		push level
		mov esi, puppet
		call func
		mov tmp, eax
	}

	return tmp;
}
