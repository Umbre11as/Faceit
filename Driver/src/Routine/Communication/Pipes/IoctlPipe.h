#pragma once

#include "../CommunicationPipe.h"
#include <ntifs.h>

class IoctlPipe : public CommunicationPipe {
public:
    explicit IoctlPipe(PDRIVER_OBJECT driverObject);
public:
    NTSTATUS Install(CommunicateRoutine routine) override;
private:
    PDRIVER_OBJECT driverObject;
};
