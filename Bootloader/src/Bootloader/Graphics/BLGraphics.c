#include "BLGraphics.h"

#include <Library/UefiBootServicesTableLib.h> // gBS

EFI_STATUS BlSetupGraphics() {
    EFI_STATUS status = EFI_SUCCESS;

    if (EFI_ERROR(status = GxSetBestResolution(screenWidth, screenHeight)))
        return status;

    if (EFI_ERROR(status = GxScreenSize(&screenWidth, &screenHeight)))
        return status;

    if (EFI_ERROR(status = BlSetupLogo()))
        return status;

    return status;
}

EFI_STATUS BlDisplayLogo(IN BOOL Animation) {
    return BlDisplayLogoEx(screenWidth, screenHeight, Animation);
}

int progress = 0;
UINT progressWidth = 8;

// BlImgAllocateImageBuffer - 217 calls (9 with type not application), ImgArchStartBootApplication - 1, OslFwpKernelSetupPhase1 - 1
#define MAX_PROGRESS 11
#define PROGRESS_BAR_WIDTH 280
#define PROGRESS_Y 150

void BlUpdateProgress() {
    progress++;
}

EFI_STATUS BlRenderProgressBar() {
    EFI_STATUS status = EFI_SUCCESS;

    if (EFI_ERROR(status = GxFillRoundRect(screenWidth / 2 - PROGRESS_BAR_WIDTH / 2, screenHeight / 2 + PROGRESS_Y, PROGRESS_BAR_WIDTH, 6, 3, ENCODE_COLOR_RGB(38, 38, 38), RGB_CHANNEL)))
        return status;

    if (EFI_ERROR(status = GxFillRoundRect(screenWidth / 2 - PROGRESS_BAR_WIDTH / 2, screenHeight / 2 + PROGRESS_Y, progressWidth, 6, 3, ENCODE_COLOR_RGB(214, 214, 214), RGB_CHANNEL)))
        return status;

    return status;
}

void BlRenderAndUpdateProgress(IN UINT Delay) {
    UINT neededProgress = (UINT)((progress / (float) MAX_PROGRESS) * PROGRESS_BAR_WIDTH);
    for (UINT i = 0; i < 150; i++) {
        if (progressWidth >= neededProgress)
            break;

        progressWidth++;
        BlRenderProgressBar();
        gBS->Stall(Delay);
    }

    gBS->Stall(20000);
}
