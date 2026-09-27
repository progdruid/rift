#include "SonarSystem.h"

#include "BeMaterial.h"
#include "RiftSettings.h"

auto SonarSystem::TryPing(glm::vec3 shipPos, std::optional<glm::vec3> target) -> bool {
    const auto& settings = RiftStore::Get();
    const auto& sonar = settings.Sonar;
    if (!settings.Ship.FlightAssist) return false;

    _ping = { .Center = shipPos, .Radius = 0.0f, .Speed = sonar.PingSpeed, .Active = true };
    _echo.Active = false;
    if (!target) return true;

    const float distance = glm::length(*target - shipPos);
    const float delay = settings.Posterize.FogEnd / sonar.EchoSpeed + distance * sonar.EchoDelayPerMeter;
    _echo = { .Center = *target, .Radius = distance - sonar.EchoSpeed * delay, .Speed = sonar.EchoSpeed, .Active = true };
    return true;
}

auto SonarSystem::Update(float deltaTime, glm::vec3 viewerPos) -> void {
    const auto& settings = RiftStore::Get();
    const float margin = settings.Posterize.FogEnd + settings.Sonar.TrailLength + settings.Sonar.EndMargin;

    for (Wave* wave : { &_ping, &_echo }) {
        if (!wave->Active) continue;
        wave->Radius += wave->Speed * deltaTime;
        if (wave->Radius >= glm::length(viewerPos - wave->Center) + margin) wave->Active = false;
    }
}

auto SonarSystem::Apply(BeMaterial& posterize) const -> void {
    const auto& sonar = RiftStore::Get().Sonar;
    posterize.SetFloat3("SonarPingCenter", _ping.Center);
    posterize.SetFloat1("SonarPingRadius", _ping.Radius);
    posterize.SetFloat1("SonarPingActive", _ping.Active ? 1.0f : 0.0f);
    posterize.SetFloat3("SonarEchoCenter", _echo.Center);
    posterize.SetFloat1("SonarEchoRadius", _echo.Radius);
    posterize.SetFloat1("SonarEchoActive", _echo.Active ? 1.0f : 0.0f);
    posterize.SetFloat1("SonarTrailLength", sonar.TrailLength);
    posterize.SetFloat3("SonarColor", sonar.Color);
}
