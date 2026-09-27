#pragma once

#include <memory>

#include <include-glm.h>
#include <common.hpp>

class BeCamera;
class BeMaterial;
class DeliverySystem;

class ShipHud {
    hide
    std::shared_ptr<BeMaterial> _material;

    expose
    ShipHud();

    auto Update(const BeCamera& camera, glm::vec2 aim, glm::vec2 screenSize, const DeliverySystem* delivery) -> void;

    [[nodiscard]] auto GetMaterial() const -> const std::shared_ptr<BeMaterial>& { return _material; }
};
