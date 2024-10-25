#pragma once

#include "../../Types/Types.h"
#include "../../Graphics/Graphics.h"

EFI_STATUS BlSetupLogo();

EFI_STATUS BlDisplayLogoEx(IN UINT ScreenWidth, IN UINT ScreenHeight, IN BOOL Animation);
