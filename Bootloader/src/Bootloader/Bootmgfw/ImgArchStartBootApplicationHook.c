#include "ImgArchStartBootApplicationHook.h"

#include <Library/UefiBootServicesTableLib.h> // gBS

#include "../../Utils/Utils.h"

#include "../Graphics/BLGraphics.h"
#include "../Winload/Winload.h"

typedef EFI_STATUS(EFIAPI* IMG_ARCH_START_BOOT_APPLICATION)(PVOID, PVOID, UINT, BYTE, PVOID);
HOOK HookImgArchStartBoot;

EFI_STATUS EFIAPI ImgArchStartBootApplicationDetour(PVOID AppEntry, PVOID ImageBase, UINT ImageSize, BYTE BootOption, PVOID ReturnArgs) {
	HkInlineUnhook(&HookImgArchStartBoot);

	BlDisplayLogo(FALSE);

	HkSetupWinloadHooks(ImageBase, ImageSize);
	
	BlUpdateProgress();
	BlRenderAndUpdateProgress(9000);
	return CALL_CAST(HookImgArchStartBoot, IMG_ARCH_START_BOOT_APPLICATION, AppEntry, ImageBase, ImageSize, BootOption, ReturnArgs);
}

EFI_STATUS HkSetupImgArchStartBootHook(IN EFI_LOADED_IMAGE* LoadedImage) {
	EFI_STATUS status = EFI_SUCCESS;
    
	PVOID signature = SigFindSignature(LoadedImage->ImageBase, LoadedImage->ImageSize, "48 8B C4 48 89 58 ? 44 89 40 ? 48 89 50 ? 48 89 48 ? 55 56 57 41 54 41 55 41 56 41 57 48 8D 68 ? 48 81 EC ? ? ? ? 48 8B F9");
	if (!signature)
		return EFI_NOT_FOUND;
	
	LogPrint(gHooksLogger, L"ImgArchStartBootApplication: 0x%lX", signature);

	HkInlineHook(signature, &ImgArchStartBootApplicationDetour, &HookImgArchStartBoot);
	return status;
}
