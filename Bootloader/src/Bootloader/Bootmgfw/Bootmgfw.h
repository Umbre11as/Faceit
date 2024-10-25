#pragma once

#include <Uefi.h> // Types
#include <Protocol/LoadedImage.h> // EFI_LOADED_IMAGE Protocol

#include "../../Utils/Utils.h"

#include "ImgArchStartBootApplicationHook.h"

#define DEBUG_BOOTMGFW L"\\bootmgfw.efi"
#define BOOTMGFW_FILE_PATH L"\\EFI\\Microsoft\\Boot\\bootmgfw.efi"

EFI_STATUS BootmgfwLoadImage(OUT EFI_DEVICE_PATH** DevicePath, OUT EFI_HANDLE* ImageHandle, OUT EFI_LOADED_IMAGE** LoadedImage);
