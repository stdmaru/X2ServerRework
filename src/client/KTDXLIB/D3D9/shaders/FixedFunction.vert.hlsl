// D3D9's fixed-function vertex stage for the FVFs the client uses (decision T38): position (pre-transformed XYZRHW or
// XYZ through world * view * projection), the D3DCOLOR diffuse and up to two texture coordinate sets. Lighting, fog,
// vertex blending and texture transforms are not emulated (docs/porting/client-d3d9.md).
//
// Variants (defines): POSITION_RHW 0/1, DIFFUSE 0/1, TEXCOORDS 0..2. The input locations are fixed (TEXCOORD<n> is
// location n, as SDL_GPU's D3D12 backend expects): 0 position, 1 diffuse, 2 first and 3 second texture coordinates.

#ifndef POSITION_RHW
#define POSITION_RHW 0
#endif
#ifndef DIFFUSE
#define DIFFUSE 0
#endif
#ifndef TEXCOORDS
#define TEXCOORDS 0
#endif

cbuffer VertexUniforms : register(b0, space1)
{
    row_major float4x4 worldViewProjection;    // D3D row vectors: v * World * View * Projection
    float4 viewport;                           // D3DVIEWPORT9 X, Y, Width, Height
    float4 depthRange;                         // MinZ, 1 / (MaxZ - MinZ)
    float4 pixelOffset;                        // D3D9's pixel centers are half a pixel off: (1 / Width, -1 / Height)
};

struct Input
{
#if POSITION_RHW
    [[vk::location(0)]] float4 position : TEXCOORD0;    // screen x, y, z (0..1), 1 / w
#else
    [[vk::location(0)]] float3 position : TEXCOORD0;
#endif
#if DIFFUSE
    [[vk::location(1)]] float4 diffuse : TEXCOORD1;     // D3DCOLOR's bytes in memory: B, G, R, A
#endif
#if TEXCOORDS >= 1
    [[vk::location(2)]] float2 uv0 : TEXCOORD2;
#endif
#if TEXCOORDS >= 2
    [[vk::location(3)]] float2 uv1 : TEXCOORD3;
#endif
};

struct Output
{
    float4 diffuse : TEXCOORD0;
    float4 uv : TEXCOORD1;      // xy: first set, zw: second set
    float4 position : SV_Position;
};

Output main(Input input)
{
    Output output;
#if POSITION_RHW
    // Back from screen space to clip space: pixel centers move from integers (D3D9) to halves, so +0.5.
    const float w = input.position.w != 0.0 ? 1.0 / input.position.w : 1.0;
    const float2 screen = input.position.xy + 0.5 - viewport.xy;
    const float2 ndc = float2(screen.x / viewport.z * 2.0 - 1.0, 1.0 - screen.y / viewport.w * 2.0);
    const float z = (input.position.z - depthRange.x) * depthRange.y;
    output.position = float4(ndc * w, z * w, w);
#else
    output.position = mul(float4(input.position, 1.0), worldViewProjection);
    output.position.xy += pixelOffset.xy * output.position.w;
#endif
#if DIFFUSE
    output.diffuse = input.diffuse.zyxw;
#else
    output.diffuse = float4(1.0, 1.0, 1.0, 1.0);
#endif
    output.uv = float4(0.0, 0.0, 0.0, 0.0);
#if TEXCOORDS >= 1
    output.uv.xy = input.uv0;
#endif
#if TEXCOORDS >= 2
    output.uv.zw = input.uv1;
#endif
    return output;
}
