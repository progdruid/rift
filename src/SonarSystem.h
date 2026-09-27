#pragma once

#include <optional>

#include <include-glm.h>
#include <common.hpp>

class BeMaterial;

class SonarSystem {
    hide
    struct Wave {
        glm::vec3 Center{0.0f};
        float Radius = 0.0f;
        float Speed = 0.0f;
        bool Active = false;
    };

    Wave _ping;
    Wave _echo;

    expose
    auto TryPing(glm::vec3 shipPos, std::optional<glm::vec3> target) -> bool;
    auto Update(float deltaTime, glm::vec3 viewerPos) -> void;
    auto Apply(BeMaterial& posterize) const -> void;
    auto Reset() -> void { _ping.Active = false; _echo.Active = false; }
};
