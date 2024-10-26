#include "String.h"

#include <Uefi.h>
#include <Library/BaseLib.h>
#include "../Memory/Allocator.h"
#include "../Memory/Memory.h"

SIZE_T SsStringLength(IN const CHAR16* String) {
	return StrLen(String);
}

CHAR16** SsStringSplit(IN const CHAR16* String, IN const CHAR16 Character, OUT SIZE_T* OutSize, OUT CHAR16** Buffer) {
	EFI_STATUS status;
	const SIZE_T length = StrLen(String);
	if (length == 0)
		return NULL;

	CHAR16** splitten = NULL;
	status = ExAllocate(length, &splitten);
	if (EFI_ERROR(status))
		return NULL;

	SIZE_T lastIndex = 0;
	SIZE_T cursor = 0;
	for (SIZE_T i = 0; i < length; i++) {
		if (String[i] == Character || i == length - 1) {
			CHAR16* substring = NULL;
			status = ExAllocate(length * sizeof(CHAR16), &substring);
			if (EFI_ERROR(status)) {
				ExFree(splitten);
				return NULL;
			}

			BOOL isEnd = (i == length - 1);
			MmCopyMemory(substring, String + lastIndex, (i - lastIndex + (isEnd ? 1 : 0)) * sizeof(CHAR16));
			substring[i - lastIndex + 1] = L'\0';

			splitten[cursor++] = substring;
			lastIndex = i + 1;
		}
	}

	if (OutSize)
		*OutSize = cursor;
	return splitten;
}

CHAR16* SsAsciiToUnicode(IN const CHAR8* AsciiString) {
    SIZE_T length = AsciiStrLen(AsciiString);

    CHAR16* name = NULL;
    ExAllocate((length + 1) * sizeof(CHAR16), (PVOID*) &name);
    AsciiStrToUnicodeStr(AsciiString, name);

    return name;
}
