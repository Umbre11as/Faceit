#include "Allocator.h"

namespace Allocator {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations" // Allows windows support up to 1909
    PVOID AllocateKernel(IN SIZE_T size) {
        return ExAllocatePool(NonPagedPool, size);
    }
#pragma clang diagnostic pop

    void FreeKernel(IN PVOID buffer) {
        ExFreePool(buffer);
    }

    PVOID AllocatePhysical(IN SIZE_T size, IN PVOID virtualAddress, IN ULONG protect, OUT PMDL* outMdl) {
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
