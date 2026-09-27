#pragma once

#include <include-glm.h>
#include <common.hpp>

class BeMaterial;
class RiftTerrain;

class OxygenSystem {
    hide
    const RiftTerrain& _terrain;
    float _level = 1.0f;

    expose
    explicit OxygenSystem(const RiftTerrain& terrain);

    auto Update(float deltaTime, glm::vec3 shipPos, bool docked, bool inZone) -> void;
    auto ApplyVignette(BeMaterial& vignette) const -> void;
    auto Reset() -> void { _level = 1.0f; }

    [[nodiscard]] auto IsDepleted() const -> bool { return _level <= 0.01f; }
    [[nodiscard]] auto GetLevel() const -> float { return _level; }
};
