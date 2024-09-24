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

    NTSTATUS GetModuleBase(IN PEPROCESS process, IN PCSTR moduleName, OUT ModuleInfo* outInfo) {
        PPEB peb = PsGetProcessPeb(process);
        if (!peb)
            return STATUS_BUFFER_ALL_ZEROS;

        PUNICODE_STRING moduleUnicode = String(moduleName).UnicodeString();

        // TODO: Bypass attach @ https://github.com/Umbre11as/DetectAttachProcess
        KAPC_STATE state;
        KeStackAttachProcess(process, &state);

        PPEB_LDR_DATA ldr = peb->Ldr;
        if (!ldr) {
            KeUnstackDetachProcess(&state);
            return STATUS_BUFFER_ALL_ZEROS;
        }

        PLIST_ENTRY list = &ldr->InLoadOrderModuleList;
        for (PLIST_ENTRY entry = ldr->InLoadOrderModuleList.Flink; entry != list; entry = entry->Flink) {
            PLDR_DATA_TABLE_ENTRY ldrTableEntry = CONTAINING_RECORD(list, LDR_DATA_TABLE_ENTRY, InLoadOrderLinks);

            if (RtlCompareUnicodeString(&ldrTableEntry->BaseDllName, moduleUnicode, true) == 0) {
                outInfo->Base = ldrTableEntry->DllBase;
                outInfo->Size = ldrTableEntry->SizeOfImage;
                return STATUS_SUCCESS;
            }
        }

        KeUnstackDetachProcess(&state);
        return STATUS_NOT_FOUND;
    }
}
