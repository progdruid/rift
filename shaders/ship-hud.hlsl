/*

@be-material: ship-hud-material {
    Enabled: float = 1.0
    ScreenSize: float2 = (1280.0, 720.0)
    AimOffset: float2 = (0.0, 0.0)
    AimRadius: float = 150.0
    PixelSize: float = 4.0
    LineHalf: float = 0.0
    PipHalf: float = 0.0
    BracketOffset: float = 6.0
    BracketArm: float = 3.0
    WingTickLength: float = 3.0
    GroundTickLength: float = 2.0
    AimBoxHalf: float = 1.0
    DashPeriod: float = 2.0
    Color: float3 = (0.93, 0.91, 0.84)
    
    TargetPos: float2 = (0.0, 0.0)
    TargetDir: float2 = (0.0, 1.0)
    TargetState: float = 0.0
    TargetRingRadius: float = 5.0
    TargetArrowSize: float = 4.0
    TargetAlpha: float = 1.0
    HorizonDir: float2 = (1.0, 0.0)
    WingTickAlpha: float = 1.0
    WingTickOffset: float = 0.0
}

@be-shader ship-hud {
    topology triangle-strip
    rasterizer back-solid
    blend disable
    depth disable

    vertex FullscreenVertexKernel
    pixel PS

    bind s0 frame uniform-material
    bind s1 main ship-hud-material

    target s0 HudOutput float4
}

*/

/*========================================================*/
// region @be-auto-boilerplate
#include "core/be-bindless-tables.hlsl"
#include "core/uniform-material.hlsl"

struct ship_hud_material {
    float Enabled;
    float2 ScreenSize;
    float2 AimOffset;
    float AimRadius;
    float PixelSize;
    float LineHalf;
    float PipHalf;
    float BracketOffset;
    float BracketArm;
    float WingTickLength;
    float GroundTickLength;
    float AimBoxHalf;
    float DashPeriod;
    float3 Color;
    float2 TargetPos;
    float2 TargetDir;
    float TargetState;
    float TargetRingRadius;
    float TargetArrowSize;
    float TargetAlpha;
    float2 HorizonDir;
    float WingTickAlpha;
    float WingTickOffset;
};

struct DrawRoot {
    uniform_material* Frame;
    ship_hud_material* Main;
};
[[vk::push_constant]] DrawRoot Root;

property uniform_material* _Frame { get { return Root.Frame; } }
property ship_hud_material* _Main { get { return Root.Main; } }

struct PixelOutput {
    float4 HudOutput : SV_Target0;
};

// endregion
/*========================================================*/

#include "core/fullscreen-vertex.hlsl"

static const int TARGET_HIDDEN = 0;
static const int TARGET_ON_SCREEN = 1;
static const int TARGET_OFF_SCREEN = 2;

PixelOutput PS(FullscreenVSOutput input) {
    // cell space: everything below is in whole cells, offset from the center cell
    float pixelSize = _Main.PixelSize;
    float lineHalf = _Main.LineHalf;
    float lineReach = lineHalf + 0.5;

    float2 screenCenter = _Main.ScreenSize * 0.5;
    float2 centerCell = floor(screenCenter / pixelSize);
    float2 p = floor(input.Position.xy / pixelSize) - centerCell;
    float2 absP = abs(p);

    float2 aimCell = floor((screenCenter + _Main.AimOffset * _Main.AimRadius) / pixelSize) - centerCell;
    float2 targetCell = floor(_Main.TargetPos / pixelSize) - centerCell;
    float wingRadius = _Main.AimRadius / pixelSize + round(_Main.WingTickOffset / pixelSize);

    float hit = 0.0;


    // center pip
    if (max(absP.x, absP.y) <= _Main.PipHalf) hit = max(hit, 1.0);


    // boresight brackets
    float bracketOffset = _Main.BracketOffset;
    float bracketInner = bracketOffset - _Main.BracketArm;
    float bracketOuter = bracketOffset + lineHalf;
    bool horizontalArm = abs(absP.y - bracketOffset) <= lineReach && absP.x <= bracketOuter && absP.x >= bracketInner;
    bool verticalArm = abs(absP.x - bracketOffset) <= lineReach && absP.y <= bracketOuter && absP.y >= bracketInner;
    if (horizontalArm || verticalArm) hit = max(hit, 1.0);


    // wing ticks
    float2 horizonDir = _Main.HorizonDir;
    float2 groundDir = float2(-horizonDir.y, horizonDir.x);
    float alongHorizon = dot(p, horizonDir);
    float towardGround = dot(p, groundDir);
    float wingAlpha = _Main.WingTickAlpha;
    if (abs(abs(alongHorizon) - wingRadius) <= _Main.WingTickLength && abs(towardGround) <= lineReach) hit = max(hit, wingAlpha);


    // ground ticks
    float wingInnerEnd = wingRadius - _Main.WingTickLength + 0.5;
    bool groundTickSpan = towardGround >= 0.5 && towardGround <= _Main.GroundTickLength + 0.5;
    if (abs(abs(alongHorizon) - wingInnerEnd) <= lineReach && groundTickSpan) hit = max(hit, wingAlpha);


    // aim box
    float2 absFromAim = abs(p - aimCell);
    if (max(absFromAim.x, absFromAim.y) <= _Main.AimBoxHalf) hit = max(hit, 1.0);


    // aim leader
    float leaderGap = 1.0;
    float leaderLength = length(aimCell);
    float leaderT = saturate(dot(p, aimCell) / max(dot(aimCell, aimCell), 1e-4));
    float leaderAlong = leaderT * leaderLength;
    float leaderDistance = length(p - aimCell * leaderT);
    bool leaderSpan = leaderAlong > bracketOuter + leaderGap && leaderAlong < leaderLength - (_Main.AimBoxHalf + leaderGap);
    bool leaderDash = fmod(floor(leaderAlong / _Main.DashPeriod), 2.0) < 0.5;
    if (leaderDistance <= lineReach && leaderSpan && leaderDash) hit = max(hit, 1.0);


    // target marker
    int targetState = int(round(_Main.TargetState));
    float2 fromTarget = p - targetCell;

    if (targetState == TARGET_ON_SCREEN) {
        float diamond = abs(fromTarget.x) + abs(fromTarget.y);
        if (abs(diamond - _Main.TargetRingRadius) <= lineReach) hit = max(hit, _Main.TargetAlpha);
    }

    if (targetState == TARGET_OFF_SCREEN) {
        float arrowSize = _Main.TargetArrowSize;
        float chevronTail = 0.5;
        float chevronTaper = 1.5;
        float chevronWidth = 0.8;
        float2 forward = _Main.TargetDir;
        float2 side = float2(-forward.y, forward.x);
        float alongArrow = dot(fromTarget, forward);
        float acrossArrow = dot(fromTarget, side);
        float taper = saturate((arrowSize - alongArrow) / (arrowSize * chevronTaper));
        bool arrowSpan = alongArrow <= arrowSize && alongArrow >= -arrowSize * chevronTail;
        if (arrowSpan && abs(acrossArrow) <= taper * arrowSize * chevronWidth) hit = max(hit, 1.0);
    }


    PixelOutput output;
    output.HudOutput = float4(_Main.Color, hit * _Main.Enabled);
    return output;
}
