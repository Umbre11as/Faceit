#pragma clang diagnostic push
#pragma ide diagnostic ignored "bugprone-reserved-identifier"
#pragma clang diagnostic ignored "-Wmicrosoft-cast"

#include "Routine/Routine.h"

void Communicate(PVOID buffer, SIZE_T size) {
    Log("Communicate: %p - 0x%lX\n", buffer, size);
    const char* string = reinterpret_cast<char*>(buffer);
    Log("%s\n", string);
}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT, IN PUNICODE_STRING) {
    NTSTATUS status = Communication::Setup(new FunctionPointerSwapPipe, Communicate);
    if (!NT_SUCCESS(status))
        Log("Cannot setup communication");

    return STATUS_SUCCESS;
}

#pragma clang diagnostic pop