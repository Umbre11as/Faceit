#include "Bootloader/Bootloader.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wextern-initializer"
extern const UINT32 _gUefiDriverRevision = 0x200;
extern const UINT32 _gDxeRevision = 0x200;
extern CHAR8* gEfiCallerBaseName = "InsaneTweaker";

EFI_STATUS EFIAPI UefiUnload(IN EFI_HANDLE ImageHandle) {
    return EFI_ACCESS_DENIED;
}

EFI_STATUS EFIAPI UefiMain(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE* SystemTable) {
    SystemTable->ConOut->EnableCursor(SystemTable->ConOut, FALSE);
    return BlMain(SystemTable);
}

#pragma clang diagnostic pop
