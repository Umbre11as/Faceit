#include "PEUtils.h"

#include <Library/UefiLib.h>
#include "../Types/PE.h"

PVOID PeGetExport(IN PVOID Base, IN const CHAR8* FunctionName) {
    PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER) Base;
    PIMAGE_NT_HEADERS ntHeaders = (PIMAGE_NT_HEADERS)((PBYTE) Base + dosHeader->e_lfanew);
    PIMAGE_OPTIONAL_HEADER optionalHeader = &ntHeaders->OptionalHeader;

    DWORD exportsRva = optionalHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
    if (exportsRva) {
        PIMAGE_EXPORT_DIRECTORY exportDirectory = (PIMAGE_EXPORT_DIRECTORY)((PBYTE) Base + exportsRva);

        DWORD* namesArray = (DWORD*)((PBYTE) Base + exportDirectory->AddressOfNames);
        DWORD* addressArray = (DWORD*)((PBYTE) Base + exportDirectory->AddressOfFunctions);
        WORD* ordinalsArray = (WORD*)((PBYTE) Base + exportDirectory->AddressOfNameOrdinals);

        for (ULONG i = 0; i < exportDirectory->NumberOfNames; i++) {
            CHAR8* name = (CHAR8*)((PBYTE) Base + namesArray[i]);
            if (AsciiStrCmp(name, FunctionName) == 0)
                return (PVOID)((PBYTE) Base + addressArray[ordinalsArray[i]]);
        }
    }

    return NULL;
}

ULONG PeFetchImageSize(IN PVOID Base) {
    PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER) Base;
    PIMAGE_NT_HEADERS ntHeaders = (PIMAGE_NT_HEADERS)((PBYTE) Base + dosHeader->e_lfanew);
    PIMAGE_OPTIONAL_HEADER optionalHeader = &ntHeaders->OptionalHeader;

    return optionalHeader->SizeOfImage;
}
