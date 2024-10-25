#include "Logo.h"

#include <Library/UefiBootServicesTableLib.h> // gBS

#include "../../FileSystem/FileSystem.h"

#define LOGO_PATH L"\\Resources\\logo.efimage"

static LOGGER logoLogger = CONSTANT_LOGGER(L"Logo");

PIMAGE logoImage = NULL;

EFI_STATUS BlSetupLogo() {
	//logo.efimage
	EFI_STATUS status = EFI_SUCCESS;
	SIZE_T count = 0;
	PVOLUME volumes;
	if (EFI_ERROR(status = FsListVolumes(&volumes, &count))) {
		LogPrint(logoLogger, L"An error while getting volumes: 0x%lX (%d)", status, status);
		return status;
	}

	PVOID buffer = NULL;
	SIZE_T fileSize = 0;
	for (SIZE_T i = 0; i < count; i++) {
		VOLUME volume = volumes[i];

		if (EFI_ERROR(status = FsOpenVolume(&volume)))
			continue;
		
		if (!EFI_ERROR(FsReadFile(volume, LOGO_PATH, &fileSize, buffer))) {
			FsCloseVolume(volume);
			break;
		}

		if (EFI_ERROR(status = FsCloseVolume(volume)))
			continue;
	}

	if (fileSize <= 0) {
		LogPrint(logoLogger, L"File not found (Resources\\logo.efimage)");
		return EFI_NOT_FOUND;
	}

	logoImage = ImgLoadRaw(buffer, fileSize);
	if (!logoImage)
		return EFI_BUFFER_TOO_SMALL;

	return EFI_SUCCESS;
}

EFI_STATUS BlDisplayLogoEx(IN UINT ScreenWidth, IN UINT ScreenHeight, IN BOOL Animation) {
	if (!logoImage)
		return EFI_BUFFER_TOO_SMALL;

	EFI_STATUS status = EFI_SUCCESS;
	
	UINT logoX = ScreenWidth / 2 - logoImage->Width / 2;
	UINT logoY = ScreenHeight / 2 - logoImage->Height / 2;

	if (Animation) {
		for (BYTE i = 0; i < 255; i += 15) {
			if (EFI_ERROR(status = GxDisplayImageEx(logoImage, logoX, logoY, i)))
				return status;

			gBS->Stall(500);
		}
	} else
		status = GxDisplayImage(logoImage, logoX, logoY);

	return status;
}
