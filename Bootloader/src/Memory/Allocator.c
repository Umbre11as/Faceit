#include "Allocator.h"

#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>

EFI_STATUS ExAllocate(IN SIZE_T Size, OUT PVOID* Buffer) {
	return gBS->AllocatePool(EfiReservedMemoryType, Size, Buffer);
}

EFI_STATUS ExFree(IN PVOID Buffer) {
	return gBS->FreePool(Buffer);
}
