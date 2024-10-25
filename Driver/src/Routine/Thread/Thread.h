#pragma once

#include <ntifs.h>

struct Thread {
    HANDLE handle;
};

typedef void(*THREAD_START_ROUTINE)();

static THREAD_START_ROUTINE start;

NTSTATUS CreateThread(IN THREAD_START_ROUTINE startRoutine, IN Thread thread) {
    start = startRoutine;
    NTSTATUS status = PsCreateSystemThread(&thread.handle, THREAD_ALL_ACCESS, nullptr, nullptr, nullptr, [](PVOID context) {
        start();
    }, nullptr);

    return status;
}

NTSTATUS CloseThread(IN Thread thread) {
    return ZwClose(thread.handle);
}
