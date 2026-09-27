#pragma once

#include <optional>
#include <vector>

#include <include-glm.h>
#include <common.hpp>

class BeMaterial;

class SonarSystem {
    hide
    // must match SonarWaves length in posterize.hlsl
    static constexpr size_t kMaxWaves = 16;

    struct Wave {
        glm::vec3 Center{0.0f};
        float Radius = 0.0f;
        float Speed = 0.0f;
    };

    std::vector<Wave> _waves;

    expose
    auto TryPing(glm::vec3 shipPos, std::optional<glm::vec3> target) -> bool;
    auto Update(float deltaTime, glm::vec3 viewerPos) -> void;
    auto Apply(BeMaterial& posterize) const -> void;
    auto Reset() -> void { _waves.clear(); }
};
