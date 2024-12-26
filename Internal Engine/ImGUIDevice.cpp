#include "ImGUIDevice.h"

void ImGUIDevice::InitWindowStyle(void) {
#ifdef _DEBUG
    IMGUI_CHECKVERSION();
#endif
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    float dpiScale = ImGui_ImplWin32_GetDpiScaleForHwnd(engine.getWindowDesc()->hWnd);
    if (dpiScale > 1.0f) {
        ImGui::GetStyle().ScaleAllSizes(dpiScale);
        io.FontGlobalScale = dpiScale;
    }

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    ImGui::StyleColorsDark();
}

ImGUIDevice::ImGUIDevice() { }

void ImGUIDevice::Init(ID3D11Device* device, ID3D11DeviceContext* context) {
    InitWindowStyle();

    const WindowDescription* desc = engine.getWindowDesc();
    ImGui_ImplWin32_Init(desc->hWnd);
    ImGui_ImplDX11_Init(device, context);
}

void ImGUIDevice::Render() {
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Hello, world!");
    ImGui::End();

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void ImGUIDevice::Release() {
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}
