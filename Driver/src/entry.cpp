#pragma clang diagnostic push
#pragma ide diagnostic ignored "bugprone-reserved-identifier"
#pragma clang diagnostic ignored "-Wmicrosoft-cast"

#include "Routine/Routine.h"

enum Request : unsigned int {
    MODULE,
    ATTACH,
    READ_VIRTUAL
};

struct CommunicateInfo {
    ULONG Request;
    SIZE_T Size;
    PVOID Information;
};

struct ModuleInformation {
    PCSTR Name;
    ULONGLONG Address;
};

struct ReadMemoryInformation {
    PVOID Address;
    PVOID Buffer;
    SIZE_T Size;
    NTSTATUS* Status;
};

PEPROCESS process = nullptr;

void Communicate(PVOID buffer, SIZE_T size) {
    if (size != sizeof(CommunicateInfo))
        return;

    auto info = reinterpret_cast<CommunicateInfo*>(buffer);
    switch (info->Request) {
        case MODULE: {
            auto moduleInfo = reinterpret_cast<ModuleInformation*>(info->Information);
            moduleInfo->Address = reinterpret_cast<ULONGLONG>(Process::GetModuleBaseProcess(process, String(moduleInfo->Name)));

            break;
        }
        case ATTACH: {
            DWORD pid = *reinterpret_cast<DWORD*>(info->Information);
            PsLookupProcessByProcessId(reinterpret_cast<HANDLE>(pid), &process);

            break;
        }
        case READ_VIRTUAL: {
            auto readInfo = reinterpret_cast<ReadMemoryInformation*>(info->Information);
            *readInfo->Status = Memory::ReadVirtualMemory(process, readInfo->Buffer, readInfo->Address, readInfo->Size);

            break;
        }
    }
}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT, IN PUNICODE_STRING) {
    NTSTATUS status = Communication::Setup(new FunctionPointerSwapPipe, Communicate);
    if (!NT_SUCCESS(status))
        Log("Cannot setup communication");

    return STATUS_SUCCESS;
}

#pragma clang diagnostic pop