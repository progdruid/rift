#include "OxygenSystem.h"

#include "BeMaterial.h"
#include "RiftSettings.h"
#include "RiftTerrain.h"

OxygenSystem::OxygenSystem(const RiftTerrain& terrain)
    : _terrain(terrain)
{}

auto OxygenSystem::Update(float deltaTime, glm::vec3 shipPos, bool docked, bool inZone) -> void {
    const auto& oxygen = RiftStore::Get().Oxygen;
    if (!oxygen.Enabled) {
        _level = 1.0f;
        return;
    }

    const float altitude = shipPos.y - _terrain.GetHeight(shipPos.x, shipPos.z);
    const bool breathable = altitude < oxygen.LossAltitude || inZone;

    float speed = -oxygen.LossSpeed;
    if (docked) speed = oxygen.DockedRecoverSpeed;
    else if (breathable) speed = oxygen.RecoverSpeed;

    _level = glm::clamp(_level + speed * deltaTime, 0.0f, 1.0f);
}

auto OxygenSystem::ApplyVignette(BeMaterial& vignette) const -> void {
    const auto& oxygen = RiftStore::Get().Oxygen;
    const float suffocation = 1.0f - glm::clamp(_level / glm::max(oxygen.VignetteStart, 1e-4f), 0.0f, 1.0f);
    vignette.SetFloat1("Radius", glm::mix(oxygen.VignetteMaxRadius, -oxygen.VignetteSoftness, suffocation));
    vignette.SetFloat1("Softness", oxygen.VignetteSoftness);
    vignette.SetFloat3("Color", oxygen.VignetteColor);
}
