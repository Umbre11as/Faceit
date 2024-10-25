#include "Modules.h"

namespace Modules {
    NTSTATUS GetSystemModule(IN PCSTR path, OUT ModuleInfo* outInfo) {
        ULONG size = 0;
        ZwQuerySystemInformation(SystemModuleInformation, nullptr, size, &size);
        if (size <= 0)
            return STATUS_INVALID_BUFFER_SIZE;

        auto processModules = reinterpret_cast<PRTL_PROCESS_MODULES>(Allocator::AllocateKernel(size));
        ZwQuerySystemInformation(SystemModuleInformation, processModules, size, &size);
        if (!processModules)
            return STATUS_BUFFER_ALL_ZEROS;

        RTL_PROCESS_MODULE_INFORMATION moduleInformation;
        for (ULONG i = 0; i < processModules->NumberOfModules; i++) {
            moduleInformation = processModules->Modules[i];
            if (strcmp(moduleInformation.FullPathName, path) == 0) {
                outInfo->Base = moduleInformation.ImageBase;
                outInfo->Size = moduleInformation.ImageSize;

                Allocator::FreeKernel(processModules);
                return STATUS_SUCCESS;
            }
        }

        Allocator::FreeKernel(processModules);
        return STATUS_NOT_FOUND;
    }

    PVOID GetSystemModuleBase(IN PCSTR path) {
        ModuleInfo moduleInformation{};
        if (NT_SUCCESS(GetSystemModule(path, &moduleInformation)))
            return moduleInformation.Base;

        return nullptr;
    }

    PVOID GetExport(IN PVOID moduleBase, IN PCSTR functionName) {
        return RtlFindExportedRoutineByName(moduleBase, functionName);
    }
}
