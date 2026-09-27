/*

@be-material: posterize-material {
    ColorTexture: texture2d = white
    DepthTexture: texture2d = black
    UITexture: texture2d = black
    PointSampler: sampler = point-clamp
    PixelSize: float = 8.0
    DitherSpread: float = 0.5
    FogStart: float = 6.0
    FogEnd: float = 45.0
    FogColor: float3 = #334D80
    Enabled: float = 1.0
    PaletteCount: float = 7.0
    Palette: float3[8] = []
}

@be-shader posterize {
    topology triangle-strip
    rasterizer back-solid
    blend disable
    depth disable

    vertex FullscreenVertexKernel
    pixel PS

    bind s0 frame uniform-material
    bind s1 main posterize-material

    target s0 PosterizeOutput float3
}

*/

/*========================================================*/
// region @be-auto-boilerplate
#include "core/be-bindless-tables.hlsl"
#include "core/uniform-material.hlsl"

struct posterize_material {
    float PixelSize;
    float DitherSpread;
    float FogStart;
    float FogEnd;
    float3 FogColor;
    float Enabled;
    float PaletteCount;
    float3 Palette[8];
};

struct DrawRoot {
    uniform_material* Frame;
    posterize_material* Main;
    uint ColorTexture;
    uint DepthTexture;
    uint UITexture;
    uint PointSampler;
};
[[vk::push_constant]] DrawRoot Root;

property uniform_material* _Frame { get { return Root.Frame; } }
property posterize_material* _Main { get { return Root.Main; } }
property Texture2D ColorTexture { get { return Tex2DTable[Root.ColorTexture]; } }
property Texture2D DepthTexture { get { return Tex2DTable[Root.DepthTexture]; } }
property Texture2D UITexture { get { return Tex2DTable[Root.UITexture]; } }
property SamplerState PointSampler { get { return SamplerTable[Root.PointSampler]; } }

struct PixelOutput {
    float3 PosterizeOutput : SV_Target0;
};

// endregion
/*========================================================*/

#include "core/fullscreen-vertex.hlsl"
#include "core/BeFunctions.hlsli"
#include "dither.hlsli"

PixelOutput PS(FullscreenVSOutput input) {
    if (_Main.Enabled < 0.5) {
        float3 scene = ColorTexture.SampleLevel(PointSampler, input.UV, 0).rgb;
        float4 ui = UITexture.SampleLevel(PointSampler, input.UV, 0);
        PixelOutput passthrough;
        passthrough.PosterizeOutput = lerp(scene, ui.rgb, saturate(ui.a));
        return passthrough;
    }

    uint w, h;
    ColorTexture.GetDimensions(w, h);

    DitherBlock block = GetDitherBlock(input.UV, _Main.PixelSize, float2(w, h));
    float2 snappedUV = block.SnappedUV;

    float3 color = saturate(ColorTexture.SampleLevel(PointSampler, snappedUV, 0).rgb);

    float rawDepth = DepthTexture.SampleLevel(PointSampler, snappedUV, 0).r;
    float3 worldPos = ReconstructWorldPosition(snappedUV, rawDepth, _Frame.CameraInverseProjectionView);
    float dist = length(worldPos - _Frame.CameraPosition);
    float fog = saturate((dist - _Main.FogStart) / (_Main.FogEnd - _Main.FogStart));
    color = lerp(color, _Main.FogColor, fog);

    int paletteCount = int(_Main.PaletteCount);
    float3 best = _Main.Palette[0];
    float3 second = _Main.Palette[0];
    float bestDist = 1e9;
    float secondDist = 1e9;
    [unroll]
    for (int i = 0; i < 8; i++) {
        if (i >= paletteCount) break;
        float3 delta = color - _Main.Palette[i];
        float dist = dot(delta, delta);
        if (dist < bestDist) {
            secondDist = bestDist;
            second = best;
            bestDist = dist;
            best = _Main.Palette[i];
        } else if (dist < secondDist) {
            secondDist = dist;
            second = _Main.Palette[i];
        }
    }

    float3 axis = second - best;
    float denom = dot(axis, axis);
    float t = denom > 1e-6 ? saturate(dot(color - best, axis) / denom) : 0.0;

    float strength = saturate(_Main.DitherSpread);
    float dithered = saturate(0.5 + (t - 0.5) / max(strength, 1e-4));

    float3 sceneOut = (block.Threshold < dithered) ? second : best;

    float4 ui = UITexture.SampleLevel(PointSampler, input.UV, 0);
    PixelOutput output;
    output.PosterizeOutput = lerp(sceneOut, ui.rgb, saturate(ui.a));
    return output;
}
