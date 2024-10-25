#include "Image.h"

#include "../../Memory/Allocator.h"

const BYTE ImageSignature[] = { 0xA, 0xB, 0x0, 0xB, 0xA };

BOOL CheckImage(IN PIMAGE Image) {
    return MmCompareMemory(ImageSignature, Image->Signature, sizeof(ImageSignature)) == 0;
}

PIMAGE ImgLoadRaw(IN PBYTE Data, IN SIZE_T Size) {
    SIZE_T cursor = 0;

    BYTE signature[5];
    MmCopyMemory(signature, Data, sizeof(signature));
    if (MmCompareMemory(ImageSignature, signature, sizeof(signature)) != 0)
        return NULL;
    cursor += sizeof(signature);

    UINT width = 0, height = 0;
    MmCopyMemory(&width, Data + cursor, sizeof(width));
    cursor += sizeof(width);
    MmCopyMemory(&height, Data + cursor, sizeof(height));
    cursor += sizeof(height);

    int* pixels = NULL;
    SIZE_T pixelsSize = width * height * sizeof(int);
    ExAllocate(pixelsSize, &pixels);
    MmCopyMemory(pixels, Data + cursor, pixelsSize);

    PIMAGE image = NULL;
    ExAllocate(sizeof(image), &image);
    
    MmCopyMemory(image->Signature, signature, sizeof(signature));
    image->Width = width;
    image->Height = height;
    image->Pixels = pixels;
    return image;
}
