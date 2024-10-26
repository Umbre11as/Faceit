#include "Mapper.h"

#include "../../Types/PE.h"
#include "../../Memory/Memory.h"
#include "../../Utils/PEUtils.h"

#ifndef MAPPER_SHOW_LOG
#define MAPPER_SHOW_LOG 0
#endif

#if MAPPER_SHOW_LOG
#include "../../String/String.h"
#include <Library/UefiLib.h>
#endif

void MpMapAndResolveDriver(IN PVOID DriverBuffer, IN PVOID MapBuffer, IN PVOID NtoskrnlBase, OUT PVOID* EntryPoint) {
    PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER) DriverBuffer;
    PIMAGE_NT_HEADERS ntHeaders = (PIMAGE_NT_HEADERS) ((PBYTE) DriverBuffer + dosHeader->e_lfanew);
    PIMAGE_OPTIONAL_HEADER optionalHeader = &ntHeaders->OptionalHeader;

    MmCopyMemory(MapBuffer, DriverBuffer, optionalHeader->SizeOfHeaders);

    PIMAGE_SECTION_HEADER sectionHeader = IMAGE_FIRST_SECTION(ntHeaders);
    for (USHORT i = 0; i != ntHeaders->FileHeader.NumberOfSections; i++) {
        if (sectionHeader->SizeOfRawData)
            MmCopyMemory((PBYTE) MapBuffer + sectionHeader->VirtualAddress, (PBYTE) DriverBuffer + sectionHeader->PointerToRawData, sectionHeader->SizeOfRawData);

        sectionHeader++;
    }

    PIMAGE_DATA_DIRECTORY relocationDirectory = &optionalHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC];
    if (relocationDirectory->VirtualAddress) {
        PIMAGE_BASE_RELOCATION relocation = (PIMAGE_BASE_RELOCATION) ((PBYTE) MapBuffer + relocationDirectory->VirtualAddress);
        for (UINT size = 0; size < relocationDirectory->Size; ) {
            UINT relocationCount = (relocation->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(WORD);
            USHORT* relocationData = (USHORT*)((PBYTE) relocation + sizeof(IMAGE_BASE_RELOCATION));
            PBYTE relocationBase = (PBYTE) MapBuffer + relocation->VirtualAddress;

            for (UINT i = 0; i < relocationCount; i++) {
                USHORT data = *relocationData;
                WORD type = data >> 12;
                WORD offset = data & 0xFFF;

                switch (type) {
                    case IMAGE_REL_BASED_ABSOLUTE:
                        break;
                    case IMAGE_REL_BASED_DIR64: {
                        UINT64* rva = (UINT64*)(relocationBase + offset);
                        *rva = (UINT64)((PBYTE) MapBuffer + (*rva - ntHeaders->OptionalHeader.ImageBase));
                        break;
                    }
                    default:
                        return;
                }

                relocationData++;
            }

            size += relocation->SizeOfBlock;
            relocation = (PIMAGE_BASE_RELOCATION) relocationData;
        }
    }

    DWORD importsRva = optionalHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
    if (importsRva) {
        PIMAGE_IMPORT_DESCRIPTOR importDescriptor = (PIMAGE_IMPORT_DESCRIPTOR)((PBYTE) MapBuffer + importsRva);

        while (importDescriptor->Characteristics) {
#if MAPPER_SHOW_LOG
            CHAR8* name = (CHAR8*)((PBYTE) mapBuffer + importDescriptor->Name);
            Print(L"Name: %s\n", SsAsciiToUnicode(name));
            if (AsciiStrCmp(name, "ntoskrnl.exe") != 0)
                Print(L"Cannot resolve import\n");
#endif

            PIMAGE_THUNK_DATA64 thunkData = (PIMAGE_THUNK_DATA64) ((PBYTE) MapBuffer + importDescriptor->FirstThunk);
            PIMAGE_THUNK_DATA64 originalThunkData = (PIMAGE_THUNK_DATA64) ((PBYTE) MapBuffer + importDescriptor->OriginalFirstThunk);
            while (thunkData->u1.AddressOfData) {
                PIMAGE_IMPORT_BY_NAME importByName = (PIMAGE_IMPORT_BY_NAME)((PBYTE) MapBuffer + thunkData->u1.AddressOfData);
                CHAR8* importName = (CHAR8*)(importByName->Name);
                PVOID resolvedAddress = PeGetExport(NtoskrnlBase, importName);
#if MAPPER_SHOW_LOG
                Print(L"Function name: %s\n", SsAsciiToUnicode(importName));
                Print(L"Address from ntoskrnl.exe: %p\n", resolvedAddress);
#endif

                thunkData->u1.Function = (QWORD) resolvedAddress;

                thunkData++;
                originalThunkData++;
            }

            importDescriptor++;
        }
    }

    *EntryPoint = (PBYTE) MapBuffer + optionalHeader->AddressOfEntryPoint;
}
