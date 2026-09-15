#include "GUIController.hpp"

#include <engine/graphics/GraphicsController.hpp>
#include <imgui.h>

#include "LightingController.hpp"

void GUIController::draw() {
    auto *graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto *lighting = engine::core::Controller::get<LightingController>();

    graphics->begin_gui();

    ImGui::Begin("Lighting");
    ImGui::Text("Directional light");

    ImGui::DragFloat3(
            "Direction",
            &lighting->lights.directional.direction.x,
            0.05f,
            -1.0f,
            1.0f
            );

    ImGui::ColorEdit3(
            "Directional color",
            &lighting->lights.directional.color.x
            );

    ImGui::SliderFloat(
            "Directional intensity",
            &lighting->lights.directional.intensity,
            0.0f,
            3.0f
            );

    ImGui::Separator();

    ImGui::Text("Point light");

    ImGui::DragFloat3(
            "Position",
            &lighting->lights.point.position.x,
            0.05f
            );

    ImGui::ColorEdit3(
            "Point color",
            &lighting->lights.point.color.x
            );

    ImGui::SliderFloat(
            "Point intensity",
            &lighting->lights.point.intensity,
            0.0f,
            5.0f
            );

    ImGui::Separator();

    ImGui::Text("Ambient");

    ImGui::SliderFloat(
            "Ambient intensity",
            &lighting->lights.ambient_intensity,
            0.0f,
            1.0f);

    ImGui::End();

    graphics->end_gui();
}