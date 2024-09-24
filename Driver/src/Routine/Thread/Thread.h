#pragma once

#include <ntifs.h>

struct Thread {
    HANDLE handle;
};

typedef void(*THREAD_START_ROUTINE)();

static THREAD_START_ROUTINE start;

NTSTATUS CreateThread(THREAD_START_ROUTINE startRoutine, Thread thread) {
    start = startRoutine;
    NTSTATUS status = PsCreateSystemThread(&thread.handle, THREAD_ALL_ACCESS, nullptr, nullptr, nullptr, [](PVOID context) {
        start();
    }, nullptr);

    return status;
}

NTSTATUS CloseThread(Thread thread) {
    return ZwClose(thread.handle);
}
