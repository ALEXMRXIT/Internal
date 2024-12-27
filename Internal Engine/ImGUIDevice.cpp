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

    static int selectedButton = 1;
    static int last_selected = 1;

    ImGui::Begin("Inspector");

    ImGui::BeginChild("ButtonBlock", ImVec2(0, 40), true);
    {
        float availableWidth = ImGui::GetContentRegionAvail().x;
        float buttonWidth = (availableWidth - ImGui::GetStyle().ItemSpacing.x) / 2;

        if (selectedButton == 1) {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.5f, 1.0f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.6f, 1.0f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.4f, 0.69f, 1.0f, 1.0f));
        }
        if (ImGui::Button("World", ImVec2(buttonWidth, 20))) {
            last_selected = 1;
        }
        if (selectedButton == 1) ImGui::PopStyleColor(3);

        ImGui::SameLine();

        if (selectedButton == 2) {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.5f, 1.0f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.6f, 1.0f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.4f, 0.69f, 1.0f, 1.0f));
        }
        if (ImGui::Button("Local", ImVec2(buttonWidth, 20))) {
            last_selected = 2;
        }
        if (selectedButton == 2) ImGui::PopStyleColor(3);
    }
    ImGui::EndChild();

    ImGui::End();

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    selectedButton = last_selected;
}

void ImGUIDevice::Release() {
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}
