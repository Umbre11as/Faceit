#pragma once

#include <ntifs.h>

namespace Memory {
    // лол
    NTSTATUS ReadFromReadOnly(IN PVOID address, OUT PVOID buffer, IN SIZE_T size) {
        PMDL mdl = IoAllocateMdl(address, size, false, false, nullptr);
        if (!mdl)
            return STATUS_BUFFER_ALL_ZEROS;

        __try {
            MmProbeAndLockPages(mdl, KernelMode, IoReadAccess);
        } __except (EXCEPTION_EXECUTE_HANDLER) {
            IoFreeMdl(mdl);
            return STATUS_ACCESS_DENIED;
        }

        PVOID mappedAddress = MmMapLockedPagesSpecifyCache(mdl, KernelMode, MmNonCached, nullptr, false, NormalPagePriority);
        if (!mappedAddress) {
            MmUnlockPages(mdl);
            IoFreeMdl(mdl);
            return STATUS_INVALID_ADDRESS;
        }

        MmProtectMdlSystemAddress(mdl, PAGE_READWRITE);

        RtlCopyMemory(buffer, address, size);

        MmUnmapLockedPages(mappedAddress, mdl);
        MmUnlockPages(mdl);
        IoFreeMdl(mdl);
        return STATUS_SUCCESS;
    }

    NTSTATUS WriteToReadOnly(IN PVOID address, IN PVOID buffer, IN SIZE_T size) {
        PMDL mdl = IoAllocateMdl(address, size, false, false, nullptr);
        if (!mdl)
            return STATUS_BUFFER_ALL_ZEROS;

        __try {
            MmProbeAndLockPages(mdl, KernelMode, IoReadAccess);
        } __except (EXCEPTION_EXECUTE_HANDLER) {
            IoFreeMdl(mdl);
            return STATUS_ACCESS_DENIED;
        }

        PVOID mappedAddress = MmMapLockedPagesSpecifyCache(mdl, KernelMode, MmNonCached, nullptr, false, NormalPagePriority);
        if (!mappedAddress) {
            MmUnlockPages(mdl);
            IoFreeMdl(mdl);
            return STATUS_INVALID_ADDRESS;
        }

        MmProtectMdlSystemAddress(mdl, PAGE_READWRITE);

        RtlCopyMemory(mappedAddress, buffer, size);

        MmUnmapLockedPages(mappedAddress, mdl);
        MmUnlockPages(mdl);
        IoFreeMdl(mdl);
        return STATUS_SUCCESS;
    }
}
