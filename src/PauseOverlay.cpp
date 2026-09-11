#include "PauseOverlay.h"

#include <utility>

#include "RiftSettings.h"
#include "imgui/imgui.h"

PauseOverlay::PauseOverlay(std::function<void()> onQuit)
: _onQuit(std::move(onQuit)) {}

auto PauseOverlay::Draw() -> bool {
    constexpr const char* label = "UNPAID BREAK###pause";

    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(360.0f, 0.0f), ImGuiCond_Always);
    constexpr ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoSavedSettings
    ;
    ImGui::Begin(label, nullptr, flags);

    bool keepOpen = true;

    ImGui::SeparatorText("Settings");
    ImGui::Dummy(ImVec2(0.0f, 48.0f));

    ImGui::SeparatorText("Controls");
    ImGui::TextUnformatted(kControlsHelp);

    ImGui::SeparatorText("");
    const float halfWidth = (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x) * 0.5f;
    if (ImGui::Button("Resume", ImVec2(halfWidth, 0.0f))) {
        keepOpen = false;
    }
    ImGui::SameLine();
    if (ImGui::Button("Quit", ImVec2(halfWidth, 0.0f))) {
        if (_onQuit) {
            _onQuit();
        }
        keepOpen = false;
    }

    ImGui::End();

    return keepOpen;
}
