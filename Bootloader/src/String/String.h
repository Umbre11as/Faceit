#pragma once

#include "../Types/Types.h"

SIZE_T SsStringLength(IN const CHAR16* String);

CHAR16** SsStringSplit(IN const CHAR16* String, IN const CHAR16 Character, OUT SIZE_T* OutSize, OUT CHAR16** Buffer);

CHAR16* SsAsciiToUnicode(IN const CHAR8* AsciiString);
