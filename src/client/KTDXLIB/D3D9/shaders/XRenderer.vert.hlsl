// The vertex shaders of the client's XRenderer.fx effect (CKTDGXRenderer), rewritten by hand (decision T37): the
// .fx's 80 vs_1_1 shaders are one program here. Which one runs is the Program block, set by the device from the
// effect's x2::d3d9::VertexShader: the kind (real color, cartoon, black or color edge, light flow in world or local
// space), the bones blended (0: none; 1-4 as the .fx's iNumBones), fog, and the light flow's axis. The pixel stage
// stays fixed function (FixedFunction.frag.hlsl), as in the .fx.
//
// Registers is the .fx's constants as D3DX would load them: x2::d3d9::Effect writes them through
// SetVertexShaderConstantF (CKTDGEffectStateManager caches and flushes them), one float4 per register. Matrices are
// D3DX's rows as they are (row_major), so mul(v, M) is D3D's v * M.
//
// Variants (defines), by what the FVF has: SKINNED 0/1 (blend weights and indices), NORMAL 0/1, TEXCOORD 0/1. Inputs
// the FVF lacks read as D3D9's defaults (0, 0, 0, 1). Input locations: 0 position, 2 first texture coordinates,
// 4 blend weights, 5 blend indices, 6 normal (TEXCOORD<n> is location n, see FixedFunction.vert.hlsl).

#ifndef SKINNED
#define SKINNED 0
#endif
#ifndef NORMAL
#define NORMAL 0
#endif
#ifndef TEXCOORD
#define TEXCOORD 0
#endif

cbuffer Registers : register(b0, space1)
{
    row_major float4x4 g_amWorldPalette[20];    // c0-c79
    row_major float4x4 g_mViewProj;             // c80
    row_major float4x4 g_mWorldViewProj;        // c84
    float4 g_Color;                             // c88
    float4 g_LightPos;                          // c89
    float4 g_OutLineColor;                      // c90
    float4 g_LightFlowPoint;                    // c91
    float4 g_TexOffSet01;                       // c92
    float4 g_fFog;                              // c93: linear fog (x + distance * y), height fog (z + y * w)
    float4 g_fEye;                              // c94
    float4 scalars0;                            // c95: g_fTexOffSet2X, g_fTexOffSet2Y, g_fOutLineWide, g_fLightFlowImpact
    float4 scalars1;                            // c96: g_fLightFlowWide, g_fDensity
};

cbuffer Program : register(b1, space1)
{
    int4 program;           // kind, bones (0-4), fog (0/1), light flow axis (0 x, 1 y, 2 z, 3 point)
    float4 pixelOffset;     // D3D9's pixel centers are half a pixel off: (1 / Width, -1 / Height)
};

static const int KIND_REAL_COLOR = 0;
static const int KIND_CARTOON = 1;
static const int KIND_BLACK_EDGE = 2;
static const int KIND_COLOR_EDGE = 3;
static const int KIND_FLOW_WORLD = 4;
static const int KIND_FLOW_LOCAL = 5;

struct Input
{
    [[vk::location(0)]] float3 position : TEXCOORD0;
#if TEXCOORD
    [[vk::location(2)]] float2 uv : TEXCOORD2;
#endif
#if SKINNED
    [[vk::location(4)]] float3 weights : TEXCOORD4;
    [[vk::location(5)]] uint4 indices : TEXCOORD5;  // the palette slots, one byte each in memory order
#endif
#if NORMAL
    [[vk::location(6)]] float3 normal : TEXCOORD6;
#endif
};

struct Output
{
    float4 diffuse : TEXCOORD0;
    float4 uv : TEXCOORD1;      // xy: stage 0's set, zw: stage 1's
    float4 extra : TEXCOORD2;   // xy: stage 2's set, z: fog factor
    float4 position : SV_Position;
};

float FogFactor(float4 wPos)
{
    const float4 xpos = wPos - g_fEye;
    const float xfog = g_fFog.x + length(xpos) * g_fFog.y;     // linear fog by distance
    const float yfog = g_fFog.z + wPos.y * g_fFog.w;           // height fog
    return max(xfog, yfog) - scalars1.y;
}

// VS_Skin of the .fx: the position and the normal (w = 0) through the palette. One bone takes the first slot with
// weight 1; two and three blend that many weights as they are; four take the last weight as what the others leave.
void Skin(float4 pos, float3 normal, float3 weights, uint4 indices, int bones, out float4 wPos, out float4 wNor)
{
    const float4 nor = float4(normal, 0.0);
    wPos = float4(0.0, 0.0, 0.0, 1.0);
    wNor = float4(0.0, 0.0, 0.0, 0.0);
    if (bones == 1) {
        wPos.xyz = mul(pos, g_amWorldPalette[indices.x]).xyz;
        wNor = mul(nor, g_amWorldPalette[indices.x]);
        return;
    }
    const float w[3] = {weights.x, weights.y, weights.z};
    const uint slot[4] = {indices.x, indices.y, indices.z, indices.w};
    const int blended = bones < 4 ? bones : 3;
    float last = 1.0;
    for (int i = 0; i < blended; ++i) {
        wPos.xyz += mul(pos, g_amWorldPalette[slot[i]]).xyz * w[i];
        wNor += mul(nor, g_amWorldPalette[slot[i]]) * w[i];
        last -= w[i];
    }
    if (bones >= 4) {
        wPos.xyz += mul(pos, g_amWorldPalette[slot[3]]).xyz * last;
        wNor += mul(nor, g_amWorldPalette[slot[3]]) * last;
    }
}

float FlowDistance(float4 p, int axis)
{
    switch (axis) {
    case 0: return abs(p.x - scalars0.w);
    case 1: return abs(p.y - scalars0.w);
    case 2: return abs(p.z - scalars0.w);
    default: return distance(p, g_LightFlowPoint);
    }
}

Output main(Input input)
{
    const int kind = program.x;
    const int bones = program.y;
    const bool fog = program.z != 0;

    const float4 pos = float4(input.position, 1.0);
#if NORMAL
    const float3 normal = input.normal;
#else
    const float3 normal = float3(0.0, 0.0, 0.0);
#endif
#if TEXCOORD
    const float2 uv = input.uv;
#else
    const float2 uv = float2(0.0, 0.0);
#endif

    // World space: blended by the palette (skinned shaders), or the palette's first matrix (the world matrix).
    float4 wPos;
    float4 wNor;
#if SKINNED
    if (bones > 0) {
        Skin(pos, normal, input.weights, input.indices, bones, wPos, wNor);
    } else
#endif
    {
        wPos = mul(pos, g_amWorldPalette[0]);
        wNor = mul(float4(normal, 0.0), g_amWorldPalette[0]);
    }
    const bool skinned = bones > 0;

    Output output;
    output.diffuse = float4(1.0, 1.0, 1.0, 1.0);    // what the .fx's shaders leave unwritten
    output.uv = float4(0.0, 0.0, 0.0, 0.0);
    output.extra = float4(0.0, 0.0, 1.0, 0.0);      // no fog unless the shader computes it

    if (kind == KIND_REAL_COLOR || kind == KIND_CARTOON) {
        output.position = skinned ? mul(wPos, g_mViewProj) : mul(pos, g_mWorldViewProj);
        output.uv.xy = uv + g_TexOffSet01.xy;
        if (kind == KIND_REAL_COLOR) {
            output.uv.zw = uv + g_TexOffSet01.zw;
        } else {
            // The cartoon ramp: stage 1 samples at the lighting term, both coordinates.
            output.uv.zw = dot(normalize(wNor), g_LightPos).xx;
        }
        output.extra.xy = uv + scalars0.xy;
        output.diffuse = g_Color;
    } else if (kind == KIND_BLACK_EDGE || kind == KIND_COLOR_EDGE) {
        // The outline: the vertex pushed out along its normal (not normalized, as in the .fx).
        output.position = mul(wPos + wNor * scalars0.z, g_mViewProj);
        if (kind == KIND_COLOR_EDGE) {
            output.diffuse = g_OutLineColor;
        }
    } else {
        // Light flow: alpha falls off with the distance to a plane or a point, in world or local space.
        const float4 p = kind == KIND_FLOW_WORLD ? wPos : pos;
        output.diffuse = g_Color;
        output.diffuse.w = g_Color.w / (0.00001 + FlowDistance(p, program.w) / scalars1.x);
        output.position = kind == KIND_FLOW_WORLD || skinned ? mul(wPos, g_mViewProj) : mul(pos, g_mWorldViewProj);
    }
    if (fog && kind <= KIND_COLOR_EDGE) {
        output.extra.z = FogFactor(wPos);
    }
    output.position.xy += pixelOffset.xy * output.position.w;
    return output;
}
