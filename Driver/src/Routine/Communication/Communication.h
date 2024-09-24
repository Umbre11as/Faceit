#pragma once

#include "Pipes/FunctionPointerSwapPipe.h"
#include "Pipes/IoctlPipe.h"

namespace Communication {
    NTSTATUS Setup(CommunicationPipe* pipe, CommunicateRoutine routine) {
        NTSTATUS status = pipe->Install(routine);
        if (NT_SUCCESS(status))
            DbgPrintEx(0, 0, "Communication configured using %s pipe\n", pipe->ID());

        return status;
    }
}
