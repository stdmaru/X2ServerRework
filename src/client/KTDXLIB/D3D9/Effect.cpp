// KTDXLIB's effects (decision T37). See Effect.h and docs/porting/client-d3d9.md.

#include "Effect.h"

#include <algorithm>
#include <cstring>

namespace x2::d3d9 {

// --- Effect ---------------------------------------------------------------------------------------------------------

Effect::Effect(Device& device, const EffectDescription& description) : m_device(device), m_description(description)
{
    m_values.resize(description.parameters.size());
    for (std::size_t i = 0; i < description.parameters.size(); ++i) {
        const std::string& name = description.parameters[i].name;
        if (name == description.shaderIndex) {
            m_shaderIndex = static_cast<int>(i);
        } else if (name == description.fogEnable) {
            m_fogEnable = static_cast<int>(i);
        } else if (name == description.fogColor) {
            m_fogColor = static_cast<int>(i);
        }
    }
    for (const EffectShaderArray& array : description.shaderArrays) {
        auto& shaders = m_shaders.emplace_back();
        for (int index = 0; index < array.count; ++index) {
            EffectProgram program;
            program.kind = array.kind;
            program.bones = index % 5;
            program.fog = array.count == 10 && index >= 5;
            program.axis = array.axis;
            shaders.push_back(device.CreateVertexShader(program));
        }
    }
}

Effect::~Effect() = default;

ULONG Effect::Release()
{
    const ULONG references = --m_references;
    if (references == 0) {
        delete this;
    }
    return references;
}

int Effect::Find(D3DXHANDLE parameter) const
{
    if (parameter == nullptr) {
        return -1;
    }
    const auto& parameters = m_description.parameters;
    // A handle is the parameter's name as the effect holds it; D3DX takes names as handles too.
    for (std::size_t i = 0; i < parameters.size(); ++i) {
        if (parameters[i].name.c_str() == parameter) {
            return static_cast<int>(i);
        }
    }
    for (std::size_t i = 0; i < parameters.size(); ++i) {
        if (parameters[i].name == parameter) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

int Effect::FindTechnique(D3DXHANDLE technique) const
{
    if (technique == nullptr) {
        return -1;
    }
    const auto& techniques = m_description.techniques;
    for (std::size_t i = 0; i < techniques.size(); ++i) {
        if (techniques[i].name.c_str() == technique) {
            return static_cast<int>(i);
        }
    }
    for (std::size_t i = 0; i < techniques.size(); ++i) {
        if (techniques[i].name == technique) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

D3DXHANDLE Effect::GetParameterByName(D3DXHANDLE parent, LPCSTR name)
{
    if (parent != nullptr || name == nullptr) {
        return nullptr;     // the effects have no structures
    }
    const int index = Find(name);
    return index < 0 ? nullptr : m_description.parameters[static_cast<std::size_t>(index)].name.c_str();
}

D3DXHANDLE Effect::GetTechniqueByName(LPCSTR name)
{
    const int index = FindTechnique(name);
    return index < 0 ? nullptr : m_description.techniques[static_cast<std::size_t>(index)].name.c_str();
}

void Effect::Dirty(int first, int count)
{
    if (m_dirtyFirst >= m_dirtyEnd) {
        m_dirtyFirst = first;
        m_dirtyEnd = first + count;
    } else {
        m_dirtyFirst = std::min(m_dirtyFirst, first);
        m_dirtyEnd = std::max(m_dirtyEnd, first + count);
    }
}

HRESULT Effect::SetBool(D3DXHANDLE parameter, BOOL b)
{
    return SetInt(parameter, b ? 1 : 0);
}

HRESULT Effect::SetInt(D3DXHANDLE parameter, INT n)
{
    const int index = Find(parameter);
    if (index < 0) {
        return D3DERR_INVALIDCALL;
    }
    const EffectParameter& p = m_description.parameters[static_cast<std::size_t>(index)];
    if (p.type == EffectParameter::Bool || p.type == EffectParameter::Int) {
        m_values[static_cast<std::size_t>(index)].vector[0] = static_cast<float>(p.type == EffectParameter::Bool ? (n != 0)
                                                                                                                : n);
        return D3D_OK;
    }
    return SetFloat(parameter, static_cast<float>(n));
}

HRESULT Effect::SetFloat(D3DXHANDLE parameter, FLOAT f)
{
    const int index = Find(parameter);
    if (index < 0) {
        return D3DERR_INVALIDCALL;
    }
    const EffectParameter& p = m_description.parameters[static_cast<std::size_t>(index)];
    if (p.type == EffectParameter::Matrix) {
        return D3DERR_INVALIDCALL;
    }
    if (p.type == EffectParameter::Bool || p.type == EffectParameter::Int) {
        return SetInt(parameter, static_cast<INT>(f));
    }
    const int component = p.type == EffectParameter::Float ? p.component : 0;
    m_values[static_cast<std::size_t>(index)].vector[static_cast<std::size_t>(component)] = f;
    if (p.reg >= 0) {
        m_registers[static_cast<std::size_t>(p.reg * 4 + component)] = f;
        Dirty(p.reg, 1);
    }
    return D3D_OK;
}

HRESULT Effect::SetVector(D3DXHANDLE parameter, const D3DXVECTOR4* vector)
{
    const int index = Find(parameter);
    if (index < 0 || vector == nullptr) {
        return D3DERR_INVALIDCALL;
    }
    const EffectParameter& p = m_description.parameters[static_cast<std::size_t>(index)];
    if (p.type != EffectParameter::Vector) {
        return p.type == EffectParameter::Matrix ? D3DERR_INVALIDCALL : SetFloat(parameter, vector->x);
    }
    m_values[static_cast<std::size_t>(index)].vector = {vector->x, vector->y, vector->z, vector->w};
    if (p.reg >= 0) {
        std::memcpy(&m_registers[static_cast<std::size_t>(p.reg * 4)], &vector->x, 4 * sizeof(float));
        Dirty(p.reg, 1);
    }
    return D3D_OK;
}

HRESULT Effect::SetMatrix(D3DXHANDLE parameter, const D3DXMATRIX* matrix)
{
    return SetMatrixArray(parameter, matrix, 1);
}

HRESULT Effect::SetMatrixArray(D3DXHANDLE parameter, const D3DXMATRIX* matrices, UINT count)
{
    const int index = Find(parameter);
    if (index < 0 || matrices == nullptr) {
        return D3DERR_INVALIDCALL;
    }
    const EffectParameter& p = m_description.parameters[static_cast<std::size_t>(index)];
    if (p.type != EffectParameter::Matrix || count > static_cast<UINT>(p.elements)) {
        return D3DERR_INVALIDCALL;
    }
    // D3DX's rows as they are: the HLSL reads them row_major.
    std::memcpy(&m_registers[static_cast<std::size_t>(p.reg * 4)], matrices, count * sizeof(D3DXMATRIX));
    Dirty(p.reg, static_cast<int>(count) * 4);
    return D3D_OK;
}

HRESULT Effect::SetTexture(D3DXHANDLE /*parameter*/, Texture* /*texture*/)
{
    return D3DERR_INVALIDCALL;  // the effects have no texture parameters
}

HRESULT Effect::SetTechnique(D3DXHANDLE technique)
{
    const int index = FindTechnique(technique);
    if (index < 0 || m_begun) {
        return D3DERR_INVALIDCALL;
    }
    m_technique = index;
    return D3D_OK;
}

HRESULT Effect::SetStateManager(ID3DXEffectStateManager* manager)
{
    m_manager = manager;
    return D3D_OK;
}

HRESULT Effect::Begin(UINT* passes, DWORD /*flags*/)
{
    // The D3DXFX_DONOTSAVE* flags are what the engine passes; nothing is saved without them either (Effect.h).
    if (m_begun) {
        return D3DERR_INVALIDCALL;
    }
    m_begun = true;
    m_pass = -1;
    if (passes != nullptr) {
        *passes = static_cast<UINT>(m_description.techniques[static_cast<std::size_t>(m_technique)].passes.size());
    }
    return D3D_OK;
}

void Effect::ApplyState(const EffectState& state)
{
    switch (state.kind) {
    case EffectState::Render:
        if (m_manager) {
            m_manager->SetRenderState(static_cast<D3DRENDERSTATETYPE>(state.type), state.value);
        } else {
            m_device.SetRenderState(static_cast<D3DRENDERSTATETYPE>(state.type), state.value);
        }
        break;
    case EffectState::Stage:
        if (m_manager) {
            m_manager->SetTextureStageState(state.stage, static_cast<D3DTEXTURESTAGESTATETYPE>(state.type), state.value);
        } else {
            m_device.SetTextureStageState(state.stage, static_cast<D3DTEXTURESTAGESTATETYPE>(state.type), state.value);
        }
        break;
    case EffectState::Sampler:
        if (m_manager) {
            m_manager->SetSamplerState(state.stage, static_cast<D3DSAMPLERSTATETYPE>(state.type), state.value);
        } else {
            m_device.SetSamplerState(state.stage, static_cast<D3DSAMPLERSTATETYPE>(state.type), state.value);
        }
        break;
    }
}

bool Effect::ApplyParameterStates(const EffectPass& pass)
{
    const int index = m_shaderIndex >= 0 ? static_cast<int>(m_values[static_cast<std::size_t>(m_shaderIndex)].vector[0])
                                         : 0;
    const auto& shaders = m_shaders[static_cast<std::size_t>(pass.shaderArray)];
    if (index < 0 || index >= static_cast<int>(shaders.size())) {
        return false;
    }
    VertexShader* shader = shaders[static_cast<std::size_t>(index)].get();
    if (m_manager) {
        m_manager->SetVertexShader(shader);
    } else {
        m_device.SetVertexShader(shader);
    }
    if (pass.fog) {
        const bool enable = m_fogEnable >= 0 && m_values[static_cast<std::size_t>(m_fogEnable)].vector[0] != 0.0f;
        std::array<float, 4> color{};
        if (m_fogColor >= 0) {
            color = m_values[static_cast<std::size_t>(m_fogColor)].vector;
        }
        auto byte = [](float v) { return static_cast<DWORD>(std::clamp(v, 0.0f, 1.0f) * 255.0f + 0.5f); };
        const DWORD argb = (byte(color[3]) << 24) | (byte(color[0]) << 16) | (byte(color[1]) << 8) | byte(color[2]);
        ApplyState({EffectState::Render, 0, D3DRS_FOGENABLE, enable ? DWORD{TRUE} : DWORD{FALSE}});
        ApplyState({EffectState::Render, 0, D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR});
        ApplyState({EffectState::Render, 0, D3DRS_FOGCOLOR, argb});
    }
    return true;
}

void Effect::WriteRegisters(int first, int count)
{
    if (count <= 0) {
        return;
    }
    const float* data = &m_registers[static_cast<std::size_t>(first * 4)];
    if (m_manager) {
        m_manager->SetVertexShaderConstantF(static_cast<UINT>(first), data, static_cast<UINT>(count));
    } else {
        m_device.SetVertexShaderConstantF(static_cast<UINT>(first), data, static_cast<UINT>(count));
    }
}

HRESULT Effect::BeginPass(UINT pass)
{
    const EffectTechnique& technique = m_description.techniques[static_cast<std::size_t>(m_technique)];
    if (!m_begun || m_pass >= 0 || pass >= technique.passes.size()) {
        return D3DERR_INVALIDCALL;
    }
    const EffectPass& p = technique.passes[pass];
    for (const EffectState& state : p.states) {
        ApplyState(state);
    }
    if (!ApplyParameterStates(p)) {
        return D3DERR_INVALIDCALL;
    }
    WriteRegisters(0, static_cast<int>(kEffectRegisters));
    m_dirtyFirst = m_dirtyEnd = 0;
    m_pass = static_cast<int>(pass);
    return D3D_OK;
}

HRESULT Effect::CommitChanges()
{
    if (m_pass < 0) {
        return D3DERR_INVALIDCALL;
    }
    const EffectTechnique& technique = m_description.techniques[static_cast<std::size_t>(m_technique)];
    if (!ApplyParameterStates(technique.passes[static_cast<std::size_t>(m_pass)])) {
        return D3DERR_INVALIDCALL;
    }
    WriteRegisters(m_dirtyFirst, m_dirtyEnd - m_dirtyFirst);
    m_dirtyFirst = m_dirtyEnd = 0;
    return D3D_OK;
}

HRESULT Effect::EndPass()
{
    if (m_pass < 0) {
        return D3DERR_INVALIDCALL;
    }
    m_pass = -1;
    return D3D_OK;
}

HRESULT Effect::End()
{
    if (!m_begun) {
        return D3DERR_INVALIDCALL;
    }
    m_begun = false;
    m_pass = -1;
    return D3D_OK;
}

VertexShader* Effect::Shader(int array, int index) const
{
    if (array < 0 || array >= static_cast<int>(m_shaders.size()) || index < 0 ||
        index >= static_cast<int>(m_shaders[static_cast<std::size_t>(array)].size())) {
        return nullptr;
    }
    return m_shaders[static_cast<std::size_t>(array)][static_cast<std::size_t>(index)].get();
}

// --- XRenderer.fx ---------------------------------------------------------------------------------------------------

namespace {

EffectState RS(D3DRENDERSTATETYPE type, DWORD value)
{
    return {EffectState::Render, 0, static_cast<DWORD>(type), value};
}

EffectState TS(DWORD stage, D3DTEXTURESTAGESTATETYPE type, DWORD value)
{
    return {EffectState::Stage, stage, static_cast<DWORD>(type), value};
}

EffectState SS(DWORD sampler, D3DSAMPLERSTATETYPE type, DWORD value)
{
    return {EffectState::Sampler, sampler, static_cast<DWORD>(type), value};
}

// The .fx's vertex shader arrays, in this order.
enum ShaderArray : int {
    kRealColor, kCartoon, kBlackEdge, kColorEdge,
    kFlowWorldX, kFlowWorldY, kFlowWorldZ, kFlowWorldPoint,
    kFlowLocalX, kFlowLocalY, kFlowLocalZ, kFlowLocalPoint,
};

// The cartoon techniques' first pass: the texture by the diffuse color, then the cartoon ramp (stage 1, its u
// clamped) modulated or added. The color edge techniques leave stage 2's alpha as it is.
std::vector<EffectState> CartoonStates(D3DTEXTUREOP rampOp, bool stage2Alpha)
{
    std::vector<EffectState> states = {
        TS(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE),   TS(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE),
        TS(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE),   TS(1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1),
        TS(1, D3DTSS_ALPHAARG1, D3DTA_CURRENT),   TS(0, D3DTSS_COLOROP, D3DTOP_MODULATE),
        TS(0, D3DTSS_COLORARG1, D3DTA_TEXTURE),   TS(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE),
        TS(1, D3DTSS_COLOROP, static_cast<DWORD>(rampOp)), TS(1, D3DTSS_COLORARG1, D3DTA_CURRENT),
        TS(1, D3DTSS_COLORARG2, D3DTA_TEXTURE),   SS(1, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP),
    };
    if (stage2Alpha) {
        states.push_back(TS(2, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1));
        states.push_back(TS(2, D3DTSS_ALPHAARG1, D3DTA_CURRENT));
    }
    states.push_back(TS(2, D3DTSS_COLORARG1, D3DTA_CURRENT));
    states.push_back(TS(2, D3DTSS_COLORARG2, D3DTA_TEXTURE));
    return states;
}

// The outline in the texture factor's black, culling front faces. A second pass also disables stages 1 and 2.
std::vector<EffectState> BlackEdgeStates(bool disableStages)
{
    std::vector<EffectState> states = {
        RS(D3DRS_TEXTUREFACTOR, 0xFF000000),     TS(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1),
        TS(0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR),  TS(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1),
        TS(0, D3DTSS_COLORARG1, D3DTA_TFACTOR),
    };
    if (disableStages) {
        states.insert(states.end(), {TS(1, D3DTSS_COLOROP, D3DTOP_DISABLE), TS(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE),
                                     TS(2, D3DTSS_COLOROP, D3DTOP_DISABLE), TS(2, D3DTSS_ALPHAOP, D3DTOP_DISABLE)});
    }
    states.push_back(RS(D3DRS_CULLMODE, D3DCULL_CW));
    return states;
}

// The outline in the vertex color (g_OutLineColor).
std::vector<EffectState> ColorEdgeStates(bool disableStages)
{
    std::vector<EffectState> states = {
        TS(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1), TS(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1),
        TS(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE),   TS(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE),
    };
    if (disableStages) {
        states.insert(states.end(), {TS(1, D3DTSS_COLOROP, D3DTOP_DISABLE), TS(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE),
                                     TS(2, D3DTSS_COLOROP, D3DTOP_DISABLE), TS(2, D3DTSS_ALPHAOP, D3DTOP_DISABLE)});
    }
    states.push_back(RS(D3DRS_CULLMODE, D3DCULL_CW));
    return states;
}

// Light flow: the vertex alpha added to the current one, blended onto the target by its alpha.
std::vector<EffectState> LightFlowStates()
{
    return {
        RS(D3DRS_ALPHABLENDENABLE, TRUE),       RS(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA),
        RS(D3DRS_DESTBLEND, D3DBLEND_DESTALPHA), TS(0, D3DTSS_ALPHAOP, D3DTOP_ADD),
        TS(0, D3DTSS_ALPHAARG1, D3DTA_CURRENT),  TS(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE),
        TS(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE),
    };
}

EffectDescription MakeXRenderer()
{
    EffectDescription d;
    using P = EffectParameter;
    // Registers as XRenderer.vert.hlsl's Registers block lays them out.
    d.parameters = {
        {"g_amWorldPalette", P::Matrix, 0, 0, 20},
        {"g_mViewProj", P::Matrix, 80, 0, 1},
        {"g_mWorldViewProj", P::Matrix, 84, 0, 1},
        {"g_Color", P::Vector, 88, 0, 1},
        {"g_LightPos", P::Vector, 89, 0, 1},
        {"g_OutLineColor", P::Vector, 90, 0, 1},
        {"g_LightFlowPoint", P::Vector, 91, 0, 1},
        {"g_TexOffSet01", P::Vector, 92, 0, 1},
        {"g_fFog", P::Vector, 93, 0, 1},
        {"g_fEye", P::Vector, 94, 0, 1},
        {"g_fTexOffSet2X", P::Float, 95, 0, 1},
        {"g_fTexOffSet2Y", P::Float, 95, 1, 1},
        {"g_fOutLineWide", P::Float, 95, 2, 1},
        {"g_fLightFlowImpact", P::Float, 95, 3, 1},
        {"g_fLightFlowWide", P::Float, 96, 0, 1},
        {"g_fDensity", P::Float, 96, 1, 1},
        {"g_fColor", P::Vector, -1, 0, 1},
        {"g_bFog", P::Bool, -1, 0, 1},
        {"g_iShaderIndex", P::Int, -1, 0, 1},
    };
    d.shaderIndex = "g_iShaderIndex";
    d.fogEnable = "g_bFog";
    d.fogColor = "g_fColor";
    using K = EffectProgram;
    d.shaderArrays = {
        {K::RealColor, 0, 10}, {K::Cartoon, 0, 10}, {K::BlackEdge, 0, 10}, {K::ColorEdge, 0, 10},
        {K::FlowWorld, 0, 5},  {K::FlowWorld, 1, 5}, {K::FlowWorld, 2, 5}, {K::FlowWorld, 3, 5},
        {K::FlowLocal, 0, 5},  {K::FlowLocal, 1, 5}, {K::FlowLocal, 2, 5}, {K::FlowLocal, 3, 5},
    };

    const std::vector<EffectState> realColor = {
        TS(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE),   TS(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE),
        TS(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE),   TS(0, D3DTSS_COLOROP, D3DTOP_MODULATE),
        TS(0, D3DTSS_COLORARG1, D3DTA_TEXTURE),   TS(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE),
        TS(1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1), TS(1, D3DTSS_ALPHAARG1, D3DTA_CURRENT),
        // Stage 1 passes the color on, so a multi-texture's stage 2 (set by the engine) applies.
        TS(1, D3DTSS_COLOROP, D3DTOP_SELECTARG1), TS(1, D3DTSS_COLORARG1, D3DTA_CURRENT),
        TS(1, D3DTSS_COLORARG2, D3DTA_TEXTURE),   TS(2, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1),
        TS(2, D3DTSS_ALPHAARG1, D3DTA_CURRENT),   TS(2, D3DTSS_COLORARG1, D3DTA_CURRENT),
        TS(2, D3DTSS_COLORARG2, D3DTA_TEXTURE),
    };
    const EffectPass blackEdge{BlackEdgeStates(true), kBlackEdge, true};
    const EffectPass colorEdge{ColorEdgeStates(true), kColorEdge, true};
    auto flow = [](const char* name, int array) {
        return EffectTechnique{name, {{LightFlowStates(), array, false}}};
    };
    d.techniques = {
        {"REAL_COLOR", {{realColor, kRealColor, true}}},
        {"CARTOON_BLACK_EDGE", {{CartoonStates(D3DTOP_MODULATE, true), kCartoon, true}, blackEdge}},
        {"ADD_CARTOON_BLACK_EDGE", {{CartoonStates(D3DTOP_ADD, true), kCartoon, true}, blackEdge}},
        {"CARTOON_COLOR_EDGE", {{CartoonStates(D3DTOP_MODULATE, false), kCartoon, true}, colorEdge}},
        {"ADD_CARTOON_COLOR_EDGE", {{CartoonStates(D3DTOP_ADD, false), kCartoon, true}, colorEdge}},
        {"CARTOON", {{CartoonStates(D3DTOP_MODULATE, true), kCartoon, true}}},
        {"ADD_CARTOON", {{CartoonStates(D3DTOP_ADD, true), kCartoon, true}}},
        {"BLACK_EDGE", {{BlackEdgeStates(false), kBlackEdge, true}}},
        {"COLOR_EDGE", {{ColorEdgeStates(false), kColorEdge, true}}},
        flow("LIGHT_FLOW_WORLD_X", kFlowWorldX),
        flow("LIGHT_FLOW_WORLD_Y", kFlowWorldY),
        flow("LIGHT_FLOW_WORLD_Z", kFlowWorldZ),
        flow("LIGHT_FLOW_WORLD_POINT", kFlowWorldPoint),
        flow("LIGHT_FLOW_LOCAL_X", kFlowLocalX),
        flow("LIGHT_FLOW_LOCAL_Y", kFlowLocalY),
        flow("LIGHT_FLOW_LOCAL_Z", kFlowLocalZ),
        flow("LIGHT_FLOW_LOCAL_POINT", kFlowLocalPoint),
        {"CARTOON_FADE",
         {{{TS(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE), TS(0, D3DTSS_COLOROP, D3DTOP_BLENDDIFFUSEALPHA),
            TS(0, D3DTSS_COLORARG1, D3DTA_TEXTURE), TS(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE),
            SS(1, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP)},
           kRealColor, true}}},
        {"CARTOON_FADE_NONTEX",
         {{{TS(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1), TS(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE),
            TS(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1), TS(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE)},
           kRealColor, true}}},
    };
    return d;
}

}  // namespace

const EffectDescription& XRendererEffect()
{
    static const EffectDescription description = MakeXRenderer();
    return description;
}

HRESULT CreateXRendererEffect(Device* device, LPD3DXEFFECT* effect)
{
    if (effect == nullptr) {
        return D3DERR_INVALIDCALL;
    }
    *effect = nullptr;
    if (device == nullptr) {
        return D3DERR_INVALIDCALL;
    }
    *effect = new Effect(*device, XRendererEffect());
    return D3D_OK;
}

}  // namespace x2::d3d9
