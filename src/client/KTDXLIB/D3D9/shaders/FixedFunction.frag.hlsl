// D3D9's fixed-function texture stages, fog and alpha test, for three stages (decision T38). Stage states arrive as
// uniforms with D3D9's values (D3DTOP_*, D3DTA_*, D3DCMP_*), so one shader serves every combination. A stage with no
// texture samples a white texel (KTDXLIB binds a 1x1 white texture), so MODULATE with no texture gives the diffuse
// color, as D3D9 does. Fog blends with the vertex's fog factor (an effect's oFog; the fixed-function vertex shader
// gives 1). Not emulated: D3DTA_SPECULAR (0), D3DTA_TEMP and D3DTSS_RESULTARG (always CURRENT), bump mapping, pixel
// (table) fog.

Texture2D<float4> texture0 : register(t0, space2);
SamplerState sampler0 : register(s0, space2);
Texture2D<float4> texture1 : register(t1, space2);
SamplerState sampler1 : register(s1, space2);
Texture2D<float4> texture2 : register(t2, space2);
SamplerState sampler2 : register(s2, space2);

cbuffer FragmentUniforms : register(b0, space3)
{
    int4 colorStage[3];     // D3DTSS_COLOROP, COLORARG1, COLORARG2, COLORARG0
    int4 alphaStage[3];     // D3DTSS_ALPHAOP, ALPHAARG1, ALPHAARG2, ALPHAARG0
    int4 texCoordIndex;     // D3DTSS_TEXCOORDINDEX of stages 0 to 2 (0 to 2)
    float4 textureFactor;   // D3DRS_TEXTUREFACTOR
    float4 alphaTest;       // D3DRS_ALPHAFUNC (D3DCMP_ALWAYS when the test is off), D3DRS_ALPHAREF, alpha-to-coverage
    float4 fogColor;        // D3DRS_FOGCOLOR; w: 1 with D3DRS_FOGENABLE, else 0
};

struct Input
{
    float4 diffuse : TEXCOORD0;
    float4 uv : TEXCOORD1;      // xy: set 0, zw: set 1
    float4 extra : TEXCOORD2;   // xy: set 2, z: fog factor
};

float2 TexCoords(int set, Input input)
{
    return set == 0 ? input.uv.xy : set == 1 ? input.uv.zw : input.extra.xy;
}

static const int TOP_DISABLE = 1;

float4 Argument(int argument, float4 current, float4 diffuse, float4 texel)
{
    float4 value;
    switch (argument & 0xF) {
    case 0: value = diffuse; break;                 // D3DTA_DIFFUSE
    case 2: value = texel; break;                   // D3DTA_TEXTURE
    case 3: value = textureFactor; break;           // D3DTA_TFACTOR
    case 4: value = float4(0.0, 0.0, 0.0, 0.0); break; // D3DTA_SPECULAR
    default: value = current; break;                // D3DTA_CURRENT, D3DTA_TEMP
    }
    if ((argument & 0x20) != 0) {                   // D3DTA_ALPHAREPLICATE
        value = value.aaaa;
    }
    if ((argument & 0x10) != 0) {                   // D3DTA_COMPLEMENT
        value = 1.0 - value;
    }
    return value;
}

float4 Operation(int op, float4 a1, float4 a2, float4 a0, float4 current, float4 diffuse, float4 texel)
{
    switch (op) {
    case 2: return a1;                                          // SELECTARG1
    case 3: return a2;                                          // SELECTARG2
    case 4: return a1 * a2;                                     // MODULATE
    case 5: return a1 * a2 * 2.0;                               // MODULATE2X
    case 6: return a1 * a2 * 4.0;                               // MODULATE4X
    case 7: return a1 + a2;                                     // ADD
    case 8: return a1 + a2 - 0.5;                               // ADDSIGNED
    case 9: return (a1 + a2 - 0.5) * 2.0;                       // ADDSIGNED2X
    case 10: return a1 - a2;                                    // SUBTRACT
    case 11: return a1 + a2 - a1 * a2;                          // ADDSMOOTH
    case 12: return lerp(a2, a1, diffuse.a);                    // BLENDDIFFUSEALPHA
    case 13: return lerp(a2, a1, texel.a);                      // BLENDTEXTUREALPHA
    case 14: return lerp(a2, a1, textureFactor.a);              // BLENDFACTORALPHA
    case 15: return a1 + a2 * (1.0 - texel.a);                  // BLENDTEXTUREALPHAPM
    case 16: return lerp(a2, a1, current.a);                    // BLENDCURRENTALPHA
    case 18: return float4(a1.rgb + a1.a * a2.rgb, a1.a);       // MODULATEALPHA_ADDCOLOR
    case 19: return float4(a1.rgb * a2.rgb + a1.a, a1.a);       // MODULATECOLOR_ADDALPHA
    case 20: return float4((1.0 - a1.a) * a2.rgb + a1.rgb, a1.a); // MODULATEINVALPHA_ADDCOLOR
    case 21: return float4((1.0 - a1.rgb) * a2.rgb + a1.a, a1.a); // MODULATEINVCOLOR_ADDALPHA
    case 24: return saturate(dot(a1.rgb - 0.5, a2.rgb - 0.5) * 4.0).xxxx; // DOTPRODUCT3
    case 25: return a0 + a1 * a2;                               // MULTIPLYADD
    case 26: return lerp(a2, a1, a0);                           // LERP
    default: return a1 * a2;
    }
}

float4 Stage(int stage, float4 current, float4 diffuse, float4 texel)
{
    const int4 c = colorStage[stage];
    const int4 a = alphaStage[stage];
    const float4 color = Operation(c.x, Argument(c.y, current, diffuse, texel), Argument(c.z, current, diffuse, texel),
                                   Argument(c.w, current, diffuse, texel), current, diffuse, texel);
    float alpha = current.a;
    if (a.x != TOP_DISABLE) {
        alpha = Operation(a.x, Argument(a.y, current, diffuse, texel), Argument(a.z, current, diffuse, texel),
                          Argument(a.w, current, diffuse, texel), current, diffuse, texel).a;
    }
    return saturate(float4(color.rgb, alpha));
}

bool AlphaTest(int func, float alpha, float reference)
{
    // D3D9 compares 8-bit values.
    const float value = floor(saturate(alpha) * 255.0 + 0.5);
    switch (func) {
    case 1: return false;                   // D3DCMP_NEVER
    case 2: return value < reference;       // LESS
    case 3: return value == reference;      // EQUAL
    case 4: return value <= reference;      // LESSEQUAL
    case 5: return value > reference;       // GREATER
    case 6: return value != reference;      // NOTEQUAL
    case 7: return value >= reference;      // GREATEREQUAL
    default: return true;                   // ALWAYS
    }
}

float4 main(Input input) : SV_Target0
{
    float4 current = input.diffuse;
    // A disabled stage disables the ones after it.
    if (colorStage[0].x != TOP_DISABLE) {
        const float4 texel0 = texture0.Sample(sampler0, TexCoords(texCoordIndex.x, input));
        current = Stage(0, current, input.diffuse, texel0);
        if (colorStage[1].x != TOP_DISABLE) {
            const float4 texel1 = texture1.Sample(sampler1, TexCoords(texCoordIndex.y, input));
            current = Stage(1, current, input.diffuse, texel1);
            if (colorStage[2].x != TOP_DISABLE) {
                const float4 texel2 = texture2.Sample(sampler2, TexCoords(texCoordIndex.z, input));
                current = Stage(2, current, input.diffuse, texel2);
            }
        }
    }
    if (alphaTest.z != 0.0) {
        // Alpha-to-coverage (anti-aliasing, opaque draws with a GREATER or GREATEREQUAL test): instead of cutting at
        // the reference, the alpha becomes the share of samples covered across about one pixel around it, so cut-out
        // edges (leaves, grass, fences) are smoothed like geometry edges. Blending is off, so only the coverage reads it.
        const float value = saturate(current.a) * 255.0;
        current.a = saturate((value - alphaTest.y) / max(fwidth(value), 0.0001) + 0.5);
    } else if (!AlphaTest(int(alphaTest.x), current.a, alphaTest.y)) {
        discard;
    }
    if (fogColor.w != 0.0) {
        current.rgb = lerp(fogColor.rgb, current.rgb, saturate(input.extra.z));
    }
    return current;
}
