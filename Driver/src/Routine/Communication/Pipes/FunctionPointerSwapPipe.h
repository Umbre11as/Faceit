#pragma once

#include "../CommunicationPipe.h"
#include "../../Utils/Modules.h"
#include <CaveHook.h>

class FunctionPointerSwapPipe : public CommunicationPipe {
public:
    FunctionPointerSwapPipe();
public:
    NTSTATUS Install(CommunicateRoutine routine) override;
};
