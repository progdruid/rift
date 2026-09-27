#include "ShipHud.h"

#include <algorithm>
#include <cmath>

#include "BeCamera.h"
#include "BeMaterial.h"
#include "BeShaderLibrary.h"
#include "DeliverySystem.h"
#include "RiftSettings.h"

ShipHud::ShipHud() {
    const auto& settings = RiftStore::Get();
    const auto& shader = *BeShaderLibrary::GetShader("ship-hud");
    _material = BeMaterial::Create(BeShaderLibrary::GetShaderScheme(shader, "main"));
    _material->SetFloat1("PixelSize", settings.Posterize.PixelSize);
    _material->SetFloat1("AimRadius", settings.Ship.AimRadius);
    _stableBlend = settings.Ship.FlightAssist ? 1.0f : 0.0f;
}

auto ShipHud::Update(float deltaTime, const BeCamera& camera, glm::vec2 aim, glm::vec2 screenSize, const DeliverySystem* delivery) -> void {
    const auto& hud = RiftStore::Get().Hud;
    const auto& ship = RiftStore::Get().Ship;
    const auto& marker = hud.Marker;

    // style
    _material->SetFloat1("Enabled", hud.Enabled ? 1.0f : 0.0f);
    _material->SetFloat3("Color", hud.Color);
    _material->SetFloat1("LineHalf", hud.LineHalf);
    _material->SetFloat1("PipHalf", hud.PipHalf);
    _material->SetFloat1("AimBoxHalf", hud.AimBoxHalf);
    _material->SetFloat1("DashPeriod", hud.DashPeriod);
    _material->SetFloat1("WingTickLength", hud.WingTickLength);
    _material->SetFloat1("GroundTickLength", hud.GroundTickLength);
    _material->SetFloat1("TargetArrowSize", marker.ArrowSize);


    // screen and aim
    _material->SetFloat2("ScreenSize", screenSize);
    _material->SetFloat2("AimOffset", aim);


    // flight mode brackets
    const float stableTarget = ship.FlightAssist ? 1.0f : 0.0f;
    const float stableStep = deltaTime / glm::max(hud.BracketMorphTime, 1e-4f);
    _stableBlend += glm::clamp(stableTarget - _stableBlend, -stableStep, stableStep);
    const float stableEase = glm::smoothstep(0.0f, 1.0f, _stableBlend);
    _material->SetFloat1("BracketOffset", glm::mix(hud.BracketOffset, hud.BracketStableOffset, stableEase));
    _material->SetFloat1("BracketArm", glm::mix(hud.BracketArm, hud.BracketStableOffset, stableEase));


    // horizon
    const glm::vec3 worldUp = { 0.0f, 1.0f, 0.0f };
    const glm::vec2 upScreen = { glm::dot(worldUp, camera.GetRight()), glm::dot(worldUp, camera.GetUp()) };
    glm::vec2 horizonDir = { upScreen.y, -upScreen.x };
    const float horizonLen = glm::length(horizonDir);
    horizonDir = horizonLen > 1e-3f ? horizonDir / horizonLen : glm::vec2(1.0f, 0.0f);
    _material->SetFloat2("HorizonDir", { horizonDir.x, -horizonDir.y });


    // wing ticks
    const float pitch = glm::degrees(std::asin(glm::clamp(camera.GetFront().y, -1.0f, 1.0f)));
    const float wingFade = glm::smoothstep(hud.WingTickFadeStartPitch, hud.WingTickFadeEndPitch, std::abs(pitch));
    _material->SetFloat1("WingTickAlpha", 1.0f - wingFade);
    _material->SetFloat1("WingTickOffset", (pitch >= 0.0f ? 1.0f : -1.0f) * wingFade * hud.WingTickSlide);


    // delivery target marker
    float targetState = 0.0f;
    glm::vec2 targetPixel = screenSize * 0.5f;
    glm::vec2 targetDir = { 0.0f, 1.0f };
    float targetRadius = marker.MinRadius;
    float targetAlpha = 1.0f;
    if (delivery && delivery->HasContract() && !delivery->CanComplete()) {
        const glm::vec3 targetWorld = delivery->GetTargetPosition(camera.Position);
        const float distance = glm::length(targetWorld - camera.Position);
        const glm::vec4 clip = camera.GetProjectionMatrix() * camera.GetViewMatrix() * glm::vec4(targetWorld, 1.0f);
        const bool behind = clip.w <= 1e-4f;
        glm::vec2 ndc = glm::vec2(clip.x, clip.y) / clip.w;
        if (behind) ndc = -ndc;
        const bool onScreen = !behind && std::abs(ndc.x) <= 1.0f && std::abs(ndc.y) <= 1.0f;
        if (distance > marker.SizeFar) {
            targetState = 0.0f;
        } else if (onScreen) {
            targetState = 1.0f;
            targetPixel = { (ndc.x * 0.5f + 0.5f) * screenSize.x, (0.5f - ndc.y * 0.5f) * screenSize.y };
            targetRadius = glm::mix(marker.MinRadius, marker.MaxRadius, glm::smoothstep(marker.SizeFar, marker.SizeNear, distance));
            targetAlpha = glm::smoothstep(marker.FadeNear, marker.FadeFar, distance);
        } else {
            targetState = 2.0f;
            const float extent = std::max(std::max(std::abs(ndc.x), std::abs(ndc.y)), 1e-4f);
            const glm::vec2 marked = (ndc / extent) * marker.ScreenMargin;
            targetPixel = { (marked.x * 0.5f + 0.5f) * screenSize.x, (0.5f - marked.y * 0.5f) * screenSize.y };
            targetDir = glm::normalize(glm::vec2(ndc.x, -ndc.y));
        }
    }
    _material->SetFloat2("TargetPos", targetPixel);
    _material->SetFloat2("TargetDir", targetDir);
    _material->SetFloat1("TargetState", targetState);
    _material->SetFloat1("TargetRingRadius", targetRadius);
    _material->SetFloat1("TargetAlpha", targetAlpha);
}
