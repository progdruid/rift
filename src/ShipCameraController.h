#pragma once

#include <include-glm.h>
#include <common.hpp>

class BeCamera;
class BeInput;
class RiftTerrain;

class ShipCameraController {
    hide
    BeCamera* _camera;
    const RiftTerrain* _terrain;
    glm::vec3 _velocity{0.0f};
    glm::vec3 _angularVelocity{0.0f};
    glm::vec2 _aim{0.0f};
    float _lastImpactSpeed{0.0f};
    float _groundEffectProximity{0.0f};
    bool _isInDock{false};
    bool _wasInDockLast{false};
    bool _isCaptured{false};
    bool _controlsEnabled{true};
    glm::vec3 _anchor{0.0f};

    expose
    explicit ShipCameraController(BeCamera* camera, const RiftTerrain* terrain);

    auto Update(float deltaTime, BeInput* input) -> void;
    auto DrawDebugUI() -> void;
    auto Respawn(glm::vec3 position) -> void;
    auto GetAim() const -> glm::vec2 { return _aim; }
    auto GetLastImpactSpeed() const -> float { return _lastImpactSpeed; }

    auto SetInDock(bool inDock) -> void { _wasInDockLast = _isInDock; _isInDock = inDock; }
    auto HasJustEnteredDock() const -> bool { return _isInDock && !_wasInDockLast; }

    auto Capture(glm::vec3 anchor) -> void { _isCaptured = true; _anchor = anchor; }
    auto Uncapture() -> void { _isCaptured = false; }
    auto IsCaptured() const -> bool { return _isCaptured; }

    auto SetControlsEnabled(bool enabled) -> void { _controlsEnabled = enabled; }
};
