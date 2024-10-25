#pragma once

#include "../Types/Types.h"

#define ZeroMemory(Dst, Size) MmSetMemory(Dst, 0, Size)

PVOID MmCopyMemory(OUT PVOID Dst, IN PCVOID Src, IN SIZE_T Size);

PVOID MmMoveMemory(OUT PVOID Dst, IN PCVOID Src, IN SIZE_T Size);

PVOID MmSetMemory(OUT PVOID Dst, IN int Character, IN SIZE_T Size);

int MmCompareMemory(IN PCVOID Buffer1, IN PCVOID Buffer2, IN SIZE_T Size);
