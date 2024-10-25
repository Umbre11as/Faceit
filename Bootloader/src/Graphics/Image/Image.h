#pragma once

#include "../../Types/Types.h"
#include "../../Memory/Memory.h"

typedef struct _IMAGE {
    BYTE Signature[5];
    UINT Width, Height;
    int* Pixels;
} IMAGE, *PIMAGE;

BOOL ImgCheckImage(IN PIMAGE Image);

PIMAGE ImgLoadRaw(IN PBYTE Data, IN SIZE_T Size);
