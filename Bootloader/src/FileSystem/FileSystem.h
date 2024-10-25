#pragma once

#include <Library/UefiLib.h> // EFI_FILE_IO_INTERFACE
#include <Library/DevicePathLib.h> // FileDevicePath

#include "../Types/Types.h"
#include "../Logger/Logger.h"

typedef struct _VOLUME {
	SIZE_T Index;
	EFI_HANDLE Handle;
	EFI_FILE_IO_INTERFACE* FileIO;
	EFI_FILE_HANDLE FileManager;
} VOLUME, *PVOLUME;

EFI_STATUS FsListVolumes(OUT PVOLUME* Interfaces, OUT SIZE_T* Count);

EFI_STATUS FsOpenVolume(IN PVOLUME Volume);

EFI_STATUS FsCloseVolume(IN VOLUME Volume);

EFI_STATUS FsLocateFile(IN VOLUME Volume, IN const CHAR16* FileName, IN ULONGLONG OpenMode, IN ULONGLONG Attributes, OUT EFI_DEVICE_PATH** DevicePath);

EFI_STATUS FsReadFile(IN VOLUME Volume, IN const CHAR16* FileName, OUT SIZE_T* OutFileSize, OUT PVOID OutBuffer);
