#include "UI.h"
#include "IMGUI/imgui.h"
#include <cstdlib>
#include <string>
#include <glm/gtc/type_ptr.hpp>

void DrawUI(Camera& camera, std::vector<PlacedShape>& shapes, int& selected, UIState& ui)
{
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    float rightPanelWidth = 320.0f;

    ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x + viewport->WorkSize.x - rightPanelWidth, viewport->WorkPos.y), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(rightPanelWidth, viewport->WorkSize.y * 0.45f), ImGuiCond_Always);

    ImGui::Begin("Scene Hierarchy", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    if (ImGui::Button("Add Cube (+)", ImVec2(-1, 0))) {
        PlacedShape ns;
        ns.position = camera.target;
        ns.color = {
            0.4f + (rand() % 60) / 100.0f,
            0.4f + (rand() % 60) / 100.0f,
            0.4f + (rand() % 60) / 100.0f
        };
        shapes.push_back(ns);
        selected = (int)shapes.size() - 1;
    }

    if (ImGui::Button("Duplicate", ImVec2(-1, 0)) && selected >= 0 && selected < (int)shapes.size()) {
        PlacedShape copy = shapes[selected];
        copy.position += glm::vec3(0.6f, 0.0f, 0.6f);
        shapes.push_back(copy);
        selected = (int)shapes.size() - 1;
    }

    ImGui::Separator();

    for (int i = 0; i < (int)shapes.size(); i++) {
        std::string label = "Cube " + std::to_string(i + 1);
        if (ImGui::Selectable(label.c_str(), selected == i))
            selected = i;
    }

    ImGui::End();

    ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x + viewport->WorkSize.x - rightPanelWidth, viewport->WorkPos.y + viewport->WorkSize.y * 0.45f), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(rightPanelWidth, viewport->WorkSize.y * 0.55f), ImGuiCond_Always);

    ImGui::Begin("Properties", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    if (selected >= 0 && selected < (int)shapes.size()) {
        auto& shape = shapes[selected];
        ImGui::Text("Selected: Cube %d", selected + 1);
        ImGui::TextDisabled("F = Focus  |  Ctrl+D = Duplicate");
        ImGui::Separator();

        ImGui::DragFloat3("Location", glm::value_ptr(shape.position), 0.1f);
        ImGui::DragFloat3("Rotation", glm::value_ptr(shape.rotation), 1.0f);
        ImGui::DragFloat3("Scale", glm::value_ptr(shape.scale), 0.05f, 0.05f, 20.0f);

        ImGui::Separator();
        ImGui::ColorEdit3("Color", glm::value_ptr(shape.color));

        ImGui::Separator();
        if (ImGui::Button("Duplicate Object", ImVec2(-1, 0))) {
            PlacedShape copy = shape;
            copy.position += glm::vec3(0.6f, 0.0f, 0.6f);
            shapes.push_back(copy);
            selected = (int)shapes.size() - 1;
        }
        if (ImGui::Button("Delete Object", ImVec2(-1, 0))) {
            shapes.erase(shapes.begin() + selected);
            selected = -1;
        }
    }
    else {
        ImGui::Text("No object selected");
        ImGui::TextDisabled("Left-click a cube to select it");
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Text("Camera");
    ImGui::TextDisabled("MMB Orbit  |  Shift+MMB Pan  |  Scroll Zoom");
    ImGui::DragFloat3("Target", glm::value_ptr(camera.target), 0.1f);

    if (ImGui::Button("Reset View", ImVec2(-1, 0))) {
        camera.target = { 0, 0, 0 };
        camera.distance = 12.0f;
        camera.yaw = -45.0f;
        camera.pitch = 25.0f;
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Text("Environment");

    if (ImGui::ColorEdit3("Background", glm::value_ptr(ui.clearColor))) {
        if (ui.autoGridColor)
            ui.gridColor = glm::vec3(1.0f) - ui.clearColor * 0.7f;
    }

    if (ImGui::Checkbox("Auto Grid Color", &ui.autoGridColor) && ui.autoGridColor)
        ui.gridColor = glm::vec3(1.0f) - ui.clearColor * 0.7f;

    if (ui.autoGridColor) ImGui::BeginDisabled();
    ImGui::ColorEdit3("Grid Color", glm::value_ptr(ui.gridColor));
    if (ui.autoGridColor) ImGui::EndDisabled();

    ImGui::End();
}