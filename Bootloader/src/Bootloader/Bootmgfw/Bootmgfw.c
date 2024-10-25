#include "Bootmgfw.h"

#include <Library/UefiBootServicesTableLib.h> // gBS

#include "../../Logger/Logger.h"
#include "../../FileSystem/FileSystem.h"
#include "../../Memory/Memory.h"

LOGGER gBootmgfwLogger = CONSTANT_LOGGER(L"BootMGFW");

EFI_STATUS BootmgfwLoadImage(OUT EFI_DEVICE_PATH** DevicePath, OUT EFI_HANDLE* ImageHandle, OUT EFI_LOADED_IMAGE** LoadedImage) {
    EFI_STATUS status = EFI_SUCCESS;

    SIZE_T count = 0;
    PVOLUME volumes;
    if (EFI_ERROR(status = FsListVolumes(&volumes, &count))) {
        LogPrint(gBootmgfwLogger, L"An error while getting volumes: 0x%lX (%d)", status, status);
        return status;
    }
    
    BOOL found = FALSE;
    for (SIZE_T i = 0; i < count; i++) {
        VOLUME volume = volumes[i];

        if (EFI_ERROR(status = FsOpenVolume(&volume)))
            continue;

        if (!EFI_ERROR(FsLocateFile(volume, BOOTMGFW_FILE_PATH, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY, DevicePath))) {
            FsCloseVolume(volume);
            found = TRUE;
            break;
        }

        if (EFI_ERROR(status = FsCloseVolume(volume)))
            continue;
    }

    if (!found) {
        LogPrint(gBootmgfwLogger, L"No file found (bootmgfw.efi)");
        return EFI_NOT_FOUND;
    }

    CHAR16* fullDevicePath = ConvertDevicePathToText(*DevicePath, TRUE, TRUE);
    LogPrint(gBootmgfwLogger, L"Found OS image file: %s", fullDevicePath);
    ExFree(fullDevicePath);

    if (EFI_ERROR(status = gBS->LoadImage(TRUE, gImageHandle, *DevicePath, NULL, 0, ImageHandle))) {
        LogPrint(gBootmgfwLogger, L"An error while loading OS image: 0x%lX (%d)", status, status);
        return status;
    }

    if (EFI_ERROR(status = gBS->HandleProtocol(*ImageHandle, &gEfiLoadedImageProtocolGuid, LoadedImage))) {
        LogPrint(gBootmgfwLogger, L"Cannot query information about OS image: 0x%lX (%d)", status, status);
        return status;
    }

    LogPrint(gBootmgfwLogger, L"OS Image at 0x%lX", (*LoadedImage)->ImageBase);
    return status;
}
