#include "SonarSystem.h"

#include <algorithm>

#include "BeMaterial.h"
#include "RiftSettings.h"

namespace {
    // waves are vertical cylinders, so only XZ distance matters
    auto HorizontalDistance(glm::vec3 a, glm::vec3 b) -> float {
        return glm::length(glm::vec2(a.x - b.x, a.z - b.z));
    }
}

auto SonarSystem::TryPing(glm::vec3 shipPos, std::optional<glm::vec3> target) -> bool {
    const auto& settings = RiftStore::Get();
    const auto& sonar = settings.Sonar;
    if (!settings.Ship.FlightAssist) return false;

    _waves.push_back({ .Center = shipPos, .Radius = 0.0f, .Speed = sonar.PingSpeed });
    if (target) {
        const float distance = HorizontalDistance(*target, shipPos);
        const float delay = settings.Posterize.FogEnd / sonar.EchoSpeed + distance * sonar.EchoDelayPerMeter;
        _waves.push_back({ .Center = *target, .Radius = distance - sonar.EchoSpeed * delay, .Speed = sonar.EchoSpeed });
    }

    // drop the oldest waves when the pool is full
    if (_waves.size() > kMaxWaves) _waves.erase(_waves.begin(), _waves.end() - kMaxWaves);
    return true;
}

auto SonarSystem::Update(float deltaTime, glm::vec3 viewerPos) -> void {
    const auto& settings = RiftStore::Get();
    const float margin = settings.Posterize.FogEnd + settings.Sonar.TrailLength + settings.Sonar.EndMargin;

    for (Wave& wave : _waves) wave.Radius += wave.Speed * deltaTime;
    std::erase_if(_waves, [&](const Wave& wave) {
        return wave.Radius >= HorizontalDistance(viewerPos, wave.Center) + margin;
    });
}

auto SonarSystem::Apply(BeMaterial& posterize) const -> void {
    const auto& sonar = RiftStore::Get().Sonar;
    std::vector<glm::vec4> packed;
    packed.reserve(_waves.size());
    for (const Wave& wave : _waves) packed.emplace_back(wave.Center, wave.Radius);
    posterize.SetFloat4Array("SonarWaves", packed);
    posterize.SetFloat1("SonarWaveCount", static_cast<float>(packed.size()));
    posterize.SetFloat1("SonarTrailLength", sonar.TrailLength);
    posterize.SetFloat3("SonarColor", sonar.Color);
}
