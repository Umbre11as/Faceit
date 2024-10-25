#pragma once

#include <Uefi.h>
#include "../Memory/Allocator.h"

typedef struct _LOGGER {
	const CHAR16* Name;
} LOGGER, *PLOGGER;

void LogSetupLogger();

void LogPrint(IN LOGGER Logger, IN const CHAR16* Text, IN OPTIONAL ...);

#define CONSTANT_LOGGER(s) { s }
