#include <dwmapi.h>
#include "Utils/CommunicationUtils.h"
#include "Utils/ProcessUtils.h"
#include <d3d11.h>
#include <imgui.h>
#include <backends/imgui_impl_dx11.h>
#include <backends/imgui_impl_win32.h>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wwritable-strings"
#pragma clang diagnostic ignored "-Wformat"

using Utils::Communications::Read;

struct C_BaseEntity {

};

enum Offsets : ULONGLONG {
    ENTITY_LIST = 27065416,
    LOCAL_PLAYER_PAWN = 25381656,
};

template<typename T = C_BaseEntity>
T* GetEntity(ULONGLONG client, int index) {
    static auto entityList = Read<ULONGLONG>(client + ENTITY_LIST);
    auto entry = Read<ULONGLONG>(entityList + 0x8 * ((index & 0x7FFF) >> 9) + 16);

    return Read<T*>(entry + 120 * (index & 0x1FF));
}

LONG gScreenWidth = 0, gScreenHeight = 0;

IDXGISwapChain* gSwapChain = nullptr;
ID3D11Device* gDevice = nullptr;
ID3D11DeviceContext* gContext = nullptr;
ID3D11RenderTargetView* gOverlayRenderTarget = nullptr;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI WndProc(HWND windowHandle, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(windowHandle, msg, wParam, lParam))
        return true;

    if (msg == WM_DESTROY)
        PostQuitMessage(0);

    return DefWindowProc(windowHandle, msg, wParam, lParam);
}

int main() {
    /*if (!Communication::Setup())
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

    auto localPlayerPawn = Read<C_BaseEntity*>(clientAddress + LOCAL_PLAYER_PAWN);
    printf("local pawn: %p\n", localPlayerPawn);
    if (!localPlayerPawn)
        return 1;*/

    RECT size{};
    HWND desktopWindowHandle = GetDesktopWindow();
    GetWindowRect(desktopWindowHandle, &size);

    gScreenWidth = size.right;
    gScreenHeight = size.bottom;

    WNDCLASSEX windowClass = {
            sizeof(WNDCLASSEX),
            CS_CLASSDC,
            WndProc,
            0L, 0L,
            GetModuleHandle(nullptr),
            nullptr, nullptr, nullptr, nullptr,
            "Lightshot",
            nullptr
    };
    RegisterClassEx(&windowClass);
    HWND windowHandle = CreateWindowEx(
            WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE,
            windowClass.lpszClassName, windowClass.lpszClassName,
            WS_POPUP,
            0, 0,
            gScreenWidth, gScreenHeight,
            nullptr, nullptr,
            windowClass.hInstance,
            nullptr
    );

    SetLayeredWindowAttributes(windowHandle, RGB(0, 0, 0), 255, LWA_ALPHA);
    MARGINS margin = { -1 };
    DwmExtendFrameIntoClientArea(windowHandle, &margin);

    DXGI_SWAP_CHAIN_DESC swapChainDesc{};
    swapChainDesc.BufferCount = 2;
    swapChainDesc.BufferDesc.Width = 0;
    swapChainDesc.BufferDesc.Height = 0;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.OutputWindow = windowHandle;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.Windowed = TRUE;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    D3D_FEATURE_LEVEL featureLevel;
    D3D_FEATURE_LEVEL featureLevels[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };

    HRESULT result = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, featureLevels, 2, D3D11_SDK_VERSION, &swapChainDesc, &gSwapChain, &gDevice, &featureLevel, &gContext);
    if (result == DXGI_ERROR_UNSUPPORTED)
        result = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, 0, featureLevels, 2, D3D11_SDK_VERSION, &swapChainDesc, &gSwapChain, &gDevice, &featureLevel, &gContext);
    if (result != S_OK)
        return 1;

    ID3D11Texture2D* framebufferTexture;
    gSwapChain->GetBuffer(0, IID_PPV_ARGS(&framebufferTexture));
    gDevice->CreateRenderTargetView(framebufferTexture, nullptr, &gOverlayRenderTarget);
    framebufferTexture->Release();

    ShowWindow(windowHandle, SW_SHOWDEFAULT);
    UpdateWindow(windowHandle);

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGui_ImplWin32_Init(windowHandle);
    ImGui_ImplDX11_Init(gDevice, gContext);

    float clearColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
    bool closeNeeded = false;
    while (!closeNeeded) {
        MSG msg;
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            if (msg.message == WM_QUIT) {
                closeNeeded = true;
                break;
            }
        }

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(0, 0), ImVec2(20, 20), ImColor(255, 0, 0));

        ImGui::Render();
        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }

        gContext->OMSetRenderTargets(1, &gOverlayRenderTarget, nullptr);
        gContext->ClearRenderTargetView(gOverlayRenderTarget, clearColor);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        gSwapChain->Present(1, 0);

        SetWindowLong(windowHandle, GWL_EXSTYLE, WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOOLWINDOW);
    }

    ImGui::DestroyContext();
    gContext->Release();
    gDevice->Release();
    return 0;
}
#pragma clang diagnostic pop
