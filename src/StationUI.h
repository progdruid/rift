#pragma once

#include <umbrellas/include-glm.h>

#include "OverlaySystem.h"

class DeliverySystem;

namespace StationUI {
    auto Draw(DeliverySystem& delivery, glm::vec3 shipPosition) -> void;
}

class StationOverlay : public Overlay {
    hide
    DeliverySystem& _delivery;
    const glm::vec3& _shipPosition;

    expose
    StationOverlay(DeliverySystem& delivery, const glm::vec3& shipPosition);

    auto Draw() -> bool override;
    [[nodiscard]] auto Id() const -> const char* override { return "station"; }
};
