#ifdef INTERNAL_ENGINE_GUI_INTERFACE
#include "ImGUIDevice.h"
#include "GameObject.h"
#include "Location.h"
#include "Component.h"
#include "Model.h"
#include "Config.h"
#include "PrimitiveDrawable.h"
#include "ShadowMap.h"

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

    ImFont* font = io.Fonts->AddFontFromFileTTF("arlrdbd.ttf", m_fontSize);
    io.FontDefault = font;

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    DefaultStyle();
}

void ImGUIDevice::DefaultStyle(void) {
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    style.WindowPadding = ImVec2(8, 8);
    style.WindowRounding = 6.0f;
    style.FramePadding = ImVec2(4, 3);
    style.FrameRounding = 2.0f;
    style.ItemSpacing = ImVec2(8, 4);
    style.ItemInnerSpacing = ImVec2(4, 4);
    style.IndentSpacing = 21.0f;
    style.ScrollbarSize = 16.0f;
    style.ScrollbarRounding = 9.0f;
    style.GrabMinSize = 10.0f;
    style.GrabRounding = 2.0f;
    style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
    style.ButtonTextAlign = ImVec2(0.5f, 0.5f);

    colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
    colors[ImGuiCol_Border] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.07f, 0.07f, 0.07f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.70f, 0.70f, 0.70f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
    colors[ImGuiCol_Separator] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    colors[ImGuiCol_PlotLines] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.00f, 0.59f, 1.00f, 0.35f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
}

void ImGUIDevice::WhiteStyle(void) {
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    colors[ImGuiCol_Text] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.92f, 0.92f, 0.92f, 1.00f);
    colors[ImGuiCol_Border] = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.88f, 0.88f, 0.88f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.76f, 0.76f, 0.76f, 1.00f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.45f, 1.00f, 0.82f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.45f, 1.00f, 0.82f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.00f, 0.45f, 1.00f, 0.82f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.00f, 0.37f, 0.78f, 1.00f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.00f, 0.35f, 1.00f, 0.78f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.00f, 0.33f, 1.00f, 0.84f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.00f, 0.31f, 1.00f, 0.88f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.92f, 0.92f, 0.92f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.00f, 0.49f, 1.00f, 0.59f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.00f, 0.49f, 1.00f, 0.59f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.00f, 0.39f, 1.00f, 0.71f);
    colors[ImGuiCol_Button] = ImVec4(0.00f, 0.49f, 1.00f, 0.59f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.00f, 0.49f, 1.00f, 0.71f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.00f, 0.49f, 1.00f, 0.78f);
    colors[ImGuiCol_Header] = ImVec4(0.00f, 0.49f, 1.00f, 0.78f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 0.49f, 1.00f, 0.71f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.00f, 0.49f, 1.00f, 0.78f);
    colors[ImGuiCol_Separator] = ImVec4(0.00f, 0.39f, 1.00f, 0.59f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.00f, 0.27f, 1.00f, 0.59f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.00f, 0.25f, 1.00f, 0.63f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.39f, 1.00f, 0.59f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.00f, 0.27f, 1.00f, 0.59f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.00f, 0.25f, 1.00f, 0.63f);
    colors[ImGuiCol_PlotLines] = ImVec4(0.00f, 0.39f, 1.00f, 0.75f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.00f, 0.39f, 1.00f, 0.75f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.00f, 0.39f, 1.00f, 0.75f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.00f, 0.35f, 0.92f, 0.78f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.00f, 0.47f, 1.00f, 0.59f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
}

ImGUIDevice::ImGUIDevice() { 
    m_styleSelectedState = false;
    m_selectedStyle = 0;
    m_fontSize = 14.0f;
}

void ImGUIDevice::Init(ID3D11Device* device, ID3D11DeviceContext* context) {
    InitWindowStyle();

    const WindowDescription* desc = engine.getWindowDesc();
    ImGui_ImplWin32_Init(desc->hWnd);
    ImGui_ImplDX11_Init(device, context);
}

static bool isDragging = false;
static GameObject* draggedObject = nullptr;

bool ImGUIDevice::DisplayHierarchy(GameObject* parent) {
    bool clickedOnElement = false;
    if (!parent) return clickedOnElement;

    ImGui::PushID(parent);
    bool isSelected = (engine.lastSelected == parent);

    if (isSelected) {
        ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.26f, 0.59f, 0.98f, 0.31f));
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.26f, 0.59f, 0.98f, 0.51f));
        ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.26f, 0.59f, 0.98f, 0.71f));
    }
    else ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.16f, 0.16f, 0.16f, 0.31f));

    GameObject* current = parent->FirstChild();
    bool nodeOpen = ImGui::TreeNodeEx(parent->name.c_str(), ImGuiTreeNodeFlags_OpenOnArrow |
        ImGuiTreeNodeFlags_Framed | (current == nullptr ? ImGuiTreeNodeFlags_Leaf : 0) |
        (isSelected ? ImGuiTreeNodeFlags_Selected : 0));
    if (!isSelected) ImGui::PopStyleColor(1);

    if (ImGui::IsItemClicked()) {
        clickedOnElement = true;
        if (engine.lastSelected) {
            engine.lastSelected->selectable = false;
            engine.lastSelected = nullptr;
        }
        parent->selectable = true;
        engine.lastSelected = parent;
    }

    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceNoDisableHover)) {
        isDragging = true;
        draggedObject = parent;
        ImGui::SetDragDropPayload("GAMEOBJECT", &parent, sizeof(GameObject*));
        ImGui::Text("Moving %s", parent->name.c_str());
        ImGui::EndDragDropSource();
    }

    bool dropedInParent = false;
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GAMEOBJECT")) {
            GameObject* droppedObject = *(GameObject**)payload->Data;
            if (droppedObject != parent->Parent()) {
                droppedObject->SetParent(parent);
                current = parent->FirstChild();
                dropedInParent = true;
            }
        }
        ImGui::EndDragDropTarget();
    }

    if (!dropedInParent && isDragging && ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
        if (ImGui::GetDragDropPayload() && draggedObject == parent) {
            isDragging = false;
            if (GameObject* ptr = parent->Parent()) {
                if (GameObject* data = *(GameObject**)ImGui::GetDragDropPayload()->Data) {
                    if (ptr != data) {
                        ptr->RemoveChild(parent);
                        parent->SetParent(nullptr);
                    }
                }
            }
            else if (!ImGui::BeginDragDropTarget()) {
                std::list<GameObject*>& list = engine.location()->staticObjects();
                auto itToMove = std::find(list.begin(), list.end(), draggedObject);

                if (itToMove != list.end()) {
                    ImVec2 mousePos = ImGui::GetMousePos();
                    ImVec2 windowPos = ImGui::GetWindowPos();
                    ImVec2 relativeMousePos = ImVec2(mousePos.x - windowPos.x, mousePos.y - windowPos.y);

                    auto itTarget = list.begin();
                    int iterator = 1;
                    while (itTarget != list.end()) {
                        GameObject* targetObject = *itTarget;
                        ImVec2 itemMin = ImGui::GetItemRectMin();

                        const float height = ImGui::GetItemRectMax().y - ImGui::GetItemRectMin().y;
                        const float offset = 10.0f;

                        if (relativeMousePos.y - (offset * iterator) < height * iterator) {
                            list.splice(itTarget, list, itToMove);
                            break;
                        }
                        ++itTarget;
                        ++iterator;
                    }
                    if (itTarget == list.end())
                        list.splice(list.end(), list, itToMove);
                }
            }
        }
    }

    if (nodeOpen) {
        while (current != nullptr) {
            if (!current->serialize) {
                current = current->m_next;
                continue;
            }
            clickedOnElement = DisplayHierarchy(current);
            current = current->m_next;
        }
        ImGui::TreePop();
    }

    if (isSelected) ImGui::PopStyleColor(3);
    ImGui::PopID();
    return clickedOnElement;
}

void ImGUIDevice::Render() {
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

    ImGui::Begin("Inspector");
    {
        if (engine.lastSelected) {
            GameObject* gameObject = engine.lastSelected;
            ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
            ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.1f, 0.1f, 0.1f, 0.25f));
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
    
            float availableWidth = ImGui::GetContentRegionAvail().x;
            float width = availableWidth - ImGui::GetStyle().ItemSpacing.x;

            if (ImGui::BeginChild("ObjectEditorFrame", ImVec2(width, 75.0f), true)) {
                bool state = gameObject->IsEnabled();
                if (ImGui::Checkbox("##ToggleObject", &state))
                    gameObject->Enable(state);
                ImGui::SameLine();
    
                static char buffer[128];
                std::strncpy(buffer, gameObject->name.c_str(), sizeof(buffer));
                ImGui::SameLine();
                ImGui::SetNextItemWidth(width - ImGui::GetFrameHeight() * 6);
                if (ImGui::InputText("##ObjectName", buffer, IM_ARRAYSIZE(buffer)))
                    gameObject->name = buffer;

                ImGui::SameLine();
                bool st = gameObject->IsStatic();
                if (ImGui::Checkbox("##ToggleStatic", &st))
                    gameObject->Static(st);
                ImGui::SameLine();
                ImGui::Text("Static");

                const char* tags[] = { "Default Tag" };
                const char* layers[] = { "Default Layer" };
    
                const char* labelTag = "Tag:";
                const char* labelLayer = "Layer:";
                ImVec2 labelTagSize = ImGui::CalcTextSize(labelTag);
                ImVec2 labelLayerSize = ImGui::CalcTextSize(labelLayer);
    
                float availableWidth = ImGui::GetContentRegionAvail().x;
                float comboWidth = (availableWidth - labelTagSize.x - labelLayerSize.x - ImGui::GetStyle().ItemSpacing.x * 3) / 2;
    
                ImGui::Dummy(ImVec2(0.0f, 2.0f));
                ImGui::Text("Tag:");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(comboWidth);
                if (ImGui::Combo("##Tag", &gameObject->selectedTag, tags, IM_ARRAYSIZE(tags))) {
    
                }
    
                ImGui::SameLine();
                ImGui::Text("Layer:");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(comboWidth);
                if (ImGui::Combo("##Layer", &gameObject->selectedLayer, layers, IM_ARRAYSIZE(layers))) {
    
                }
            }
            ImGui::EndChild();
    
            ImGui::PopStyleColor(2);
            ImGui::PopStyleVar();
            for (AbstractBaseComponent* component : gameObject->getComponents())
                component->UpdateInterfaceInInspector(gameObject);
        }
    
        ImGui::End();
    }
    
    ImGui::Begin("Hierarchy");
    {
        bool clickedOnElement = false;
        for (GameObject* entity : engine.location()->staticObjects()) {
            if (!entity->Parent())
                clickedOnElement |= DisplayHierarchy(entity);
        }
    
        if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(0) && !clickedOnElement) {
            if (engine.lastSelected) {
                engine.lastSelected->selectable = false;
                engine.lastSelected = nullptr;
            }
        }
    
        ImGui::End();
    }
    
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Open..", "Ctrl+O")) {}
            if (ImGui::MenuItem("Save", "Ctrl+S")) {}
            if (ImGui::MenuItem("Close", "Ctrl+W")) {}
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit")) {
            if (ImGui::MenuItem("Style Selector"))
                m_styleSelectedState = true;
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
    
    if (m_styleSelectedState) {
        ImGui::Begin("Styles");
        {
            ImGui::BeginGroup();
            {
                const char* styles[] = { "Default", "White" };
                if (ImGui::Combo("Select Style", &m_selectedStyle, styles, IM_ARRAYSIZE(styles))) {
                    switch (m_selectedStyle) {
                    case 0: DefaultStyle(); break;
                    case 1: WhiteStyle(); break;
                    }
                }
            }
            ImGui::EndGroup();
            static float fontSize = 0;
            ImGui::BeginGroup();
            {
                ImGui::SliderFloat("Font Size", &fontSize, 15.0f, 32.0f);
            }
            ImGui::EndGroup();
            ImGui::BeginGroup();
            {
                static bool isRaycast = config.debugRaycast;
                if (ImGui::Checkbox("Debug Raycast?", &isRaycast)) {
                    engine.setDebugRaycast(isRaycast);
                    if (!isRaycast)
                        gizmozRect.GizmosClear();
                }
            }
            ImGui::EndGroup();
            if (ImGui::Button("Ok")) {
                ImGui::OpenPopup("Are you sure?");
            }
            if (ImGui::BeginPopupModal("Are you sure?", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
                ImGui::Text("Are you sure you want to save your settings?");
    
                if (ImGui::Button("OK")) {
                    m_fontSize = fontSize;
                    ImGui::CloseCurrentPopup();
                    m_styleSelectedState = false;
                }
                ImGui::SameLine();
                if (ImGui::Button("Cancel")) {
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Close")) {
                m_styleSelectedState = false;
            }
        }
        ImGui::End();
    }
    
    ImGui::Begin("Project");
    {
    
    }
    ImGui::End();
    
    ImGui::Begin("Assets");
    {
    
    }
    ImGui::End();

    ImGui::Begin("Console");
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