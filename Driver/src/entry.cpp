#pragma clang diagnostic push
#pragma ide diagnostic ignored "bugprone-reserved-identifier"
#pragma clang diagnostic ignored "-Wmicrosoft-cast"

#include "Routine/Routine.h"

#define Log(format, ...) DbgPrintEx(0, 0, format, __VA_ARGS__)

NTSTATUS DriverEntry(IN PDRIVER_OBJECT, IN PUNICODE_STRING) {
    Log(String("Hello world").CString());

    return STATUS_SUCCESS;
}

#pragma clang diagnostic pop