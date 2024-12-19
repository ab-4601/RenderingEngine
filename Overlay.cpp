#include "Overlay.h"
#include <imgui_internal.h>

ImGuiIO& Overlay::_init(GLFWwindow* window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 450");

    io.Fonts->AddFontDefault();
    mainfont = io.Fonts->AddFontFromFileTTF("Fonts/JetBrainsMono-SemiBold.ttf",
        18.5f, NULL);
    IM_ASSERT(this->mainfont != NULL);

    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowBorderSize = 1.f;
    style.WindowRounding = 8.f;
    style.Colors[ImGuiCol_Border] = ImVec4(0.9f, 0.41f, 0.f, 1.f);
    style.Colors[ImGuiCol_Text] = ImVec4(1.f, 0.9f, 0.9f, 1.f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.9f, 0.41f, 0.f, 1.f);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.1f, 0.1f, 0.5f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.05f, 0.05f, 0.05f, 1.f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.1f, 0.1f, 0.1f, 0.f);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.9f, 0.41f, 0.f, 1.f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.9f, 0.41f, 0.f, 1.f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.4f, 0.4f, 0.4f, 1.f);
    style.Colors[ImGuiCol_HeaderHovered] = style.Colors[ImGuiCol_FrameBgHovered];

    return io;
}

void Overlay::_newFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::GetMainViewport();
}

void Overlay::_updateTransformOperation(const Window& window) {
    if (window.getKeyPress(GLFW_KEY_1))
        transformOperation = ImGuizmo::TRANSLATE;

    else if (window.getKeyPress(GLFW_KEY_2)) 
        transformOperation = ImGuizmo::ROTATE;

    else if (window.getKeyPress(GLFW_KEY_3))
        transformOperation = ImGuizmo::SCALE;
}

void Overlay::DrawVec3Control(const std::string& label, float* values, float resetValue, float columnWidth) {
    ImGui::PushID(label.c_str());

    ImGui::Columns(2);
    ImGui::SetColumnWidth(0, columnWidth);
    ImGui::Text(label.c_str());

    ImGui::NextColumn();

    ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(2.f, 2.f));

    float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.f;
    ImVec2 buttonSize = { lineHeight + 3.f, lineHeight };

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.1f, 0.15f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.2f, 0.2f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.8f, 0.1f, 0.15f, 1.f));
    if (ImGui::Button("X", buttonSize))
        values[0] = resetValue;
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    ImGui::DragFloat("##x", &values[0], 1.f, 0.f, 0.f, "%.2f");
    ImGui::PopItemWidth();
    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.2f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.7f, 0.2f, 1.f));
    if(ImGui::Button("Y", buttonSize))
        values[1] = resetValue;
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    ImGui::DragFloat("##y", &values[1], 1.f, 0.f, 0.f, "%.2f");
    ImGui::PopItemWidth();
    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.25f, 0.9f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.35f, 1.f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.25f, 0.8f, 1.f));
    if(ImGui::Button("Z", buttonSize))
        values[2] = resetValue;

    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    ImGui::DragFloat("##z", &values[2], 1.f, 0.f, 0.f, "%.2f");
    ImGui::PopItemWidth();
    ImGui::PopStyleVar();

    ImGui::Columns(1);
    ImGui::PopID();
}

void Overlay::DrawDragFloat(const std::string& label, float* values, float speed, float columnWidth) {
    ImGui::PushID(label.c_str());

    ImGui::Columns(2);
    ImGui::SetColumnWidth(0, columnWidth);
    ImGui::Text(label.c_str());
    ImGui::NextColumn();

    std::string labelID = "##" + label;
    ImGui::DragFloat(labelID.c_str(), values, speed, 0.f, 0.f, "%.4f");
    ImGui::Columns(1);
    ImGui::PopID();
}

void Overlay::DrawSliderFloat(const std::string& label, float* values, float min, float max, float columnWidth) 
{
    ImGui::PushID(label.c_str());

    ImGui::Columns(2);
    ImGui::SetColumnWidth(0, columnWidth);
    ImGui::Text(label.c_str());
    ImGui::NextColumn();

    std::string labelID = "##" + label;
    ImGui::SliderFloat(labelID.c_str(), values, min, max, "%.4f");
    ImGui::Columns(1);
    ImGui::PopID();
}

void Overlay::renderGUI(ImGuiIO& io, float& exposure, float& shadowRadius, float& filterRadius, float& bloomThreshold,
    bool& drawSkybox, bool& displayGrid, bool& displayCoordinateSystem, bool& enableBloom, bool& enableWireframe,
    bool& enableShadows, bool& enableHDR, bool& enableSSAO, glm::vec3& dirLightLocation, Mesh* currMesh) 
{
    ImGui::PushFont(this->mainfont);
    ImGui::Begin("Scene Information and modifiers");

    if (ImGui::TreeNode("Scene")) {
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 3.f);
            ImGui::Checkbox("Render Skybox", &drawSkybox);
            ImGui::Checkbox("Display Coordinate System", &displayCoordinateSystem);
            ImGui::Checkbox("Display Grid", &displayGrid);
            ImGui::Checkbox("Wireframe", &enableWireframe);
            ImGui::Checkbox("Enable HDR", &enableHDR);
            ImGui::Checkbox("Enable bloom", &enableBloom);
            ImGui::Checkbox("Enable shadows", &enableShadows);
            ImGui::Checkbox("SSAO", &enableSSAO);
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 3.f);
        ImGui::TreePop();
    }

    ImGui::NewLine();

    if (ImGui::TreeNode("HDR")) {
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 3.f);
            this->DrawDragFloat("Exposure", &exposure, 0.01f, 150.f);
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 3.f);
        ImGui::TreePop();
    }

    ImGui::NewLine();

    if (ImGui::TreeNode("Bloom")) {
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 3.f);
            this->DrawDragFloat("Filter Radius", &filterRadius, 0.0001f, 150.f);
            bloomThreshold = bloomThreshold < 0.f ? 0.f : bloomThreshold;
            this->DrawDragFloat("Knee", &bloomThreshold, 0.01f, 150.f);
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 3.f);
        ImGui::TreePop();
    }

    ImGui::NewLine();

    if (ImGui::TreeNode("Shadows")) {
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 3.f);
            this->DrawDragFloat("PCF filter radius", &shadowRadius, 0.001f, 150.f);
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 3.f);
        ImGui::TreePop();
    }

    ImGui::NewLine();

    if (ImGui::TreeNode("Material Properties")) {
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 3.f);
            ImGui::Columns(2);
            ImGui::SetColumnWidth(0, 120.f);
            ImGui::Text("Albedo");
            ImGui::NextColumn();
            ImGui::ColorEdit4("##color", this->colorBuffer);
            ImGui::Columns(1);

            this->DrawSliderFloat("Metallic", &this->metallic, 0.f, 1.f);
            this->DrawSliderFloat("Roughness", &this->roughness, 0.f, 1.f);
            this->DrawSliderFloat("AO", &this->ao, 0.f, 1.f);
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 3.f);

        ImGui::TreePop();
    }

    ImGui::NewLine();

    if (ImGui::TreeNode("Transforms"))
    {
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 3.f);
            this->DrawVec3Control("Translation", this->translation);
            this->DrawVec3Control("Rotation", this->rotation);
            this->DrawVec3Control("Scale", this->scale, 1.f);
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 3.f);

        ImGui::TreePop();
    }

    ImGui::NewLine();
    this->DrawVec3Control("Directional Light", glm::value_ptr(dirLightLocation), 0.f, 150.f);
    ImGui::NewLine();
    
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
    ImGui::PopFont();
    ImGui::End();

    if (currMesh != nullptr) {
        glm::mat4 model = glm::mat4(1.f);
        
        ImGuizmo::RecomposeMatrixFromComponents(
            this->translation,
            this->rotation,
            this->scale,
            glm::value_ptr(model)
        );

        currMesh->setModelMatrix(model);

        glm::vec3 color{ this->colorBuffer[0], this->colorBuffer[1], this->colorBuffer[2] };

        currMesh->setColor(color);
        currMesh->setMeshMaterial(this->roughness, this->metallic, this->ao);
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Overlay::manipulate(int windowWidth, int windowHeight, const Camera& camera, Mesh* mesh) {
    ImGuizmo::BeginFrame();
    ImGuizmo::Enable(true);
    ImGuizmo::AllowAxisFlip(false);
    ImGuizmo::SetOrthographic(false);
    ImGuizmo::SetDrawlist(ImGui::GetBackgroundDrawList());
    ImGuizmo::SetRect(0, 0, windowWidth, windowHeight);
    ImGuizmo::Manipulate(glm::value_ptr(camera.generateViewMatrix()), glm::value_ptr(camera.getProjectionMatrix()),
        transformOperation, ImGuizmo::WORLD, glm::value_ptr(mesh->getModelMatrix()));
    ImGuizmo::DecomposeMatrixToComponents(
        glm::value_ptr(mesh->getModelMatrix()), this->translation, this->rotation, this->scale
    );

    this->colorBuffer[0] = mesh->getColor().x;
    this->colorBuffer[1] = mesh->getColor().y;
    this->colorBuffer[2] = mesh->getColor().z;

    this->metallic = mesh->getMetallic();
    this->roughness = mesh->getRoughness();
    this->ao = mesh->getAO();
}

Overlay::~Overlay() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}