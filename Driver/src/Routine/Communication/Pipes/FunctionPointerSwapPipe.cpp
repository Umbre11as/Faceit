#include "FunctionPointerSwapPipe.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmicrosoft-cast"
FunctionPointerSwapPipe::FunctionPointerSwapPipe() : CommunicationPipe("Ptr Swap") {}

CommunicateRoutine ptrSwapCommunicate;

void Detour(PVOID buffer, SIZE_T size, ULONGLONG cookie) {
    if (ExGetPreviousMode() == UserMode && cookie == 0xABCCC2F && buffer && size > 0)
        ptrSwapCommunicate(buffer, size);
}

NTSTATUS FunctionPointerSwapPipe::Install(CommunicateRoutine routine) {
    routine = ptrSwapCommunicate;

    PVOID ntoskrnlBase = Modules::GetSystemModuleBase(NTOSKRNL_PATH);
    if (!ntoskrnlBase)
        return STATUS_INVALID_ADDRESS;

    PVOID function = Modules::GetExport(ntoskrnlBase, "NtCompareSigningLevels");
    if (!function)
        return STATUS_NOT_FOUND;

    CaveHook(reinterpret_cast<ULONGLONG>(function), Detour, nullptr);
    return STATUS_SUCCESS;
}

#pragma clang diagnostic pop