#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wimplicit-exception-spec-mismatch"
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#pragma once

#include <ntifs.h>

// Сорян за такие тупые названия
namespace Allocator {
    PVOID AllocateKernel(IN SIZE_T size) {
        return ExAllocatePool(NonPagedPool, size);
    }

    void FreeKernel(IN PVOID buffer) {
        ExFreePool(buffer);
    }

    PVOID AllocatePhysical(IN SIZE_T size, IN PVOID virtualAddress, IN ULONG protect = 0, OUT PMDL* outMdl = nullptr) {
        PMDL mdl = IoAllocateMdl(virtualAddress, size, FALSE, FALSE, nullptr);
        MmBuildMdlForNonPagedPool(mdl);

        PVOID mappingAddress = MmMapLockedPagesSpecifyCache(mdl, KernelMode, MmCached, nullptr, FALSE, NormalPagePriority);

        if (protect > 0)
            MmProtectMdlSystemAddress(mdl, protect);

        if (outMdl)
            *outMdl = mdl;

        return mappingAddress;
    }

    void FreePhysical(IN PVOID buffer, IN PMDL mdl) {
        MmUnmapLockedPages(buffer, mdl);
        MmUnlockPages(mdl);
        IoFreeMdl(mdl);
    }
}

PVOID operator new(IN SIZE_T size) {
    return Allocator::AllocateKernel(size);
}

void operator delete(IN PVOID buffer) {
    Allocator::FreeKernel(buffer);
}

PVOID operator new[](IN SIZE_T size) {
    return Allocator::AllocateKernel(size);
}

void operator delete[](IN PVOID buffer, IN SIZE_T) {
    Allocator::FreeKernel(buffer);
}

#pragma clang diagnostic pop