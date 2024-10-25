#include "FileSystem.h"

#include <Library/UefiBootServicesTableLib.h> // gBS
#include <Guid/FileInfo.h> // gEfiFileInfoGuid

#include "../Memory/Allocator.h"

EFI_STATUS FsListVolumes(OUT PVOLUME* Interfaces, OUT SIZE_T* Count) {
	EFI_STATUS status = EFI_SUCCESS;

	EFI_HANDLE* handles = NULL;
	if (EFI_ERROR((status = gBS->LocateHandleBuffer(ByProtocol, &gEfiSimpleFileSystemProtocolGuid, NULL, Count, &handles)))) {
		return status;
	}

	if (EFI_ERROR(status = ExAllocate(1 * sizeof(VOLUME), Interfaces)))
		return status;

	for (SIZE_T i = 0; i < *Count; i++) {
		VOLUME volume;
		volume.Index = i;
		volume.Handle = handles[i];
		(*Interfaces)[i] = volume;
	}

	return status;
}

EFI_STATUS FsOpenVolume(IN PVOLUME Volume) {
	if (!Volume->Handle)
		return EFI_BUFFER_TOO_SMALL;

	EFI_STATUS status = EFI_SUCCESS;
	if (EFI_ERROR(status = gBS->OpenProtocol(Volume->Handle, &gEfiSimpleFileSystemProtocolGuid, &Volume->FileIO, gImageHandle, NULL, EFI_OPEN_PROTOCOL_GET_PROTOCOL)))
		return status;

	return Volume->FileIO->OpenVolume(Volume->FileIO, &Volume->FileManager);
}


EFI_STATUS FsCloseVolume(IN VOLUME Volume) {
	EFI_STATUS status = EFI_SUCCESS;

	if (EFI_ERROR(status = gBS->CloseProtocol(Volume.Handle, &gEfiSimpleFileSystemProtocolGuid, gImageHandle, NULL)))
		return status;

	return status;
}

EFI_STATUS FsLocateFile(IN VOLUME Volume, IN const CHAR16* FileName, IN ULONGLONG OpenMode, IN ULONGLONG Attributes, OUT EFI_DEVICE_PATH** DevicePath) {
	EFI_STATUS status = EFI_SUCCESS;

	EFI_FILE_HANDLE fileHandle;
	if (EFI_ERROR(status = Volume.FileManager->Open(Volume.FileManager, &fileHandle, FileName, OpenMode, Attributes)))
		return status;

	if (EFI_ERROR(status = Volume.FileManager->Close(fileHandle)))
		return status;

	*DevicePath = FileDevicePath(Volume.Handle, FileName);
	return status;
}

EFI_STATUS FsReadFile(IN VOLUME Volume, IN const CHAR16* FileName, OUT SIZE_T* OutFileSize, OUT PVOID OutBuffer) {
	EFI_STATUS status = EFI_SUCCESS;

	EFI_FILE_HANDLE fileHandle;
	if (EFI_ERROR(status = Volume.FileManager->Open(Volume.FileManager, &fileHandle, FileName, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY)))
		return status;

	SIZE_T fileInfoSize = 0;
	EFI_FILE_INFO* fileInfoBuffer = NULL;

	if (EFI_ERROR(status = fileHandle->GetInfo(fileHandle, &gEfiFileInfoGuid, &fileInfoSize, NULL))) {
		if (status == EFI_BUFFER_TOO_SMALL) {
			ExAllocate(fileInfoSize, &fileInfoBuffer);
		} else
			return status;
	}

	if (EFI_ERROR(status = fileHandle->GetInfo(fileHandle, &gEfiFileInfoGuid, &fileInfoSize, fileInfoBuffer)))
		return status;

	*OutFileSize = fileInfoBuffer->FileSize;
	if (EFI_ERROR(status = fileHandle->Read(fileHandle, OutFileSize, OutBuffer)))
		return status;

	if (EFI_ERROR(status = Volume.FileManager->Close(fileHandle)))
		return status;

	return status;
}
