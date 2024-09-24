#include <Windows.h>
#include <cstdio>

typedef void(*NtCompareSigningLevelsFn)(PVOID buffer, SIZE_T size, ULONGLONG cookie);

int main() {
    HMODULE ntDllModule = LoadLibrary("ntdll.dll");
    if (!ntDllModule) {
        fprintf(stderr, "Cannot open ntdll.dll\n");
        return 1;
    }

    auto NtCompareSigningLevels = reinterpret_cast<NtCompareSigningLevelsFn>(GetProcAddress(ntDllModule, "NtCompareSigningLevels"));
    if (!NtCompareSigningLevels) {
        fprintf(stderr, "Cannot find NtCompareSigningLevels\n");
        return 1;
    }

    const char* string = "hello world\n";
    NtCompareSigningLevels(const_cast<char*>(string), strlen(string) + 1, 0xABCCC2F);

    return 0;
}
