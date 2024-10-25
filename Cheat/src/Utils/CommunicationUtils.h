#pragma once

#include <ntstatus.h>
#include "../Communication/Communication.h"

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

namespace Utils {
    namespace Communications {
        void Attach(DWORD pid) {
            CommunicateInfo info{};
            info.Request = ATTACH;
            info.Size = sizeof(DWORD);
            info.Information = &pid;

            Communication::Send(&info, sizeof(info));
        }

        ULONGLONG GetModuleAddress(PCSTR name) {
            CommunicateInfo info{};
            ModuleInformation moduleInfo{};
            moduleInfo.Name = name;

            info.Request = MODULE;
            info.Size = sizeof(moduleInfo);
            info.Information = &moduleInfo;
            Communication::Send(&info, sizeof(info));

            return moduleInfo.Address;
        }

        template<typename T>
        T Read(ULONGLONG Address) {
            T buffer{};
            if (Address == 0)
                return buffer;

            NTSTATUS status = STATUS_SUCCESS;

            CommunicateInfo info{};
            ReadMemoryInformation readInfo{};
            readInfo.Address = reinterpret_cast<PVOID>(Address);
            readInfo.Buffer = &buffer;
            readInfo.Size = sizeof(T);
            readInfo.Status = &status;

            info.Request = READ_VIRTUAL;
            info.Size = sizeof(readInfo);
            info.Information = &readInfo;
            Communication::Send(&info, sizeof(info));

            return buffer;
        }
    }
}
