#include "Winload.h"

#include <Library/BaseLib.h> // StrnCmp
#include <Library/UefiBootServicesTableLib.h> // gBS

#include "../../Utils/Utils.h"
#include "../Graphics/BLGraphics.h"

static LOGGER gWinloadLogger = CONSTANT_LOGGER(L"Winload");

typedef enum _BL_ARCH_MODE {
	BlProtectedMode,
	BlRealMode
} BL_ARCH_MODE;

typedef void(__stdcall* BLP_ARCH_SWITCH_CONTEXT)(BL_ARCH_MODE);
BLP_ARCH_SWITCH_CONTEXT BlpArchSwitchContext;

HOOK HookBgDisplayBackgroundImage;

EFI_STATUS EFIAPI BgDisplayBackgroundImageDetour() {
	return EFI_SUCCESS;
}

typedef EFI_STATUS(EFIAPI* BL_GET_BOOT_OPTION_BOOLEAN)(__int64, __int64, PBYTE);
HOOK HookBlGetBootOptionBoolean;

EFI_STATUS EFIAPI BlGetBootOptionBooleanDetour(__int64 a1, __int64 key, PBYTE outResult) {
	HkInlineUnhook(&HookBlGetBootOptionBoolean);

	EFI_STATUS status = CALL_CAST(HookBlGetBootOptionBoolean, BL_GET_BOOT_OPTION_BOOLEAN, a1, key, outResult);

	HkInlineHook(HookBlGetBootOptionBoolean.Address, &BlGetBootOptionBooleanDetour, &HookBlGetBootOptionBoolean);
	if (key == 0x16000069) { // Boot animation progress circle
		*outResult = TRUE;
		return 1; // Disables only if result > 0
	}

	return status;
}

typedef EFI_STATUS(EFIAPI* OSL_FWP_KERNEL_SETUP_PHASE1)(PVOID);
HOOK HookOslFwpKernelSetupPhase1;

typedef struct _UNICODE_STRING {
	USHORT Length;
	USHORT MaximumLength;
	CHAR16* Buffer;
} UNICODE_STRING;
typedef UNICODE_STRING* PUNICODE_STRING;

typedef void* HANDLE;

typedef struct _KLDR_DATA_TABLE_ENTRY {
	struct _LIST_ENTRY InLoadOrderLinks;
	VOID* ExceptionTable;
	UINT32 ExceptionTableSize;
	VOID* GpValue;
	struct _NON_PAGED_DEBUG_INFO* NonPagedDebugInfo;
	VOID* DllBase;
	VOID* EntryPoint;
	UINT32 SizeOfImage;
	struct _UNICODE_STRING FullDllName;
	struct _UNICODE_STRING BaseDllName;
	UINT32 Flags;
	UINT16 LoadCount;
	union {
		UINT16 SignatureLevel : 4;
		UINT16 SignatureType : 3;
		UINT16 Unused : 9;
		UINT16 EntireField;
	} u1;
	VOID* SectionPointer;
	UINT32 CheckSum;
	UINT32 CoverageSectionSize;
	VOID* CoverageSection;
	VOID* LoadedImports;
	VOID* Spare;
	UINT32 SizeOfImageNotRounded;
	UINT32 TimeDateStamp;
} KLDR_DATA_TABLE_ENTRY, *PKLDR_DATA_TABLE_ENTRY;

typedef struct _LOADER_PARAMETER_BLOCK {
	UINT32 OsMajorVersion;
	UINT32 OsMinorVersion;
	UINT32 Size;
	UINT32 OsLoaderSecurityVersion;
	struct _LIST_ENTRY LoadOrderListHead;
	struct _LIST_ENTRY MemoryDescriptorListHead;
	struct _LIST_ENTRY BootDriverListHead;
	struct _LIST_ENTRY EarlyLaunchListHead;
	struct _LIST_ENTRY CoreDriverListHead;
	struct _LIST_ENTRY CoreExtensionsDriverListHead;
	struct _LIST_ENTRY TpmCoreDriverListHead;
} LOADER_PARAMETER_BLOCK, *PLOADER_PARAMETER_BLOCK;

#define CONTAINING_RECORD(address, type, field) ((type *)((char*)(address) - (UINT64)(&((type *)0)->field)))

KLDR_DATA_TABLE_ENTRY GetModule(LIST_ENTRY* list, const CHAR16* name) {
	for (LIST_ENTRY* entry = list->ForwardLink; entry != list; entry = entry->ForwardLink) {
		PKLDR_DATA_TABLE_ENTRY module = CONTAINING_RECORD(entry, KLDR_DATA_TABLE_ENTRY, InLoadOrderLinks);
		if (module && StrnCmp(name, module->BaseDllName.Buffer, module->BaseDllName.Length) == 0)
			return *module;
	}
}



EFI_STATUS EFIAPI OslFwpKernelSetupPhase1Detour(PLOADER_PARAMETER_BLOCK LoaderBlock) {
	HkInlineUnhook(&HookOslFwpKernelSetupPhase1);

	KLDR_DATA_TABLE_ENTRY ntoskrnlModule = GetModule(&LoaderBlock->LoadOrderListHead, L"ntoskrnl.exe");

	BlpArchSwitchContext(BlRealMode);
	LogPrint(gWinloadLogger, L"ntoskrnl.exe: %p", ntoskrnlModule.DllBase);
	BlUpdateProgress();
	BlRenderAndUpdateProgress(4000);
	BlpArchSwitchContext(BlProtectedMode);

	return CALL_CAST(HookOslFwpKernelSetupPhase1, OSL_FWP_KERNEL_SETUP_PHASE1, LoaderBlock);
}

typedef EFI_STATUS(EFIAPI* BL_IMG_ALLOCATE_IMAGE_BUFFER)(PVOID*, SIZE_T, UINT, UINT, PVOID, UINT);
HOOK HookBlImgAllocateImageBuffer;

#define BL_MEMORY_TYPE_APPLICATION 0xE0000012

EFI_STATUS EFIAPI BlImgAllocateImageBufferDetour(PVOID* ImageBuffer, SIZE_T ImageSize, UINT MemoryType, UINT Attributes, PVOID Unused, UINT Flags) {
	HkInlineUnhook(&HookBlImgAllocateImageBuffer);

	EFI_STATUS status = CALL_CAST(HookBlImgAllocateImageBuffer, BL_IMG_ALLOCATE_IMAGE_BUFFER, ImageBuffer, ImageSize, MemoryType, Attributes, Unused, Flags);
	if (MemoryType == BL_MEMORY_TYPE_APPLICATION)
		return status;

	BlpArchSwitchContext(BlRealMode);
	BlUpdateProgress();
	BlRenderAndUpdateProgress(3000);
	BlpArchSwitchContext(BlProtectedMode);

	HkInlineHook(HookBlImgAllocateImageBuffer.Address, &BlImgAllocateImageBufferDetour, &HookBlImgAllocateImageBuffer);
	return status;
}

EFI_STATUS HkSetupWinloadHooks(IN PVOID Base, IN UINT Size) {
	BlpArchSwitchContext = (BLP_ARCH_SWITCH_CONTEXT) SigFindSignature(Base, Size, "40 53 48 83 EC ? 48 8B 15");

	PVOID bgdbi = SigFindSignature(Base, Size, "40 53 48 83 EC ? 33 DB F6 05");
	HkInlineHook(bgdbi, &BgDisplayBackgroundImageDetour, &HookBgDisplayBackgroundImage);
	LogPrint(gHooksLogger, L"BgDisplayBackgroundImage: 0x%lX", bgdbi);

	PVOID bgbob = SigFindSignature(Base, Size, "40 53 48 83 EC ? 8B C2");
	HkInlineHook(bgbob, &BlGetBootOptionBooleanDetour, &HookBlGetBootOptionBoolean);
	LogPrint(gHooksLogger, L"BlGetBootOptionBoolean: 0x%lX", bgbob);

	PVOID ofksp1 = SigFindSignature(Base, Size, "48 89 4C 24 ? 55 53 56 57 41 54 41 55 41 56 41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 48 8B F1");
	HkInlineHook(ofksp1, &OslFwpKernelSetupPhase1Detour, &HookOslFwpKernelSetupPhase1);
	LogPrint(gHooksLogger, L"OslFwpKernelSetupPhase1: 0x%lX", ofksp1);

	PVOID biaib = SigFindSignature(Base, Size, "48 89 5C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 55 41 54 41 55 41 56 41 57 48 8B EC 48 83 EC ? 48 8B 31");
	HkInlineHook(biaib, &BlImgAllocateImageBufferDetour, &HookBlImgAllocateImageBuffer);
	LogPrint(gHooksLogger, L"BlImgAllocateImageBuffer: 0x%lX", biaib);

	return EFI_SUCCESS;
}
