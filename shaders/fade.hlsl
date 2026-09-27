/*

@be-material: fade-material {
    ColorTexture: texture2d = white
    PointSampler: sampler = point-clamp
    Fade: float = 0.0
}

@be-shader fade {
    topology triangle-strip
    rasterizer back-solid
    blend disable
    depth disable

    vertex FullscreenVertexKernel
    pixel PS

    bind s0 frame uniform-material
    bind s1 main fade-material

    target s0 FadeOutput float3
}

*/

/*========================================================*/
// region @be-auto-boilerplate
#include "core/be-bindless-tables.hlsl"
#include "core/uniform-material.hlsl"

struct fade_material {
    float Fade;
};

struct DrawRoot {
    uniform_material* Frame;
    fade_material* Main;
    uint ColorTexture;
    uint PointSampler;
};
[[vk::push_constant]] DrawRoot Root;

property uniform_material* _Frame { get { return Root.Frame; } }
property fade_material* _Main { get { return Root.Main; } }
property Texture2D ColorTexture { get { return Tex2DTable[Root.ColorTexture]; } }
property SamplerState PointSampler { get { return SamplerTable[Root.PointSampler]; } }

struct PixelOutput {
    float3 FadeOutput : SV_Target0;
};

// endregion
/*========================================================*/

#include "core/fullscreen-vertex.hlsl"

PixelOutput PS(FullscreenVSOutput input) {
    float3 color = ColorTexture.SampleLevel(PointSampler, input.UV, 0).rgb;

    PixelOutput output;
    output.FadeOutput = color * (1.0 - _Main.Fade);
    return output;
}
