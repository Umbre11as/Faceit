#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#pragma once

#include "../Allocator/Allocator.h"
#include <ntstrsafe.h>

class String {
public:
    String() = default;
    explicit String(PCSTR cstring) {
        ansiString = new ANSI_STRING;
        RtlInitAnsiString(ansiString, cstring);
    }
    explicit String(PUNICODE_STRING unicodeString) : ansiString(UnicodeToAnsi(unicodeString)) {}
    explicit String(PANSI_STRING ansiString) : ansiString(ansiString) {}
public:
    static PANSI_STRING UnicodeToAnsi(PUNICODE_STRING unicodeString) {
        auto ansiStr = new ANSI_STRING;
        ansiStr->Length = unicodeString->Length;
        ansiStr->MaximumLength = unicodeString->MaximumLength;

        RtlUnicodeStringToAnsiString(ansiStr, unicodeString, TRUE);
        return ansiStr;
    }

    static PUNICODE_STRING AnsiToUnicode(PANSI_STRING ansiString) {
        auto unicodeStr = new UNICODE_STRING;
        unicodeStr->Length = ansiString->Length;
        unicodeStr->MaximumLength = ansiString->MaximumLength;

        RtlAnsiStringToUnicodeString(unicodeStr, ansiString, FALSE);
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
private:
    PANSI_STRING ansiString;
};

#pragma clang diagnostic pop