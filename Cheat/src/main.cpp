#include "Utils/CommunicationUtils.h"
#include "Utils/ProcessUtils.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wwritable-strings"
#pragma clang diagnostic ignored "-Wformat"

using Utils::Communications::Read;

struct C_BaseEntity {

};

template<typename T = C_BaseEntity>
T* GetEntity(ULONGLONG client, int index) {
    static auto entityList = Read<ULONGLONG>(client + 27065416);
    auto entry = Read<ULONGLONG>(entityList + 0x8 * ((index & 0x7FFF) >> 9) + 16);

    return Read<T*>(entry + 120 * (index & 0x1FF));
}

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
    if (!clientAddress) {
        MessageBox(nullptr, "Cannot find client.dll", "Error", MB_OK);
        return 1;
    }

    auto localPlayerPawn = Read<C_BaseEntity*>(clientAddress + 25381656);
    printf("local pawn: %p\n", localPlayerPawn);
    if (!localPlayerPawn)
        return 1;

    for (int i = 0; i < 64; i++) {
        C_BaseEntity* controller = GetEntity(clientAddress, i);
        printf("%p\n", controller);


    }

    return 0;
}
#pragma clang diagnostic pop
