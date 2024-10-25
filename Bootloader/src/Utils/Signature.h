#pragma once

#include "../Types/Types.h"

PVOID SigFindSignature(IN PVOID Base, IN SIZE_T Size, IN const CHAR8* Signature);
