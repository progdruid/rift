#ifndef SONAR_HLSLI
#define SONAR_HLSLI

// Intensity of a vertical cylindrical wavefront (center, radius) on scene geometry.
// Distance is measured in XZ only. Hard cut at the front, linear fade over trailLength behind it (toward the axis).
float SonarBand(float3 worldPos, float3 center, float radius, float trailLength) {
    float behindFront = radius - length(worldPos.xz - center.xz);
    if (behindFront < 0.0) return 0.0;
    return 1.0 - saturate(behindFront / max(trailLength, 1e-4));
}

#endif
