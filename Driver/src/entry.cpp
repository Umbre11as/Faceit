#pragma clang diagnostic push
#pragma ide diagnostic ignored "bugprone-reserved-identifier"
#pragma clang diagnostic ignored "-Wmicrosoft-cast"

#include "Routine/Routine.h"

NTSTATUS DriverEntry(IN PDRIVER_OBJECT, IN PUNICODE_STRING) {


    return STATUS_SUCCESS;
}

#pragma clang diagnostic pop