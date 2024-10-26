#include "Winload.h"

#include <Library/BaseLib.h> // StrnCmp

#include "../../Utils/Utils.h"
#include "../../FileSystem/FileSystem.h"
#include "../../Utils/PEUtils.h"

#include "../Graphics/BLGraphics.h"
#include "../Mapper/Mapper.h"

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

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreturn-type"
KLDR_DATA_TABLE_ENTRY GetModule(LIST_ENTRY* list, const CHAR16* name) {
	for (LIST_ENTRY* entry = list->ForwardLink; entry != list; entry = entry->ForwardLink) {
		PKLDR_DATA_TABLE_ENTRY module = CONTAINING_RECORD(entry, KLDR_DATA_TABLE_ENTRY, InLoadOrderLinks);
		if (module && StrnCmp(name, module->BaseDllName.Buffer, module->BaseDllName.Length) == 0)
			return *module;
	}
}
#pragma clang diagnostic pop

PVOID mapBuffer = NULL;

PVOID driverData;
ULONG driverImageSize = 0;

EFI_STATUS ReadDriver() {
    EFI_STATUS status = EFI_SUCCESS;
    SIZE_T count = 0;
    PVOLUME volumes;
    if (EFI_ERROR(status = FsListVolumes(&volumes, &count))) {
        LogPrint(gWinloadLogger, L"An error while getting volumes: 0x%lX (%d)", status, status);
        return status;
    }

    SIZE_T fileSize = 0;
    for (SIZE_T i = 0; i < count; i++) {
        VOLUME volume = volumes[i];

        if (EFI_ERROR(status = FsOpenVolume(&volume)))
            continue;

        if (!EFI_ERROR(FsReadFile(volume, L"\\Insane\\Driver\\Driver.sys", &fileSize, driverData))) {
            LogPrint(gWinloadLogger, L"Read driver at: 0x%lX (%lX%lX)", &driverData, ((PBYTE) driverData)[0], ((PBYTE) driverData)[1]);
            FsCloseVolume(volume);
            break;
        }

        if (EFI_ERROR(status = FsCloseVolume(volume)))
            continue;
    }

    return status;
}

EFI_STATUS EFIAPI OslFwpKernelSetupPhase1Detour(PLOADER_PARAMETER_BLOCK LoaderBlock) {
	HkInlineUnhook(&HookOslFwpKernelSetupPhase1);

	KLDR_DATA_TABLE_ENTRY ntoskrnlModule = GetModule(&LoaderBlock->LoadOrderListHead, L"ntoskrnl.exe");

    PVOID entryPoint;
    MpMapAndResolveDriver(driverData, mapBuffer, ntoskrnlModule.DllBase, &entryPoint);

	BlpArchSwitchContext(BlRealMode);
	LogPrint(gWinloadLogger, L"ntoskrnl.exe: %p", ntoskrnlModule.DllBase);
    LogPrint(gWinloadLogger, L"EntryPoint: %p", entryPoint);
	BlUpdateProgress();
	BlRenderAndUpdateProgress(4000);
	BlpArchSwitchContext(BlProtectedMode);

	return CALL_CAST(HookOslFwpKernelSetupPhase1, OSL_FWP_KERNEL_SETUP_PHASE1, LoaderBlock);
}

typedef EFI_STATUS(EFIAPI* BL_IMG_ALLOCATE_IMAGE_BUFFER)(PVOID*, SIZE_T, UINT, UINT, PVOID, UINT);
HOOK HookBlImgAllocateImageBuffer;

#define BL_MEMORY_TYPE_APPLICATION 0xE0000012
#define BL_MEMORY_ATTRIBUTE_RWX 0x424000

EFI_STATUS EFIAPI BlImgAllocateImageBufferDetour(PVOID* ImageBuffer, SIZE_T ImageSize, UINT MemoryType, UINT Attributes, PVOID Unused, UINT Flags) {
	HkInlineUnhook(&HookBlImgAllocateImageBuffer);

	EFI_STATUS status = CALL_CAST(HookBlImgAllocateImageBuffer, BL_IMG_ALLOCATE_IMAGE_BUFFER, ImageBuffer, ImageSize, MemoryType, Attributes, Unused, Flags);
    if (!EFI_ERROR(status) && MemoryType == BL_MEMORY_TYPE_APPLICATION) {
        EFI_STATUS driverAllocateStatus = CALL_CAST(HookBlImgAllocateImageBuffer, BL_IMG_ALLOCATE_IMAGE_BUFFER, &mapBuffer, driverImageSize, MemoryType, BL_MEMORY_ATTRIBUTE_RWX, Unused, 0);
        if (EFI_ERROR(driverAllocateStatus))
            mapBuffer = NULL;

        return status;
    }

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

    ReadDriver();
    driverImageSize = PeFetchImageSize(driverData);
    LogPrint(gWinloadLogger, L"Driver image size: 0x%lX (%ld)", driverImageSize, driverImageSize);

	return EFI_SUCCESS;
}
