#pragma once

#include "../Types/Types.h"

SIZE_T SsStringLength(IN const CHAR16* String);

EFI_STATUS SsStringSplit(IN const CHAR16* String, IN const CHAR16 Character, OUT SIZE_T* OutSize, OUT CHAR16** Buffer);
