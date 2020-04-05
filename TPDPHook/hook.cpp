#include <Windows.h>
#include <sstream>
#include "hook.h"
#include "log.h"

uintptr_t RVA::base_ = 0;

bool patch_memory(void *dst, const void *src, std::size_t sz)
{
	DWORD oldprotect;

	if(VirtualProtect(dst, sz, PAGE_EXECUTE_READWRITE, &oldprotect))
	{
		memcpy(dst, src, sz);

		VirtualProtect(dst, sz, oldprotect, &oldprotect);

		return true;
	}

	std::wstringstream msg;
	msg << L"VirtualProtect failed:\n"
		<< std::hex << std::showbase
		<< L"src: " << (uintptr_t)src << L"\n"
		<< L"dst: " << (uintptr_t)dst << L"\n"
		<< L"size: " << sz;
	log_fatal(msg.str());

	return false;
}

bool patch_call(void *mem, const void *dest_addr)
{
	unsigned int opcode = *(uint8_t*)mem;
	if(opcode != 0xE8u)
	{
		std::wstringstream msg;
		msg << std::hex << std::showbase << L"Failed to patch call, unexpected opcode: " << opcode << L"\n"
			<< L"address: " << (uintptr_t)mem << L"\n"
			<< L"dst: " << (uintptr_t)dest_addr;

		log_fatal(msg.str());
	}

	uint8_t buf[] = { 0xE8, 0, 0, 0, 0 }; // call disp32
	auto disp = ((uintptr_t)dest_addr - (uintptr_t)mem) - sizeof(buf); // address of dest relative to the *next* instruction
	memcpy(&buf[1], &disp, 4);
	return patch_memory(mem, buf, sizeof(buf));
}

bool patch_jump(void *mem, const void *dest_addr)
{
	uint8_t buf[] = { 0xE9, 0, 0, 0, 0 };
	auto disp = ((uintptr_t)dest_addr - (uintptr_t)mem) - sizeof(buf);
	memcpy(&buf[1], &disp, 4);
	return patch_memory(mem, buf, sizeof(buf));
}
