#pragma once

#include "../Types/Types.h"

EFI_STATUS ExAllocate(IN SIZE_T Size, OUT PVOID* Buffer);

EFI_STATUS ExFree(IN PVOID Buffer);
