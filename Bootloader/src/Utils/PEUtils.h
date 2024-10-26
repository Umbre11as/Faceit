#pragma once

#include "../Types/Types.h"

PVOID PeGetExport(IN PVOID Base, IN const CHAR8* FunctionName);

ULONG PeFetchImageSize(IN PVOID Base);
