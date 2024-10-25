#include "Hook.h"

#include "../Memory/Memory.h"

void HkInlineHook(IN PVOID Address, IN PVOID Detour, OUT PHOOK Hook) {
	unsigned char jmp[14] = {
		0xFF, 0x25, 0x0, 0x0, 0x0, 0x0,
		0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0
	};

	MmCopyMemory(jmp + 6, &Detour, sizeof(Detour));

	Hook->Address = Address;
	MmCopyMemory(Hook->Original, Address, sizeof(jmp));
	MmCopyMemory(Hook->Address, jmp, sizeof(jmp));
}

void HkInlineUnhook(IN PHOOK Hook) {
	MmCopyMemory(Hook->Address, Hook->Original, 14);
}
