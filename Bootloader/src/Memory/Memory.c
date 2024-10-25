#include "Memory.h"

PVOID MmCopyMemory(OUT PVOID Dst, IN PCVOID Src, IN SIZE_T Size) {
    PBYTE dstBytes = Dst;
    PCBYTE srcBytes = Src;

    while (Size--)
        *dstBytes++ = *srcBytes++;

    return Dst;
}

PVOID MmMoveMemory(OUT PVOID Dst, IN PCVOID Src, IN SIZE_T Size) {
    PBYTE dstBytes = Dst;
    PCBYTE srcBytes = Src;

    if (srcBytes < dstBytes) {
        srcBytes += Size;
        dstBytes += Size;
        while (Size--)
            *--dstBytes = *--srcBytes;
    }
    else {
        while (Size--)
            *dstBytes++ = *srcBytes++;
    }

    return Dst;
}

PVOID MmSetMemory(OUT PVOID Dst, IN int Character, IN SIZE_T Size) {
    PBYTE dstBytes = Dst;

    while (Size > 0) {
        *dstBytes = Character;
        dstBytes++;

        Size--;
    }

    return Dst;
}

int MmCompareMemory(IN PCVOID Buffer1, IN PCVOID Buffer2, IN SIZE_T Size) {
    PBYTE buffer1Bytes = Buffer1;
    PBYTE buffer2Bytes = Buffer2;
    int result = 0;

    if (Buffer1 == Buffer2)
        return result;

    while (Size > 0) {
        if (*buffer1Bytes != *buffer2Bytes) {
            result = (*buffer1Bytes > *buffer2Bytes) ? 1 : -1;
            break;
        }

        Size--;
        buffer1Bytes++;
        buffer2Bytes++;
    }

    return result;
}
