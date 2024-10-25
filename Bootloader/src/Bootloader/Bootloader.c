#include "Bootloader.h"

#include <Uefi.h> // Types
#include <Library/UefiLib.h> // Print
#include <Library/UefiBootServicesTableLib.h> // gBS

#include "../Logger/Logger.h"

#include "Bootmgfw/Bootmgfw.h"

LOGGER gLogger = CONSTANT_LOGGER(L"Bootloader");

EFI_STATUS BlMain(IN EFI_SYSTEM_TABLE* SystemTable) {
    EFI_STATUS status = EFI_SUCCESS;
    LogSetupLogger();
    LogPrint(gLogger, L"wake up, nothing is real");
    LogPrint(gLogger, L"Firmware %s", SystemTable->FirmwareVendor);

    EFI_DEVICE_PATH* devicePath;
    EFI_HANDLE imageHandle;
    EFI_LOADED_IMAGE* loadedImage;
    if (EFI_ERROR(status = BootmgfwLoadImage(&devicePath, &imageHandle, &loadedImage))) {
        LogPrint(gLogger, L"An error while loading OS image: 0x%lX (%d)", status, status);
        return status;
    }

    if (EFI_ERROR(status = HkSetupImgArchStartBootHook(loadedImage))) {
        if (status == EFI_NOT_FOUND)
            LogPrint(gLogger, L"ImgArchStartBootApplication not found");
        else
            LogPrint(gLogger, L"FATAL: Cannot setup bootmgfw hooks: 0x%lX (%d)", status, status);

        return status;
    }

    if (EFI_ERROR(status = gBS->StartImage(imageHandle, NULL, NULL))) {
        LogPrint(gLogger, L"An error while starting OS: 0x%lX (%d)", status, status);
        gBS->Stall(SEC_TO_MICRO(10));
        return status;
    }

    return status;
}
