#include "Graphics.h"

#include <Protocol/GraphicsOutput.h>
#include <Library/UefiBootServicesTableLib.h>
#include "../Memory/Allocator.h"
#include "../Math/Math.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wextern-initializer"
extern int _fltused = 0;
#pragma clang diagnostic pop

EFI_GRAPHICS_OUTPUT_PROTOCOL* GxGOPInstance() {
	static EFI_GRAPHICS_OUTPUT_PROTOCOL* gop = NULL;
	if (!gop) {
		SIZE_T count = 0;
		EFI_HANDLE* handles;
		if (EFI_ERROR(gBS->LocateHandleBuffer(ByProtocol, &gEfiGraphicsOutputProtocolGuid, NULL, &count, &handles)))
			return NULL;

		for (SIZE_T i = 0; i < count; i++)
			if (!EFI_ERROR(gBS->HandleProtocol(handles[i], &gEfiGraphicsOutputProtocolGuid, &gop)))
				break;
	}

	return gop;
}

// Credits @ HackBGRT
EFI_STATUS GxSetBestResolution(IN UINT Width, IN UINT Height) {
	EFI_GRAPHICS_OUTPUT_PROTOCOL* gop = GxGOPInstance();
	if (!gop)
		return EFI_PROTOCOL_ERROR;

	UINT bestMode = gop->Mode->Mode;
	UINT bestWidth = gop->Mode->Info->HorizontalResolution;
	UINT bestHeight = gop->Mode->Info->VerticalResolution;
	Width = (Width <= 0 ? Width < 0 ? bestWidth : 999999 : Width);
	Height = (Height <= 0 ? Height < 0 ? bestHeight : 999999 : Height);

	for (UINT i = gop->Mode->MaxMode; i--; ) {
		UINT newWidth = 0, newHeight = 0;

		EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* info = NULL;
		SIZE_T infoSize = 0;
		if (EFI_ERROR(gop->QueryMode(gop, i, &infoSize, &info)))
			continue;

		if (infoSize < sizeof(*info)) {
			ExFree(info);
			continue;
		}

		newWidth = info->HorizontalResolution;
		newHeight = info->VerticalResolution;
		ExFree(info);

		int newMissing = MAX(Width - newWidth, 0) + MAX(Height - newHeight, 0);
		int bestMissing = MAX(Width - bestWidth, 0) + MAX(Height - bestHeight, 0);
		if (newMissing > bestMissing)
			continue;

		int newOver = MAX(-Width + newWidth, 0) + MAX(-Height + newHeight, 0);
		int bestOver = MAX(-Width + bestWidth, 0) + MAX(-Height + bestHeight, 0);
		if (newMissing == bestMissing && newOver >= bestOver)
			continue;

		if (newWidth > Width || newHeight > Height)
			continue;

		bestWidth = newWidth;
		bestHeight = newHeight;
		bestMode = i;
	}

	if (bestMode != gop->Mode->Mode)
		return gop->SetMode(gop, bestMode);

	return EFI_SUCCESS;
}

EFI_STATUS GxScreenSize(OUT UINT* Width, OUT UINT* Height) {
	EFI_GRAPHICS_OUTPUT_PROTOCOL* gop = GxGOPInstance();
	if (!gop)
		return EFI_PROTOCOL_ERROR;
	
	EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* info = NULL;
	UINTN infoSize = 0;
	EFI_STATUS status = gop->QueryMode(gop, gop->Mode->Mode, &infoSize, &info);
	if (EFI_ERROR(status))
		return status;

	*Width = info->HorizontalResolution;
	*Height = info->VerticalResolution;
	return status;
}

EFI_STATUS GxFillPixel(IN UINT X, IN UINT Y, IN int Color, IN UINT Channels) {
	EFI_GRAPHICS_OUTPUT_PROTOCOL* gop = GxGOPInstance();
	if (!gop)
		return EFI_PROTOCOL_ERROR;

	if (Channels < 3 || Channels > 4)
		return EFI_INVALID_PARAMETER;

	BYTE offset = (Channels == 3 ? 16 : 24);
	BYTE r = (Color >> offset) & 0xFF;
	offset -= 8;
	BYTE g = (Color >> offset) & 0xFF;
	offset -= 8;
	BYTE b = (Color >> offset) & 0xFF;

	BYTE a = 255;
	if (Channels == 4) {
		offset -= 8;
		a = (Color >> offset) & 0xFF;
	}

	float alpha = a / 255.0f;

	EFI_GRAPHICS_OUTPUT_BLT_PIXEL bltPixel;
	bltPixel.Red = (BYTE) (r * alpha);
	bltPixel.Green = (BYTE) (g * alpha);
	bltPixel.Blue = (BYTE) (b * alpha);

	return gop->Blt(gop, &bltPixel, EfiBltVideoFill, X, Y, X, Y, 1, 1, 0);
}

EFI_STATUS GxFillRect(IN UINT X, IN UINT Y, IN UINT Width, IN UINT Height, IN int Color, IN UINT Channels) {
	UINT X2 = X + Width;
	UINT Y2 = Y + Height;

	if (X > X2) {
		int temp = X;
		X = X2;
		X2 = temp;
	}

	if (Y > Y2) {
		int temp = Y;
		Y = Y2;
		Y2 = temp;
	}

	for (int y = Y; y <= Y2; y++)
		for (int x = X; x <= X2; x++)
			GxFillPixel(x, y, Color, Channels);

	return EFI_SUCCESS;
}

EFI_STATUS GxFillRoundRect(IN UINT X, IN UINT Y, IN UINT Width, IN UINT Height, IN float Radius, IN int Color, IN UINT Channels) {
	if (Radius > (Height / 2))
		return EFI_INVALID_PARAMETER;

	UINT X2 = X + Width;
	UINT Y2 = Y + Height;

	GxFillRect(X + Radius, Y, Width - 2 * Radius, Height, Color, Channels);
	GxFillRect(X, Y + Radius, Width, Height - 2 * Radius, Color, Channels);

	GxFillCircle(X + Radius, Y + Radius, Radius, Color, Channels);
	GxFillCircle(X2 - Radius, Y + Radius, Radius, Color, Channels);
	GxFillCircle(X + Radius, Y2 - Radius, Radius, Color, Channels);
	GxFillCircle(X2 - Radius, Y2 - Radius, Radius, Color, Channels);

	return EFI_SUCCESS;
}

EFI_STATUS GxFillCircle(IN UINT X, IN UINT Y, IN float Radius, IN int Color, IN UINT Channels) {
	for (int y = -Radius; y <= Radius; y++) {
		for (int x = -Radius; x <= Radius; x++) {
			float distSq = MthPow(x, 2) + MthPow(y, 2);
			float dist = MthSqrt(distSq);
			if (dist <= Radius)
				GxFillPixel((UINT) (X + x), (UINT) (Y + y), Color, Channels);
		}
	}
	
	return EFI_SUCCESS;
}

EFI_STATUS GxDisplayCircle(IN UINT X, IN UINT Y, IN float Radius, IN int Color, IN UINT Channels) {
	// https://en.wikipedia.org/wiki/Midpoint_circle_algorithm
	float x = Radius;
	float y = 0;
	float decisionOver2 = 1 - x;

	while (x >= y) {
		GxFillPixel((UINT) (X + x), (UINT) (Y + y), Color, Channels);
		GxFillPixel((UINT) (X + x), (UINT) (Y - y), Color, Channels);
		GxFillPixel((UINT) (X - x), (UINT) (Y + y), Color, Channels);
		GxFillPixel((UINT) (X - x), (UINT) (Y - y), Color, Channels);
		GxFillPixel((UINT) (X + y), (UINT) (Y + x), Color, Channels);
		GxFillPixel((UINT) (X + y), (UINT) (Y - x), Color, Channels);
		GxFillPixel((UINT) (X - y), (UINT) (Y + x), Color, Channels);
		GxFillPixel((UINT) (X - y), (UINT) (Y - x), Color, Channels);

		y++;

		if (decisionOver2 < 0)
			decisionOver2 += 2 * y + 1;
		else {
			x--;
			decisionOver2 += 2 * (y - x) + 1;
		}
	}

	return EFI_SUCCESS;
}

EFI_STATUS GxDisplayImageEx(IN PIMAGE Image, IN UINT X, IN UINT Y, IN BYTE Alpha) {
	EFI_STATUS status = EFI_SUCCESS;

	for (UINT x = 0; x < Image->Width; x++) {
		for (UINT y = 0; y < Image->Height; y++) {
			int color = Image->Pixels[y * Image->Width + x];
			BYTE oldAlpha = color & 0xFF;
			BYTE newAlpha = (UINT) (oldAlpha * (Alpha / 255.0f));
			color = GxRGBAApplyAlpha(color, newAlpha);

			if (EFI_ERROR(status = GxFillPixel(X + x, Y + y, color, 4)))
				return status;
		}
	}

	return status;
}
