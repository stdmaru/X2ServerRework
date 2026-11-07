#pragma once

// KTDXLIB's effects (decision T37): ID3DXEffect as the engine calls it, on descriptions written by hand from the
// client's .fx files instead of the .fx themselves (they stay in the .kom, unread). A technique's passes are lists of
// D3D9 states and a vertex shader array; parameters live in the vertex shader constant registers the effect's HLSL
// reads (shaders/XRenderer.vert.hlsl), or in the effect when only states use them (the shader index, fog).
//
// As D3DX did, an effect applies a pass's states at BeginPass through its state manager (CKTDGEffectStateManager),
// or the device when there is none, and writes its registers with SetVertexShaderConstantF; CommitChanges applies what
// changed since: the registers set, and the states that depend on parameters (the shader of g_iShaderIndex, fog).
// Parameters set without CommitChanges reach the device at the next BeginPass. See docs/porting/client-d3d9.md.

#include "D3DXTypes.h"
#include "Device.h"

#include <array>
#include <memory>
#include <string>
#include <vector>

namespace x2::d3d9 {

struct EffectState {
    enum Kind : std::uint8_t { Render, Stage, Sampler };
    Kind kind;
    DWORD stage;    // texture stage or sampler (Render: 0)
    DWORD type;     // D3DRS_*, D3DTSS_*, D3DSAMP_*
    DWORD value;
};

// A vertex shader array of the .fx (VertexShader = vsArray[g_iShaderIndex]): 5 entries are bones 0-4; 10 are bones
// 0-4 without fog, then with it.
struct EffectShaderArray {
    EffectProgram::Kind kind;
    int axis;
    int count;
};

struct EffectPass {
    std::vector<EffectState> states;
    int shaderArray;    // index into EffectDescription::shaderArrays
    bool fog;           // FogEnable = (fog bool), FogVertexMode = Linear, FogColor = (fog color)
};

struct EffectTechnique {
    std::string name;
    std::vector<EffectPass> passes;
};

struct EffectParameter {
    enum Type : std::uint8_t { Bool, Int, Float, Vector, Matrix };
    std::string name;
    Type type;
    int reg;            // first register; -1 for a parameter only states use
    int component;      // Float: the component of the register
    int elements;       // matrices: the array's size
};

struct EffectDescription {
    std::vector<EffectParameter> parameters;
    std::vector<EffectTechnique> techniques;
    std::vector<EffectShaderArray> shaderArrays;
    std::string shaderIndex;    // the int parameter that selects the shader of the arrays
    std::string fogEnable;      // the bool parameter of FogEnable
    std::string fogColor;       // the vector parameter of FogColor
};

class Effect final : public ID3DXEffect {
public:
    Effect(Device& device, const EffectDescription& description);
    ~Effect() override;

    ULONG AddRef() override { return ++m_references; }
    ULONG Release() override;
    D3DXHANDLE GetParameterByName(D3DXHANDLE parent, LPCSTR name) override;
    D3DXHANDLE GetTechniqueByName(LPCSTR name) override;
    HRESULT SetBool(D3DXHANDLE parameter, BOOL b) override;
    HRESULT SetInt(D3DXHANDLE parameter, INT n) override;
    HRESULT SetFloat(D3DXHANDLE parameter, FLOAT f) override;
    HRESULT SetVector(D3DXHANDLE parameter, const D3DXVECTOR4* vector) override;
    HRESULT SetMatrix(D3DXHANDLE parameter, const D3DXMATRIX* matrix) override;
    HRESULT SetMatrixArray(D3DXHANDLE parameter, const D3DXMATRIX* matrices, UINT count) override;
    HRESULT SetTexture(D3DXHANDLE parameter, Texture* texture) override;
    HRESULT SetTechnique(D3DXHANDLE technique) override;
    HRESULT Begin(UINT* passes, DWORD flags) override;
    HRESULT BeginPass(UINT pass) override;
    HRESULT CommitChanges() override;
    HRESULT EndPass() override;
    HRESULT End() override;
    HRESULT OnLostDevice() override { return D3D_OK; }
    HRESULT OnResetDevice() override { return D3D_OK; }
    // The manager is not referenced: its owner releases it (X2.cpp), and D3DX's reference would delete it twice.
    HRESULT SetStateManager(ID3DXEffectStateManager* manager) override;

    // Tests: the registers as the effect holds them, and the vertex shader of an array entry.
    const float* Registers() const { return m_registers.data(); }
    VertexShader* Shader(int array, int index) const;

private:
    struct Value {
        std::array<float, 4> vector{};      // Bool/Int: x
    };

    int Find(D3DXHANDLE parameter) const;
    int FindTechnique(D3DXHANDLE technique) const;
    void Dirty(int first, int count);
    void ApplyState(const EffectState& state);
    bool ApplyParameterStates(const EffectPass& pass);
    void WriteRegisters(int first, int count);

    Device& m_device;
    const EffectDescription& m_description;
    ULONG m_references = 1;
    ID3DXEffectStateManager* m_manager = nullptr;
    std::vector<Value> m_values;                  // by parameter
    std::vector<float> m_registers = std::vector<float>(kEffectRegisters * 4, 0.0f);
    int m_dirtyFirst = 0, m_dirtyEnd = 0;         // registers set since they were last written
    std::vector<std::vector<std::unique_ptr<VertexShader>>> m_shaders;   // by array, then index
    int m_technique = 0;
    bool m_begun = false;
    int m_pass = -1;
    int m_shaderIndex = -1, m_fogEnable = -1, m_fogColor = -1;     // parameters, by index
};

// XRenderer.fx (CKTDGXRenderer): its 20 techniques and 19 parameters, as written in shaders/XRenderer.vert.hlsl.
const EffectDescription& XRendererEffect();

// What replaces D3DXCreateEffect(XRenderer.fx): the effect on the device, with one reference.
HRESULT CreateXRendererEffect(Device* device, LPD3DXEFFECT* effect);

}  // namespace x2::d3d9
