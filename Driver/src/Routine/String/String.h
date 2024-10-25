#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#pragma once

#include "../Allocator/Allocator.h"
#include <ntstrsafe.h>

class String {
public:
    String() = default;
    explicit String(IN PCSTR cstring) {
        ansiString = new ANSI_STRING;
        RtlInitAnsiString(ansiString, cstring);
    }
    explicit String(IN PUNICODE_STRING unicodeString) : ansiString(UnicodeToAnsi(unicodeString)) {}
    explicit String(IN PANSI_STRING ansiString) : ansiString(ansiString) {}
public:
    template<typename... Args>
    static String Format(IN PCSTR text, IN Args... args) {
        auto buffer = new char[1024];
        RtlStringCbPrintfA(buffer, 1024, text, args...);

        return String(buffer);
    }
public:
    static PANSI_STRING UnicodeToAnsi(IN PUNICODE_STRING unicodeString) {
        auto ansiStr = new ANSI_STRING;
        ansiStr->Length = unicodeString->Length;
        ansiStr->MaximumLength = unicodeString->MaximumLength;

        RtlUnicodeStringToAnsiString(ansiStr, unicodeString, TRUE);
        return ansiStr;
    }

    static PUNICODE_STRING AnsiToUnicode(IN PANSI_STRING ansiString) {
        auto unicodeStr = new UNICODE_STRING;
        unicodeStr->Length = ansiString->Length;
        unicodeStr->MaximumLength = ansiString->MaximumLength;

        RtlAnsiStringToUnicodeString(unicodeStr, ansiString, TRUE);
        return unicodeStr;
    }
public:
    PANSI_STRING AnsiString() const {
        return ansiString;
    }

    PUNICODE_STRING UnicodeString() const {
        return AnsiToUnicode(ansiString);
    }

    PCSTR CString() const {
        return ansiString->Buffer;
    }

    SIZE_T Length() const {
        PCSTR cstring = ansiString->Buffer;
        SIZE_T length;
        for (length = 0; *cstring != '\0'; cstring++)
            length++;

        return length;
    }
private:
    PANSI_STRING ansiString;
};

#pragma clang diagnostic pop