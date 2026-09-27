#ifndef SONAR_HLSLI
#define SONAR_HLSLI

// Intensity of a spherical wavefront (center, radius) on scene geometry.
// Hard cut at the front, linear fade over trailLength behind it (toward the center).
float SonarBand(float3 worldPos, float3 center, float radius, float trailLength) {
    float behindFront = radius - length(worldPos - center);
    if (behindFront < 0.0) return 0.0;
    return 1.0 - saturate(behindFront / max(trailLength, 1e-4));
}

#endif
