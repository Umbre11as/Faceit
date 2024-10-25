#pragma once

#include "Logo.h"

static UINT screenWidth = 1920;
static UINT screenHeight = 1080;

EFI_STATUS BlSetupGraphics();

EFI_STATUS BlDisplayLogo(IN BOOL Animation);

void BlUpdateProgress();

EFI_STATUS BlRenderProgressBar();

void BlRenderAndUpdateProgress(IN UINT Delay);
