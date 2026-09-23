#pragma once

#include <array>
#include <cstdint>
#include <memory>
#include <umbrellas/common.hpp>

#include "DeliverySystem.h"
#include "standard-game/BeStandardFullScene.h"
#include "MetaSystem.h"
#include "OverlaySystem.h"

class ShipCameraController;
class DeliverySystem;
class RiftTerrain;
class BeMaterial;
class BeImGuiPass;
struct ImFont;

class RiftScene : public BeStandardFullScene {
    hide
    std::unique_ptr<RiftTerrain> _terrain;
    std::unique_ptr<ShipCameraController> _shipCameraController;
    std::unique_ptr<DeliverySystem> _delivery;
    std::array<entt::entity, 9> _terrainTiles;
    std::shared_ptr<BeMaterial> _posterizeMaterial;
    std::shared_ptr<BeMaterial> _hudMaterial;
    ImFont* _riftFont = nullptr;
    bool _dying = false;
    float _oxygenBarAlpha = 0.0f;
    bool _showDebug = false;
    MetaSystem _meta;
    OverlaySystem _overlays;

    expose
    explicit RiftScene(BeStandardGame* game);
    ~RiftScene() override;

    auto Prepare() -> void override;
    auto Tick(float deltaTime) -> void override;

    hide
    auto EnterPlayMode() -> void;
    auto ExitPlayMode() -> void;
    auto OpenPauseMenu() -> void;
    auto DeathSequence() -> BeCoroutine;

    protect
    auto DefineAssets() -> void override;
    auto DefineSettings() -> void override;
    auto DefineScene() -> void override;
    auto DefinePasses() -> void override;
};
