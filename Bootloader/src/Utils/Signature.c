#include "Signature.h"

#include <Library/BaseLib.h> // AsciiStrLen
#include "../Memory/Allocator.h"

#define HEX_CHAR_TO_BYTE(character) (character >= '0' && character <= '9' ? character - '0' : character >= 'A' && character <= 'F' ? character - 'A' + 10 : character >= 'a' && character <= 'f' ? character - 'a' + 10 : 0)
#define GET_BYTE(a, b) (HEX_CHAR_TO_BYTE(a) << 4 | HEX_CHAR_TO_BYTE(b))

PVOID SigFindSignature(IN PVOID Base, IN SIZE_T Size, IN const CHAR8* Signature) {
    SIZE_T patternLength = AsciiStrLen(Signature);

	SIZE_T cursor = 0;
	BYTE* bytes = NULL;
	ExAllocate(patternLength * 2, &bytes);

	for (SIZE_T i = 0; i < patternLength; i++) {
		if (Signature[i] == '\?') {
			bytes[cursor++] = 0xFF;
			if (patternLength > i + 1 && Signature[i + 1] == '\?')
				i++;

			i++;
			continue;
		}

		bytes[cursor++] = GET_BYTE(Signature[i], Signature[i + 1]);
		i += 2;
	}

	QWORD start = (QWORD) Base;
	QWORD end = start + Size;

	for (QWORD address = start; address < end; address++) {
		for (SIZE_T i = 0; i < cursor; i++) {
			BYTE patternByte = bytes[i];
			BYTE moduleByte = *(BYTE*) (address + i);
			if (patternByte == 0xFF) {
				i++;
				continue;
			}

			if (patternByte != moduleByte)
				break;

			if (i == cursor - 1)
				return (PVOID) address;
		}
	}

	return NULL;
}
