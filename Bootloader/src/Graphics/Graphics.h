#pragma once

#include "../Types/Types.h"
#include "Image/Image.h"

#define RGB_CHANNEL 3
#define RGBA_CHANNEL 4

#define ENCODE_COLOR_RGB(r, g, b) (r << 16) | (g << 8) | b
#define ENCODE_COLOR_RGBA(r, g, b, a) (r << 24) | (g << 16) | (b << 8) | a

#define COLOR_WHITE_RGB ENCODE_COLOR_RGB(255, 255, 255)
#define COLOR_BLACK_RGB 0
#define COLOR_RED_RGB ENCODE_COLOR_RGB(255, 0, 0)
#define COLOR_GREEN_RGB ENCODE_COLOR_RGB(0, 255, 0)
#define COLOR_BLUE_RGB ENCODE_COLOR_RGB(0, 0, 255)

#define COLOR_WHITE_RGBA ENCODE_COLOR_RGBA(255, 255, 255, 255)
#define COLOR_BLACK_RGBA ENCODE_COLOR_RGBA(0, 0, 0, 255)
#define COLOR_RED_RGBA ENCODE_COLOR_RGBA(255, 0, 0, 255)
#define COLOR_GREEN_RGBA ENCODE_COLOR_RGBA(0, 255, 0, 255)
#define COLOR_BLUE_RGBA ENCODE_COLOR_RGBA(0, 0, 255, 255)

EFI_STATUS GxSetBestResolution(IN UINT Width, IN UINT Height);

EFI_STATUS GxScreenSize(OUT UINT* Width, OUT UINT* Height);

EFI_STATUS GxFillPixel(IN UINT X, IN UINT Y, IN int Color, IN UINT Channels);

EFI_STATUS GxFillRect(IN UINT X, IN UINT Y, IN UINT Width, IN UINT Height, IN int Color, IN UINT Channels);

EFI_STATUS GxFillRoundRect(IN UINT X, IN UINT Y, IN UINT Width, IN UINT Height, IN float Radius, IN int Color, IN UINT Channels);

EFI_STATUS GxFillCircle(IN UINT X, IN UINT Y, IN float Radius, IN int Color, IN UINT Channels);

EFI_STATUS GxDisplayCircle(IN UINT X, IN UINT Y, IN float Radius, IN int Color, IN UINT Channels);

EFI_STATUS GxDisplayImageEx(IN PIMAGE Image, IN UINT X, IN UINT Y, IN BYTE Alpha);

inline EFI_STATUS GxDisplayImage(IN PIMAGE Image, IN UINT X, IN UINT Y) {
	return GxDisplayImageEx(Image, X, Y, 255);
}

inline int GxRGBApplyAlpha(IN int RGB, IN BYTE alpha) {
	BYTE r = (RGB >> 16) & 0xFF;
	BYTE g = (RGB >> 8) & 0xFF;
	BYTE b = RGB & 0xFF;
	float a = alpha / 255.0f;

	return ENCODE_COLOR_RGB((BYTE) (r * a), (BYTE) (g * a), (BYTE) (b * a));
}

inline int GxRGBAApplyAlpha(IN int RGB, IN BYTE alpha) {
	BYTE r = (RGB >> 24) & 0xFF;
	BYTE g = (RGB >> 16) & 0xFF;
	BYTE b = (RGB >> 8) & 0xFF;

	return ENCODE_COLOR_RGBA(r, g, b, alpha);
}
