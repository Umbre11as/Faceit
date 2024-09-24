#pragma clang diagnostic push
#pragma ide diagnostic ignored "bugprone-reserved-identifier"
#pragma clang diagnostic ignored "-Wmicrosoft-cast"

#include "Routine/Routine.h"

NTSTATUS DriverEntry(IN PDRIVER_OBJECT, IN PUNICODE_STRING) {
    PVOID ntoskrnlBase = Modules::GetSystemModuleBase(NTOSKRNL_PATH);
    Log("ntoskrnl.exe: %p\n", ntoskrnlBase);

    PVOID ntCompareSigningLevels = Modules::GetExport(ntoskrnlBase, "NtCompareSigningLevels");
    Log("NtCompareSigningLevels: %p\n", ntCompareSigningLevels);

    //Utils::Sleep(1000);
    Utils::LogToFile("2 + 2 = %d\n", 4);

    return STATUS_SUCCESS;
}

#pragma clang diagnostic pop