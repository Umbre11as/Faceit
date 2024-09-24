#pragma once

#include <ntifs.h>

#define Log(format, ...) DbgPrintEx(0, 0, format, __VA_ARGS__)

namespace Utils {
    // Credits @ https://github.com/vRare/AutoSpitta-x64/blob/master/hacks.c#L9
    NTSTATUS Sleep(ULONGLONG milliseconds) {
        LARGE_INTEGER delay;
        ULONG* split;

        milliseconds *= 1000000;
        milliseconds /= 100;
        milliseconds = -milliseconds;
        split = reinterpret_cast<ULONG*>(&milliseconds);

        delay.LowPart = *split;
        split++;
        delay.HighPart = static_cast<LONG>(*split);

        return KeDelayExecutionThread(KernelMode, false, &delay);
    }

    template<typename... Args>
    void LogToFile(PCSTR text, Args... args) {
        String formatted = String::Format(text, args...);

        HANDLE fileHandle;
        OBJECT_ATTRIBUTES objectAttributes;
        IO_STATUS_BLOCK ioStatusBlock;

        String filePath = String(R"(\DosDevices\C:\Users\admin\Desktop\logging.txt)");

        InitializeObjectAttributes(&objectAttributes, filePath.UnicodeString(), OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, nullptr, nullptr)

#pragma clang diagnostic push
#pragma ide diagnostic ignored "Simplify"
        ZwCreateFile(&fileHandle, FILE_GENERIC_READ | FILE_GENERIC_WRITE, &objectAttributes, &ioStatusBlock, nullptr, FILE_ATTRIBUTE_NORMAL, 0, FILE_OPEN, FILE_SYNCHRONOUS_IO_NONALERT, nullptr, 0);
#pragma clang diagnostic pop

        FILE_STANDARD_INFORMATION fileInfo{};
        ZwQueryInformationFile(fileHandle, &ioStatusBlock, &fileInfo, sizeof(fileInfo), FileStandardInformation);
        LONGLONG length = fileInfo.EndOfFile.QuadPart;
        auto exitingBuffer = new char[length];
        if (!NT_SUCCESS(ZwReadFile(fileHandle, nullptr, nullptr, nullptr, &ioStatusBlock, exitingBuffer, length, nullptr, nullptr)))
            length = 0;
        exitingBuffer[length] = '\0';

        UNICODE_STRING concatString;
        concatString.MaximumLength = static_cast<USHORT>(length + formatted.Length());
        concatString.Length = 0;
        concatString.Buffer = new WCHAR[concatString.MaximumLength];
        RtlCopyUnicodeString(&concatString, formatted.UnicodeString());
        RtlAppendUnicodeStringToString(&concatString, String(exitingBuffer).UnicodeString());

        PANSI_STRING newAnsiString = String::UnicodeToAnsi(&concatString);
        ZwWriteFile(fileHandle, nullptr, nullptr, nullptr, &ioStatusBlock, newAnsiString->Buffer, newAnsiString->Length, nullptr, nullptr);

        ZwClose(fileHandle);
    }
}
