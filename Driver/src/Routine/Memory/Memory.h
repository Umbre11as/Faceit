#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-narrowing-conversions"
#pragma once

#include <ntifs.h>
#include <intrin.h>

extern "C" {
    PVOID NTAPI MmMapIoSpaceEx(IN PHYSICAL_ADDRESS PhysicalAddress, IN SIZE_T NumberOfBytes, IN ULONG Protect);
}

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

    void CopyList(IN PLIST_ENTRY original, IN PLIST_ENTRY copy, IN KPROCESSOR_MODE mode) {
        if (IsListEmpty(&original[mode]))
            InitializeListHead(&copy[mode]);
        else {
            copy[mode].Flink = original[mode].Flink;
            copy[mode].Blink = original[mode].Blink;
            original[mode].Flink->Blink = &copy[mode];
            original[mode].Blink->Flink = &copy[mode];
        }
    }

    void MoveApcState(IN PKAPC_STATE oldState, IN PKAPC_STATE newState) {
        memcpy(newState, oldState, sizeof(KAPC_STATE));

        CopyList(oldState->ApcListHead, newState->ApcListHead, KernelMode);
        CopyList(oldState->ApcListHead, newState->ApcListHead, UserMode);
    }

    // Credits @ https://www.unknowncheats.me/forum/anti-cheat-bypass/489305-read-write-process-attach.html

    ULONGLONG oldProcess;

    void AttachProcess(PEPROCESS process) {
        PKTHREAD thread = KeGetCurrentThread();
        PKAPC_STATE apcState = *reinterpret_cast<PKAPC_STATE*>(reinterpret_cast<ULONGLONG>(thread) + 0x98);

        if (*reinterpret_cast<UCHAR*>(reinterpret_cast<ULONGLONG>(thread) + 0x24a) == 0)
            return;

        MoveApcState(apcState, *reinterpret_cast<PKAPC_STATE*>(reinterpret_cast<ULONGLONG>(thread) + 0x258));

        InitializeListHead(&apcState->ApcListHead[KernelMode]);
        InitializeListHead(&apcState->ApcListHead[UserMode]);

        oldProcess = *reinterpret_cast<ULONGLONG*>(reinterpret_cast<ULONGLONG>(apcState) + 0x20);

        *reinterpret_cast<PEPROCESS*>(reinterpret_cast<ULONGLONG>(apcState) + 0x20) = process;
        *reinterpret_cast<UCHAR*>(reinterpret_cast<ULONGLONG>(apcState) + 0x28) = 0;
        *reinterpret_cast<UCHAR*>(reinterpret_cast<ULONGLONG>(apcState) + 0x29) = 0;
        *reinterpret_cast<UCHAR*>(reinterpret_cast<ULONGLONG>(apcState) + 0x2a) = 0;
        *reinterpret_cast<UCHAR*>(reinterpret_cast<ULONGLONG>(thread) + 0x24a) = 1;

        auto directoryTableBase = *reinterpret_cast<ULONGLONG*>(reinterpret_cast<ULONGLONG>(process) + 0x28);
        __writecr3(directoryTableBase);
    }

    PHYSICAL_ADDRESS SafeMmGetPhysicalAddress(IN PVOID address) {
        static BOOLEAN* KdEnteredDebugger = nullptr;
        if (!KdEnteredDebugger) {
            UNICODE_STRING functionName;
            RtlInitUnicodeString(&functionName, L"KdEnteredDebugger");

            KdEnteredDebugger = reinterpret_cast<BOOLEAN*>(MmGetSystemRoutineAddress(&functionName));
        }

        *KdEnteredDebugger = TRUE;
        PHYSICAL_ADDRESS PhysicalAddress = MmGetPhysicalAddress(address);
        *KdEnteredDebugger = FALSE;

        return PhysicalAddress;
    }

    void DetachProcess() {
        PKTHREAD thread = KeGetCurrentThread();
        PKAPC_STATE apcState = *reinterpret_cast<PKAPC_STATE*>(reinterpret_cast<ULONGLONG>(thread) + 0x98);

        if (*reinterpret_cast<UCHAR*>(reinterpret_cast<ULONGLONG>(thread) + 0x24a) == 0)
            return;

        MoveApcState(*reinterpret_cast<PKAPC_STATE*>(reinterpret_cast<ULONGLONG>(thread) + 0x258), apcState);

        if (oldProcess)
            *reinterpret_cast<ULONGLONG*>(reinterpret_cast<ULONGLONG>(apcState) + 0x20) = oldProcess;

        *reinterpret_cast<PEPROCESS*>(*reinterpret_cast<uintptr_t*>(reinterpret_cast<ULONGLONG>(thread) + 0x258) + 0x20) = nullptr;
        *reinterpret_cast<UCHAR*>(reinterpret_cast<ULONGLONG>(thread) + 0x24a) = 0;

        auto directoryTableBase = *reinterpret_cast<ULONGLONG*>(reinterpret_cast<ULONGLONG>(*reinterpret_cast<PEPROCESS*>(reinterpret_cast<ULONGLONG>(apcState) + 0x20)) + 0x28);
        __writecr3(directoryTableBase);

        if (!(IsListEmpty(&apcState->ApcListHead[KernelMode])))
            *reinterpret_cast<UCHAR*>(reinterpret_cast<ULONGLONG>(apcState) + 0x29) = 1;

        RemoveEntryList(&apcState->ApcListHead[KernelMode]);
        oldProcess = 0;
    }

    NTSTATUS SafeReadVirtual(PEPROCESS process, PVOID dst, PVOID src, SIZE_T size) {
        AttachProcess(process);
        if (!MmIsAddressValid(src)) {
            DetachProcess();
            return STATUS_INVALID_ADDRESS;
        }

        PHYSICAL_ADDRESS srcPhysical = SafeMmGetPhysicalAddress(src);
        DetachProcess();

        if (!srcPhysical.QuadPart)
            return STATUS_UNSUCCESSFUL;

        // TODO: MmMapIoSpaceEx, memcpy bypass
        PVOID mappedIoSpace = MmMapIoSpaceEx(srcPhysical, size, PAGE_READWRITE);
        if (!mappedIoSpace)
            return STATUS_UNSUCCESSFUL;

        memcpy(dst, mappedIoSpace, size);

        MmUnmapIoSpace(mappedIoSpace, size);
        return STATUS_SUCCESS;
    }
}

#pragma clang diagnostic pop