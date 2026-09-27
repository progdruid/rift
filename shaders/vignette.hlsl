/*

@be-material: vignette-material {
    ColorTexture: texture2d = white
    PointSampler: sampler = point-clamp
    PixelSize: float = 4.0
    Radius: float = 2.0
    Softness: float = 0.4
    Color: float3 = #1F2C47
}

@be-shader vignette {
    topology triangle-strip
    rasterizer back-solid
    blend disable
    depth disable

    vertex FullscreenVertexKernel
    pixel PS

    bind s0 frame uniform-material
    bind s1 main vignette-material

    target s0 VignetteOutput float3
}

*/

/*========================================================*/
// region @be-auto-boilerplate
#include "core/be-bindless-tables.hlsl"
#include "core/uniform-material.hlsl"

struct vignette_material {
    float PixelSize;
    float Radius;
    float Softness;
    float3 Color;
};

struct DrawRoot {
    uniform_material* Frame;
    vignette_material* Main;
    uint ColorTexture;
    uint PointSampler;
};
[[vk::push_constant]] DrawRoot Root;

property uniform_material* _Frame { get { return Root.Frame; } }
property vignette_material* _Main { get { return Root.Main; } }
property Texture2D ColorTexture { get { return Tex2DTable[Root.ColorTexture]; } }
property SamplerState PointSampler { get { return SamplerTable[Root.PointSampler]; } }

struct PixelOutput {
    float3 VignetteOutput : SV_Target0;
};

// endregion
/*========================================================*/

#include "core/fullscreen-vertex.hlsl"
#include "dither.hlsli"

PixelOutput PS(FullscreenVSOutput input) {
    uint w, h;
    ColorTexture.GetDimensions(w, h);

    float3 color = ColorTexture.SampleLevel(PointSampler, input.UV, 0).rgb;
    DitherBlock block = GetDitherBlock(input.UV, _Main.PixelSize, float2(w, h));

    // dithered vignette: per-block coverage ramps from 0 at Radius to 1 at Radius + Softness
    // radius measured in half-widths (screen side edge = 1.0), so the shape stays circular
    float2 vp = (block.SnappedUV - 0.5) * 2.0 * float2(1.0, float(h) / float(w));
    float vignette = saturate((length(vp) - _Main.Radius) / max(_Main.Softness, 1e-4));
    if (block.Threshold < vignette) color = _Main.Color;

    PixelOutput output;
    output.VignetteOutput = color;
    return output;
}
