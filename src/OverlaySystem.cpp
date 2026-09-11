#include "OverlaySystem.h"

#include <algorithm>
#include <utility>
#include <vector>

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

auto OverlaySystem::Push(std::unique_ptr<Overlay> overlay) -> void {
    if (!overlay || IsOpen(overlay->Id())) { return; }
    _stack.push_back(std::move(overlay));
    _dirty = true;
}

auto OverlaySystem::CloseTop() -> void {
    if (!_stack.empty()) {
        _stack.pop_back();
        _dirty = true;
    }
}

auto OverlaySystem::Close(std::string_view id) -> void {
    std::erase_if(
        _stack,
        [&](const std::unique_ptr<Overlay>& overlay) -> bool { return id == overlay->Id(); }
    );
    _dirty = true;
}

auto OverlaySystem::Clear() -> void {
    _stack.clear();
    _dirty = true;
}

auto OverlaySystem::Draw() -> void {
    const bool reorder = _dirty;
    _dirty = false;

    bool hasDim = false;
    size_t dimIndex = 0;
    for (size_t index = _stack.size(); index-- > 0;) {
        if (_stack[index]->DimsBehind()) {
            dimIndex = index;
            hasDim = true;
            break;
        }
    }

    std::vector<size_t> closed;
    for (size_t index = 0; index < _stack.size(); ++index) {
        if (hasDim && index == dimIndex) {
            if (reorder) {
                ImGui::SetNextWindowFocus();
            }
            DrawDim();
        }
        if (reorder) {
            ImGui::SetNextWindowFocus();
        }
        if (!_stack[index]->Draw()) {
            closed.push_back(index);
        }
    }

    if (!closed.empty()) {
        for (auto it = closed.rbegin(); it != closed.rend(); ++it) {
            _stack.erase(_stack.begin() + static_cast<std::ptrdiff_t>(*it));
        }
        _dirty = true;
    }
}

auto OverlaySystem::DrawDim() -> void {
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.5f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    constexpr ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoNavFocus
    ;
    ImGui::Begin("###overlay-dim", nullptr, flags);
    ImGui::InvisibleButton(
        "##veil",
        viewport->Size,
        ImGuiButtonFlags_NoNavFocus | ImGuiButtonFlags_NoFocus
    );
    ImGui::End();
    ImGui::PopStyleVar(3);
    ImGui::PopStyleColor();
}

auto OverlaySystem::IsOpen(std::string_view id) const -> bool {
    return std::ranges::any_of(
        _stack,
        [&](const std::unique_ptr<Overlay>& overlay) -> bool { return id == overlay->Id(); }
    );
}

auto OverlaySystem::BlocksControls() const -> bool {
    return std::ranges::any_of(
        _stack, 
        [](const std::unique_ptr<Overlay>& overlay) -> bool { return overlay->BlocksControls(); }
    );
}

auto OverlaySystem::FreezesSim() const -> bool {
    return std::ranges::any_of(
        _stack,
        [](const std::unique_ptr<Overlay>& overlay) -> bool { return overlay->FreezesSim(); }
    );
}

auto OverlaySystem::ReleasesCursor() const -> bool {
    return std::ranges::any_of(
        _stack,
        [](const std::unique_ptr<Overlay>& overlay) -> bool { return overlay->ReleasesCursor(); }
    );
}

MessageOverlay::MessageOverlay(
    std::string id, 
    std::string title, 
    std::string body, 
    std::vector<Button> buttons
)
: _id(std::move(id))
, _title(std::move(title))
, _body(std::move(body))
, _buttons(std::move(buttons)) {}

auto MessageOverlay::Draw() -> bool {
    const std::string label = _title + "###" + _id;

    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(460.0f, 0.0f), ImGuiCond_Always);
    constexpr ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoSavedSettings
    ;
    ImGui::Begin(label.c_str(), nullptr, flags);

    bool keepOpen = true;

    ImGui::PushTextWrapPos(0.0f);
    if (!_body.empty()) {
        ImGui::TextWrapped("%s", _body.c_str());
        ImGui::Spacing();
    }
    for (const Button& button : _buttons) {
        if (ImGui::Button(button.Label.c_str(), ImVec2(-1.0f, 0.0f))) {
            if (button.OnClick) {
                button.OnClick();
            }
            keepOpen = false;
        }
    }
    ImGui::PopTextWrapPos();
    ImGui::End();

    return keepOpen;
}
