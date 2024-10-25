#include "Logger.h"

#include <Library/PrintLib.h> // UnicodeSPrint
#include <Library/UefiRuntimeServicesTableLib.h> // gRT

#include <stdarg.h>

#include "C:\Users\admin\Desktop\img2efi\font_page.h"

#include "../Graphics/Graphics.h"
#include "../Memory/Memory.h"

static PIMAGE font;

void LogSetupLogger() {
	font = ImgLoadRaw(rawData, sizeof(rawData));
}

void LogPrint(IN LOGGER Logger, IN const CHAR16* Text, IN OPTIONAL ...) {
	VA_LIST marker;
	VA_START(marker, Text);

	CHAR16 formatted[256];
	UnicodeVSPrint(formatted, sizeof(formatted), Text, marker);

	VA_END(marker);

	CHAR16 timeStr[64];
	EFI_TIME time;
	if (EFI_ERROR(gRT->GetTime(&time, NULL)))
		MmCopyMemory(timeStr, L"NaN", 7);
	else
		UnicodeSPrint(timeStr, sizeof(timeStr), L"%02d:%02d", time.Hour, time.Minute);

	//GxDisplayImage(font, 0, 0);
	Print(L"[%s] [%s]: %s\n", timeStr, Logger.Name, formatted);
}
