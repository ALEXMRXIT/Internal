#ifdef INTERNAL_ENGINE_GUI_INTERFACE
#include "ImGUIDevice.h"
#include "GameObject.h"
#include "Location.h"

void ImGUIDevice::InitWindowStyle(void) {
#ifdef _DEBUG
    IMGUI_CHECKVERSION();
#endif
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui::StyleColorsDark();

    ImGuiViewport* viewport = ImGui::GetMainViewport();
    if (viewport) {
        viewport->Size = ImVec2((float)engine.getSupportedResolution().Width, 
            (float)engine.getSupportedResolution().Height);
        viewport->Pos = ImVec2(0, 0);
    }
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
    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

    ImGui::Begin("Inspector");
    {
        if (ImGui::CollapsingHeader("Transform")) {
            static int selectedButton = 1;
            static int last_selected = 1;
            ImGui::BeginDisabled(!static_cast<bool>(engine.lastSelected));
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
            ImGui::EndDisabled();

            selectedButton = last_selected;
        }

        ImGui::End();
    }

    ImGui::Begin("Hierarchy");
    for (const GameObject* entity : engine.location()->staticObjects()) {
        ImGui::PushID(entity);
        ImGui::CollapsingHeader(entity->name.c_str());
        ImGui::PopID();
    }
    ImGui::End();

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Open..", "Ctrl+O")) {}
            if (ImGui::MenuItem("Save", "Ctrl+S")) {}
            if (ImGui::MenuItem("Close", "Ctrl+W")) {}
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit")) {
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    ImGui::Begin("Project");
    {

    }
    ImGui::End();

    ImGui::Begin("Assets");
    {

    }
    ImGui::End();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
    ImGui::Begin("Scene");
    {
        engine.setRaycast(ImGui::IsWindowHovered());
        ImVec2 pos = ImGui::GetWindowPos();
        ImVec2 size = ImGui::GetWindowSize();
        engine.setMousePosition(XMFLOAT2(pos.x, pos.y), XMFLOAT2(size.x, size.y));
        ImGui::Image(
            (ImTextureID)engine.GetRenderTextureSRV(),
            ImGui::GetContentRegionAvail(),
            ImVec2(0, -1), ImVec2(1, 0)
        );
    }
    ImGui::End();
    ImGui::PopStyleVar();

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void ImGUIDevice::Release() {
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}
#endif