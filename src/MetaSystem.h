#pragma once

#include <umbrellas/common.hpp>

class DeliverySystem;
class OverlaySystem;

class MetaSystem {
    expose
    enum class Phase { Inactive, Briefing, Running, Won, Lost, Free };

    hide
    Phase _phase = Phase::Inactive;
    float _elapsed = 0.0f;
    bool _wantsClose = false;

    expose
    auto Begin(OverlaySystem& overlays) -> void;
    auto End() -> void;
    auto Update(
        float deltaTime,
        const DeliverySystem& delivery,
        OverlaySystem& overlays
    ) -> void;
    auto DrawHud(const DeliverySystem& delivery) -> void;

    [[nodiscard]] auto WantsClose() const -> bool { return _wantsClose; }
    [[nodiscard]] auto GetElapsed() const -> float { return _elapsed; }
    auto SetElapsed(float elapsed) -> void;

    hide
    auto Announce(OverlaySystem& overlays) -> void;
};
