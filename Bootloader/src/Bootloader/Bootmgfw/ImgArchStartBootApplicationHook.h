#pragma once

#include <Protocol/LoadedImage.h> // EFI_LOADED_IMAGE Protocol

EFI_STATUS HkSetupImgArchStartBootHook(IN EFI_LOADED_IMAGE* LoadedImage);
