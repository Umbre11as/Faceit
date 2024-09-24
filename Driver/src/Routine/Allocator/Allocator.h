#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wimplicit-exception-spec-mismatch"
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#pragma once

#include <ntifs.h>

// Сорян за такие тупые названия
namespace Allocator {
    PVOID AllocateKernel(IN SIZE_T size);

    void FreeKernel(IN PVOID buffer);

    PVOID AllocatePhysical(IN SIZE_T size, IN PVOID virtualAddress, IN ULONG protect = 0, OUT PMDL* outMdl = nullptr);

    void FreePhysical(IN PVOID buffer, IN PMDL mdl);
}

PVOID operator new(IN SIZE_T size);

void operator delete(IN PVOID buffer);

PVOID operator new[](IN SIZE_T size);

void operator delete[](IN PVOID buffer, IN SIZE_T);

#pragma clang diagnostic pop