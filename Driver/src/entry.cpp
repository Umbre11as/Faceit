#pragma clang diagnostic push
#pragma ide diagnostic ignored "bugprone-reserved-identifier"
#pragma clang diagnostic ignored "-Wmicrosoft-cast"

#include "Routine/Routine.h"

enum Request : unsigned int {
    MODULE,
    ATTACH
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

PEPROCESS process = nullptr;

void Communicate(PVOID buffer, SIZE_T size) {
    if (size != sizeof(CommunicateInfo))
        return;

    auto info = reinterpret_cast<CommunicateInfo*>(buffer);
    switch (info->Request) {
        case MODULE: {
            auto moduleInfo = reinterpret_cast<ModuleInformation*>(info->Information);
            moduleInfo->Address = reinterpret_cast<ULONGLONG>(Process::GetModuleBase(process, String(moduleInfo->Name)));

            break;
        }
        case ATTACH: {
            DWORD pid = *reinterpret_cast<DWORD*>(info->Information);
            PsLookupProcessByProcessId(reinterpret_cast<HANDLE>(pid), &process);

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