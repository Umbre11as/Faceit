#include "Utils/CommunicationUtils.h"
#include "Utils/ProcessUtils.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wwritable-strings"
#pragma clang diagnostic ignored "-Wformat"
int main() {
    if (!Communication::Setup())
        return 1;
    printf("Communication configured (Ptr swap)\n");

    DWORD pid = 0;
    if (!Utils::Process::FindProcess(L"cs2.exe", &pid)) {
        MessageBox(nullptr, "Cannot find process", "Error", MB_OK);
        return 1;
    }

    printf("Process ID: 0x%lX (%ld)\n", pid, pid);
    Utils::Communications::Attach(pid);
    printf("Attached\n");

    ULONGLONG clientAddress = Utils::Communications::GetModuleAddress("client.dll");
    printf("client.dll: %p\n", clientAddress);

    return 0;
}
#pragma clang diagnostic pop
