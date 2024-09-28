#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-pro-type-member-init"
#pragma once

#include <Windows.h>
#include <winternl.h>
#include <ntstatus.h>

typedef struct _SYSTEM_PROCESS_INFO {
    ULONG NextEntryOffset;
    ULONG NumberOfThreads;
    LARGE_INTEGER Reserved[3];
    LARGE_INTEGER CreateTime;
    LARGE_INTEGER UserTime;
    LARGE_INTEGER KernelTime;
    UNICODE_STRING ImageName;
    ULONG BasePriority;
    HANDLE ProcessId;
    HANDLE InheritedFromProcessId;
} SYSTEM_PROCESS_INFO, *PSYSTEM_PROCESS_INFO;

namespace Utils {
    namespace Process {
        bool FindProcess(PWSTR name, DWORD* outPid) {
            NTSTATUS status = STATUS_UNSUCCESSFUL;
            ULONG length = sizeof(SYSTEM_PROCESS_INFO);
            PSYSTEM_PROCESS_INFO buffer;

            do {
                buffer = static_cast<PSYSTEM_PROCESS_INFO>(VirtualAlloc(nullptr, length, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE));
                if (!buffer)
                    return false;
                status = NtQuerySystemInformation(SystemProcessInformation, buffer, length, &length);
            } while (status == STATUS_INFO_LENGTH_MISMATCH);
            if (!NT_SUCCESS(status))
                return false;

            do {
                buffer = reinterpret_cast<PSYSTEM_PROCESS_INFO>(reinterpret_cast<PBYTE>(buffer) + buffer->NextEntryOffset);
                if (wcscmp(buffer->ImageName.Buffer, name) == 0) {
                    *outPid = HandleToULong(buffer->ProcessId);
                    VirtualFree(buffer, length, MEM_RELEASE);
                    return true;
                }
            } while (buffer->NextEntryOffset);

            VirtualFree(buffer, length, MEM_RELEASE);
            return false;
        }
    }
}

#pragma clang diagnostic pop