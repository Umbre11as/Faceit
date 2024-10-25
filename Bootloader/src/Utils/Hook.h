#pragma once

#include "../Types/Types.h"
#include "../Logger/Logger.h"

#define CALL_CAST(hook, typedefinition, ...) ((typedefinition) hook.Address)(__VA_ARGS__)

static LOGGER gHooksLogger = CONSTANT_LOGGER(L"Hooks");

typedef struct _HOOK {
	PVOID Address;
	UINT8 Original[14];
} HOOK, *PHOOK;

void HkInlineHook(IN PVOID Address, IN PVOID Detour, OUT PHOOK Hook);

void HkInlineUnhook(IN PHOOK Hook);
