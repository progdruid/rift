#pragma once

#include <functional>

#include <common.hpp>

#include "OverlaySystem.h"

class PauseOverlay : public Overlay {
    hide
    std::function<void()> _onQuit;

    expose
    explicit PauseOverlay(std::function<void()> onQuit);

    expose
    auto Draw() -> bool override;
    [[nodiscard]] auto FreezesSim() const -> bool override { return true; }
    [[nodiscard]] auto DimsBehind() const -> bool override { return true; }
    [[nodiscard]] auto Id() const -> const char* override { return "pause"; }
};
