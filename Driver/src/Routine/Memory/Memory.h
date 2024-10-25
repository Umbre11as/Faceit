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

    // Credits @ https://www.unknowncheats.me/forum/anti-cheat-bypass/489305-read-write-process-attach.html

    void CopyList(IN PLIST_ENTRY Original, IN PLIST_ENTRY Copy, IN KPROCESSOR_MODE Mode) {
        if (IsListEmpty(&Original[Mode]))
            InitializeListHead(&Copy[Mode]);
        else {
            Copy[Mode].Flink = Original[Mode].Flink;
            Copy[Mode].Blink = Original[Mode].Blink;
            Original[Mode].Flink->Blink = &Copy[Mode];
            Original[Mode].Blink->Flink = &Copy[Mode];
        }
    }

    void MoveApcState(IN PKAPC_STATE OldState, OUT PKAPC_STATE NewState) {
	    RtlCopyMemory(NewState, OldState, sizeof(KAPC_STATE));

        CopyList(OldState->ApcListHead, NewState->ApcListHead, KernelMode);
        CopyList(OldState->ApcListHead, NewState->ApcListHead, UserMode);
    }

    uintptr_t OldProcess;

    void AttachProcess(IN PEPROCESS process) {
        PKTHREAD Thread = KeGetCurrentThread();

        PKAPC_STATE ApcState = *(PKAPC_STATE*)(uintptr_t(Thread) + 0x98); // 0x98 = _KTHREAD::ApcState

        if (*(PEPROCESS*)(uintptr_t(ApcState) + 0x20) == process) // 0x20 = _KAPC_STATE::Process
            return;

        if ((*(UCHAR*)(uintptr_t(Thread) + 0x24a) != 0)) // 0x24a = _KTHREAD::ApcStateIndex
            return;

        MoveApcState(ApcState, *(PKAPC_STATE*)(uintptr_t(Thread) + 0x258)); // 0x258 = _KTHREAD::SavedApcState

        InitializeListHead(&ApcState->ApcListHead[KernelMode]);
        InitializeListHead(&ApcState->ApcListHead[UserMode]);

        OldProcess = *(uintptr_t*)(uintptr_t(ApcState) + 0x20);

        *(PEPROCESS*)(uintptr_t(ApcState) + 0x20) = process; // 0x20 = _KAPC_STATE::Process
        *(UCHAR*)(uintptr_t(ApcState) + 0x28) = 0;				// 0x28 = _KAPC_STATE::InProgressFlags
        *(UCHAR*)(uintptr_t(ApcState) + 0x29) = 0;				// 0x29 = _KAPC_STATE::KernelApcPending
        *(UCHAR*)(uintptr_t(ApcState) + 0x2a) = 0;				// 0x2a = _KAPC_STATE::UserApcPendingAll

        *(UCHAR*)(uintptr_t(Thread) + 0x24a) = 1; // 0x24a = _KTHREAD::ApcStateIndex

        auto DirectoryTableBase = *(QWORD*)(QWORD(process) + 0x28);  // 0x28 = _EPROCESS::DirectoryTableBase
        __writecr3(DirectoryTableBase);
    }

    void DetachProcess() {
        PKTHREAD Thread = KeGetCurrentThread();
        PKAPC_STATE ApcState = *(PKAPC_STATE*)(uintptr_t(Thread) + 0x98); // 0x98 = _KTHREAD->ApcState

        if ((*(UCHAR*)(uintptr_t(Thread) + 0x24a) == 0)) // 0x24a = KTHREAD->ApcStateIndex
            return;

        if ((*(UCHAR*)(uintptr_t(ApcState) + 0x28)) ||  // 0x28 = _KAPC_STATE->InProgressFlags
            !(IsListEmpty(&ApcState->ApcListHead[KernelMode])) ||
            !(IsListEmpty(&ApcState->ApcListHead[UserMode])))
        {
            KeBugCheck(INVALID_PROCESS_DETACH_ATTEMPT);
        }

        MoveApcState(*(PKAPC_STATE*)(uintptr_t(Thread) + 0x258), ApcState); // 0x258 = _KTHREAD::SavedApcState

        if (OldProcess)
            *(uintptr_t*)(uintptr_t(ApcState) + 0x20) = OldProcess; // 0x20 = _KAPC_STATE::Process

        *(PEPROCESS*)(*(uintptr_t*)(uintptr_t(Thread) + 0x258) + 0x20) = 0; // 0x258 = _KTHREAD::SavedApcState + 0x20 = _KAPC_STATE::Process

        *(UCHAR*)(uintptr_t(Thread) + 0x24a) = 0; // 0x24a = _KTHREAD::ApcStateIndex

        auto DirectoryTableBase = *(QWORD*)(QWORD(*(PEPROCESS*)(uintptr_t(ApcState) + 0x20)) + 0x28); // 0x20 = _KAPC_STATE::Process + 0x28 = _EPROCESS::DirectoryTableBase
        __writecr3(DirectoryTableBase);

        if (!(IsListEmpty(&ApcState->ApcListHead[KernelMode])))
        {
            *(UCHAR*)(QWORD(ApcState) + 0x29) = 1; // 0x29 = _KAPC_STATE::KernelApcPending
        }

        RemoveEntryList(&ApcState->ApcListHead[KernelMode]);

        OldProcess = 0;
    }

    // TODO: Use pml4 table to get physical address
	PHYSICAL_ADDRESS SafeMmGetPhysicalAddress(IN PVOID virtualAddress) {
        static BOOLEAN* KdEnteredDebugger = nullptr;
        if (!KdEnteredDebugger) {
            UNICODE_STRING UniCodeFunctionName = RTL_CONSTANT_STRING(L"KdEnteredDebugger");
            KdEnteredDebugger = reinterpret_cast<BOOLEAN*>(MmGetSystemRoutineAddress(&UniCodeFunctionName));
        }

        *KdEnteredDebugger = TRUE;
        PHYSICAL_ADDRESS PhysicalAddress = MmGetPhysicalAddress(virtualAddress);
        *KdEnteredDebugger = FALSE;

        return PhysicalAddress;
    }

	NTSTATUS ReadVirtualMemory(IN PEPROCESS Process, OUT PVOID Destination, IN PVOID Source, IN SIZE_T Size) {
        NTSTATUS ntStatus = STATUS_UNSUCCESSFUL;
        PHYSICAL_ADDRESS SourcePhysicalAddress;
        PVOID MappedIoSpace;
        BOOLEAN IsAttached;

        AttachProcess(Process);
        IsAttached = TRUE;

        if (!MmIsAddressValid(Source))
            goto _Exit;

        SourcePhysicalAddress = SafeMmGetPhysicalAddress(Source);

        DetachProcess();
        IsAttached = FALSE;

        if (!SourcePhysicalAddress.QuadPart)
            return ntStatus;

        MappedIoSpace = MmMapIoSpaceEx(SourcePhysicalAddress, Size, PAGE_READWRITE);
        if (!MappedIoSpace)
            goto _Exit;

        memcpy(Destination, MappedIoSpace, Size);

        MmUnmapIoSpace(MappedIoSpace, Size);

        ntStatus = STATUS_SUCCESS;

        _Exit:

            if (IsAttached)
                DetachProcess();

        return ntStatus;
    }
}

#pragma clang diagnostic pop