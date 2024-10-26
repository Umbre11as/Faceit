#pragma once

#include "../../Types/Types.h"

void MpMapAndResolveDriver(IN PVOID DriverBuffer, IN PVOID MapBuffer, IN PVOID NtoskrnlBase, OUT PVOID* EntryPoint);
