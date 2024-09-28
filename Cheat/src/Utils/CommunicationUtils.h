#pragma once

#include "../Communication/Communication.h"

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
            ModuleInformation moduleInformation{};
            moduleInformation.Name = name;

            info.Request = MODULE;
            info.Size = sizeof(moduleInformation);
            info.Information = &moduleInformation;
            Communication::Send(&info, sizeof(info));

            return moduleInformation.Address;
        }
    }
}
