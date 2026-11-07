// D3DX's animation sets and animation controller. docs/porting/client-d3d9.md#animation-2026-11-07-clientx-animation

#include "Animation.h"

#include <algorithm>
#include <cmath>
#include <cstring>

namespace x2::d3d9 {
namespace {

template <class Key>
void SortKeys(std::vector<Key>& keys)
{
    std::stable_sort(keys.begin(), keys.end(), [](const Key& a, const Key& b) { return a.Time < b.Time; });
}

// The two keys around `ticks` and how far between them it is (keys sorted, not empty).
template <class Key>
std::pair<const Key*, const Key*> Around(const std::vector<Key>& keys, double ticks, float& between)
{
    between = 0.0f;
    if (ticks <= keys.front().Time) {
        return {&keys.front(), &keys.front()};
    }
    if (ticks >= keys.back().Time) {
        return {&keys.back(), &keys.back()};
    }
    const auto next = std::upper_bound(keys.begin(), keys.end(), ticks,
                                       [](double t, const Key& key) { return t < key.Time; });
    const Key* b = &*next;
    const Key* a = b - 1;
    const double span = static_cast<double>(b->Time) - a->Time;
    between = span > 0.0 ? static_cast<float>((ticks - a->Time) / span) : 0.0f;
    return {a, b};
}

D3DXVECTOR3 Interpolate(const std::vector<D3DXKEY_VECTOR3>& keys, double ticks, const D3DXVECTOR3& none)
{
    if (keys.empty()) {
        return none;
    }
    float between;
    const auto [a, b] = Around(keys, ticks, between);
    D3DXVECTOR3 out;
    D3DXVec3Lerp(&out, &a->Value, &b->Value, between);
    return out;
}

D3DXQUATERNION Interpolate(const std::vector<D3DXKEY_QUATERNION>& keys, double ticks)
{
    if (keys.empty()) {
        return D3DXQUATERNION(0.0f, 0.0f, 0.0f, 1.0f);
    }
    float between;
    const auto [a, b] = Around(keys, ticks, between);
    if (a == b) {
        return a->Value;
    }
    D3DXQUATERNION out;
    D3DXQuaternionSlerp(&out, &a->Value, &b->Value, between);
    return out;
}

// --- Our "compressed" data: the keys as they are --------------------------------------------------------------------

constexpr std::uint32_t kCompressedMagic = 0x414B3258;  // "X2KA"
constexpr std::uint32_t kCompressedVersion = 1;

void Append(std::vector<std::byte>& out, const void* data, std::size_t size)
{
    const auto* bytes = static_cast<const std::byte*>(data);
    out.insert(out.end(), bytes, bytes + size);
}

void AppendU32(std::vector<std::byte>& out, std::uint32_t value)
{
    Append(out, &value, sizeof(value));
}

class Reader {
public:
    Reader(const void* data, std::size_t size) : m_data(static_cast<const std::byte*>(data)), m_size(size) {}
    bool Read(void* out, std::size_t size)
    {
        if (size > m_size - m_next) {
            return false;
        }
        std::memcpy(out, m_data + m_next, size);
        m_next += size;
        return true;
    }
    bool U32(std::uint32_t& value) { return Read(&value, sizeof(value)); }
    std::size_t Left() const { return m_size - m_next; }

private:
    const std::byte* m_data;
    std::size_t m_size;
    std::size_t m_next = 0;
};

template <class Key>
bool ReadKeys(Reader& in, std::uint32_t count, std::vector<Key>& keys)
{
    if (count > in.Left() / sizeof(Key)) {
        return false;
    }
    keys.resize(count);
    return count == 0 || in.Read(keys.data(), count * sizeof(Key));
}

HRESULT Buffer(const std::vector<std::byte>& bytes, LPD3DXBUFFER* out)
{
    const HRESULT hr = D3DXCreateBuffer(static_cast<DWORD>(bytes.size()), out);
    if (SUCCEEDED(hr) && !bytes.empty()) {
        std::memcpy((*out)->GetBufferPointer(), bytes.data(), bytes.size());
    }
    return hr;
}

// The frame matrix of one animation: scale, then rotation, then translation. The rotation keys are the conjugates of
// the rotations (as .x files store them; the client's rest matrices match the conjugate, 2026-11-07).
void Compose(const D3DXVECTOR3& s, const D3DXQUATERNION& r, const D3DXVECTOR3& t, D3DXMATRIX& out)
{
    const D3DXQUATERNION conjugate(-r.x, -r.y, -r.z, r.w);
    D3DXMatrixRotationQuaternion(&out, &conjugate);
    const float scale[3] = {s.x, s.y, s.z};
    for (int row = 0; row < 3; ++row) {
        for (int column = 0; column < 3; ++column) {
            out.m[row][column] *= scale[row];
        }
    }
    out.m[3][0] = t.x;
    out.m[3][1] = t.y;
    out.m[3][2] = t.z;
}

}  // namespace

// --- Animation sets -------------------------------------------------------------------------------------------------

SrtAnimations::SrtAnimations(LPCSTR name, double ticksPerSecond, D3DXPLAYBACK_TYPE playback, UINT capacity,
                             UINT callbackKeys, const D3DXKEY_CALLBACK* callbacks)
    : m_name(name ? name : ""), m_named(name != nullptr), m_ticksPerSecond(ticksPerSecond), m_playback(playback),
      m_capacity(capacity)
{
    if (callbacks) {
        m_callbacks.assign(callbacks, callbacks + callbackKeys);
    }
}

double SrtAnimations::Period() const
{
    float last = 0.0f;
    for (const SrtAnimation& animation : m_animations) {
        if (!animation.scales.empty()) {
            last = std::max(last, animation.scales.back().Time);
        }
        if (!animation.rotations.empty()) {
            last = std::max(last, animation.rotations.back().Time);
        }
        if (!animation.translations.empty()) {
            last = std::max(last, animation.translations.back().Time);
        }
    }
    return m_ticksPerSecond > 0.0 ? last / m_ticksPerSecond : 0.0;
}

double SrtAnimations::PeriodicPosition(double position) const
{
    const double period = Period();
    if (period <= 0.0) {
        return 0.0;
    }
    switch (m_playback) {
    case D3DXPLAY_ONCE:
        return std::clamp(position, 0.0, period);
    case D3DXPLAY_PINGPONG: {
        double p = std::fmod(position, 2.0 * period);
        if (p < 0.0) {
            p += 2.0 * period;
        }
        return p <= period ? p : 2.0 * period - p;
    }
    default: {
        double p = std::fmod(position, period);
        if (p < 0.0) {
            p += period;
        }
        return p;
    }
    }
}

HRESULT SrtAnimations::NameByIndex(UINT index, LPCSTR* name) const
{
    if (!name || index >= m_animations.size()) {
        return D3DERR_INVALIDCALL;
    }
    *name = m_animations[index].name.c_str();
    return D3D_OK;
}

HRESULT SrtAnimations::IndexByName(LPCSTR name, UINT* index) const
{
    if (!name || !index) {
        return D3DERR_INVALIDCALL;
    }
    for (std::size_t i = 0; i < m_animations.size(); ++i) {
        if (m_animations[i].name == name) {
            *index = static_cast<UINT>(i);
            return D3D_OK;
        }
    }
    return D3DERR_NOTFOUND;
}

HRESULT SrtAnimations::Srt(double periodicPosition, UINT index, D3DXVECTOR3* scale, D3DXQUATERNION* rotation,
                           D3DXVECTOR3* translation) const
{
    if (index >= m_animations.size()) {
        return D3DERR_INVALIDCALL;
    }
    const SrtAnimation& animation = m_animations[index];
    const double ticks = periodicPosition * m_ticksPerSecond;
    if (scale) {
        *scale = Interpolate(animation.scales, ticks, D3DXVECTOR3(1.0f, 1.0f, 1.0f));
    }
    if (rotation) {
        *rotation = Interpolate(animation.rotations, ticks);
    }
    if (translation) {
        *translation = Interpolate(animation.translations, ticks, D3DXVECTOR3(0.0f, 0.0f, 0.0f));
    }
    return D3D_OK;
}

HRESULT SrtAnimations::Add(SrtAnimation animation, DWORD* index)
{
    if (m_animations.size() >= m_capacity) {
        return D3DERR_INVALIDCALL;
    }
    SortKeys(animation.scales);
    SortKeys(animation.rotations);
    SortKeys(animation.translations);
    if (index) {
        *index = static_cast<DWORD>(m_animations.size());
    }
    m_animations.push_back(std::move(animation));
    return D3D_OK;
}

ULONG KeyframedAnimationSet::Release()
{
    const ULONG references = --m_references;
    if (references == 0) {
        delete this;
    }
    return references;
}

UINT KeyframedAnimationSet::GetNumScaleKeys(UINT animation)
{
    const SrtAnimation* a = m_data.At(animation);
    return a ? static_cast<UINT>(a->scales.size()) : 0;
}

UINT KeyframedAnimationSet::GetNumRotationKeys(UINT animation)
{
    const SrtAnimation* a = m_data.At(animation);
    return a ? static_cast<UINT>(a->rotations.size()) : 0;
}

UINT KeyframedAnimationSet::GetNumTranslationKeys(UINT animation)
{
    const SrtAnimation* a = m_data.At(animation);
    return a ? static_cast<UINT>(a->translations.size()) : 0;
}

HRESULT KeyframedAnimationSet::GetScaleKeys(UINT animation, LPD3DXKEY_VECTOR3 keys)
{
    const SrtAnimation* a = m_data.At(animation);
    if (!a || !keys) {
        return D3DERR_INVALIDCALL;
    }
    std::copy(a->scales.begin(), a->scales.end(), keys);
    return D3D_OK;
}

HRESULT KeyframedAnimationSet::GetRotationKeys(UINT animation, LPD3DXKEY_QUATERNION keys)
{
    const SrtAnimation* a = m_data.At(animation);
    if (!a || !keys) {
        return D3DERR_INVALIDCALL;
    }
    std::copy(a->rotations.begin(), a->rotations.end(), keys);
    return D3D_OK;
}

HRESULT KeyframedAnimationSet::GetTranslationKeys(UINT animation, LPD3DXKEY_VECTOR3 keys)
{
    const SrtAnimation* a = m_data.At(animation);
    if (!a || !keys) {
        return D3DERR_INVALIDCALL;
    }
    std::copy(a->translations.begin(), a->translations.end(), keys);
    return D3D_OK;
}

HRESULT KeyframedAnimationSet::RegisterAnimationSRTKeys(LPCSTR name, UINT scaleCount, UINT rotationCount,
                                                        UINT translationCount, const D3DXKEY_VECTOR3* scales,
                                                        const D3DXKEY_QUATERNION* rotations,
                                                        const D3DXKEY_VECTOR3* translations, DWORD* index)
{
    if (!name || (scaleCount && !scales) || (rotationCount && !rotations) || (translationCount && !translations)) {
        return D3DERR_INVALIDCALL;
    }
    SrtAnimation animation;
    animation.name = name;
    animation.scales.assign(scales, scales + scaleCount);
    animation.rotations.assign(rotations, rotations + rotationCount);
    animation.translations.assign(translations, translations + translationCount);
    return m_data.Add(std::move(animation), index);
}

// Lossless: the keys as they are (flags, lossiness and hierarchy are not used).
HRESULT KeyframedAnimationSet::Compress(DWORD /*flags*/, FLOAT /*lossiness*/, LPD3DXFRAME /*hierarchy*/,
                                        LPD3DXBUFFER* compressed)
{
    if (!compressed) {
        return D3DERR_INVALIDCALL;
    }
    std::vector<std::byte> bytes;
    AppendU32(bytes, kCompressedMagic);
    AppendU32(bytes, kCompressedVersion);
    AppendU32(bytes, m_data.Count());
    for (UINT i = 0; i < m_data.Count(); ++i) {
        const SrtAnimation& a = *m_data.At(i);
        AppendU32(bytes, static_cast<std::uint32_t>(a.name.size()));
        Append(bytes, a.name.data(), a.name.size());
        AppendU32(bytes, static_cast<std::uint32_t>(a.scales.size()));
        AppendU32(bytes, static_cast<std::uint32_t>(a.rotations.size()));
        AppendU32(bytes, static_cast<std::uint32_t>(a.translations.size()));
        Append(bytes, a.scales.data(), a.scales.size() * sizeof(D3DXKEY_VECTOR3));
        Append(bytes, a.rotations.data(), a.rotations.size() * sizeof(D3DXKEY_QUATERNION));
        Append(bytes, a.translations.data(), a.translations.size() * sizeof(D3DXKEY_VECTOR3));
    }
    return Buffer(bytes, compressed);
}

CompressedAnimationSet::CompressedAnimationSet(LPCSTR name, double ticksPerSecond, D3DXPLAYBACK_TYPE playback,
                                               UINT callbackKeys, const D3DXKEY_CALLBACK* callbacks,
                                               std::vector<SrtAnimation> animations, std::vector<std::byte> bytes)
    : m_data(name, ticksPerSecond, playback, static_cast<UINT>(animations.size()), callbackKeys, callbacks),
      m_bytes(std::move(bytes))
{
    for (SrtAnimation& animation : animations) {
        m_data.Add(std::move(animation), nullptr);
    }
}

ULONG CompressedAnimationSet::Release()
{
    const ULONG references = --m_references;
    if (references == 0) {
        delete this;
    }
    return references;
}

HRESULT CompressedAnimationSet::GetCompressedData(LPD3DXBUFFER* compressed)
{
    if (!compressed) {
        return D3DERR_INVALIDCALL;
    }
    return Buffer(m_bytes, compressed);
}

bool CompressedAnimationSet::Decode(const void* data, std::size_t size, std::vector<SrtAnimation>& animations)
{
    Reader in(data, size);
    std::uint32_t magic = 0, version = 0, count = 0;
    if (!in.U32(magic) || !in.U32(version) || !in.U32(count) || magic != kCompressedMagic ||
        version != kCompressedVersion) {
        return false;
    }
    animations.clear();
    for (std::uint32_t i = 0; i < count; ++i) {
        SrtAnimation a;
        std::uint32_t length = 0, scales = 0, rotations = 0, translations = 0;
        if (!in.U32(length) || length > in.Left()) {
            return false;
        }
        a.name.resize(length);
        if (!in.Read(a.name.data(), length) || !in.U32(scales) || !in.U32(rotations) || !in.U32(translations) ||
            !ReadKeys(in, scales, a.scales) || !ReadKeys(in, rotations, a.rotations) ||
            !ReadKeys(in, translations, a.translations)) {
            return false;
        }
        animations.push_back(std::move(a));
    }
    return in.Left() == 0;
}

// --- The controller -------------------------------------------------------------------------------------------------

AnimationController::AnimationController(UINT outputs, UINT sets, UINT tracks, UINT events)
    : m_maxOutputs(outputs), m_maxSets(sets), m_maxEvents(events), m_tracks(tracks)
{
}

AnimationController::~AnimationController()
{
    for (Track& track : m_tracks) {
        if (track.set) {
            track.set->Release();
        }
    }
    for (LPD3DXANIMATIONSET set : m_sets) {
        set->Release();
    }
}

ULONG AnimationController::Release()
{
    const ULONG references = --m_references;
    if (references == 0) {
        delete this;
    }
    return references;
}

HRESULT AnimationController::RegisterAnimationOutput(LPCSTR name, D3DXMATRIX* matrix)
{
    if (!name || !matrix) {
        return D3DERR_INVALIDCALL;
    }
    if (m_outputIndex.count(name) != 0) {
        return D3D_OK;  // a second frame of the same name: the first one is animated
    }
    if (m_outputs.size() >= m_maxOutputs) {
        return D3DERR_INVALIDCALL;
    }
    m_outputIndex.emplace(name, static_cast<int>(m_outputs.size()));
    m_outputs.push_back({name, matrix});
    for (Track& track : m_tracks) {
        track.outputs.clear();
    }
    return D3D_OK;
}

HRESULT AnimationController::RegisterAnimationSet(LPD3DXANIMATIONSET set)
{
    if (!set || m_sets.size() >= m_maxSets || std::find(m_sets.begin(), m_sets.end(), set) != m_sets.end()) {
        return D3DERR_INVALIDCALL;
    }
    set->AddRef();
    m_sets.push_back(set);
    return D3D_OK;
}

HRESULT AnimationController::UnregisterAnimationSet(LPD3DXANIMATIONSET set)
{
    const auto found = std::find(m_sets.begin(), m_sets.end(), set);
    if (found == m_sets.end()) {
        return D3DERR_INVALIDCALL;
    }
    for (UINT track = 0; track < m_tracks.size(); ++track) {
        if (m_tracks[track].set == set) {
            SetTrackAnimationSet(track, nullptr);
        }
    }
    m_sets.erase(found);
    set->Release();
    return D3D_OK;
}

HRESULT AnimationController::GetAnimationSet(UINT index, LPD3DXANIMATIONSET* set)
{
    if (!set) {
        return D3DERR_INVALIDCALL;
    }
    *set = nullptr;
    if (index >= m_sets.size()) {
        return D3DERR_INVALIDCALL;
    }
    *set = m_sets[index];
    (*set)->AddRef();
    return D3D_OK;
}

HRESULT AnimationController::GetAnimationSetByName(LPCSTR name, LPD3DXANIMATIONSET* set)
{
    if (!set || !name) {
        return D3DERR_INVALIDCALL;
    }
    *set = nullptr;
    for (LPD3DXANIMATIONSET candidate : m_sets) {
        if (LPCSTR candidateName = candidate->GetName(); candidateName && std::strcmp(candidateName, name) == 0) {
            *set = candidate;
            candidate->AddRef();
            return D3D_OK;
        }
    }
    return D3DERR_NOTFOUND;
}

// Events first (at the new global time), then the enabled tracks move at their speed and the outputs are blended.
// Callback keys are not reported (the engine passes no handler).
HRESULT AnimationController::AdvanceTime(double delta, ID3DXAnimationCallbackHandler* /*handler*/)
{
    m_time += delta;
    RunEvents();
    for (Track& track : m_tracks) {
        if (track.desc.Enable) {
            track.desc.Position += delta * track.desc.Speed;
        }
    }
    Blend();
    return D3D_OK;
}

// Pending events keep their schedule relative to the time they were keyed in.
HRESULT AnimationController::ResetTime()
{
    for (Event& event : m_events) {
        event.start -= m_time;
    }
    m_time = 0.0;
    return D3D_OK;
}

HRESULT AnimationController::SetTrackAnimationSet(UINT track, LPD3DXANIMATIONSET set)
{
    if (track >= m_tracks.size()) {
        return D3DERR_INVALIDCALL;
    }
    Track& t = m_tracks[track];
    if (set) {
        set->AddRef();
    }
    if (t.set) {
        t.set->Release();
    }
    t.set = set;
    t.outputs.clear();
    return D3D_OK;
}

HRESULT AnimationController::GetTrackAnimationSet(UINT track, LPD3DXANIMATIONSET* set)
{
    if (!set || track >= m_tracks.size()) {
        return D3DERR_INVALIDCALL;
    }
    *set = m_tracks[track].set;
    if (*set) {
        (*set)->AddRef();
    }
    return D3D_OK;
}

HRESULT AnimationController::SetTrackPriority(UINT track, D3DXPRIORITY_TYPE priority)
{
    if (track >= m_tracks.size()) {
        return D3DERR_INVALIDCALL;
    }
    m_tracks[track].desc.Priority = priority;
    return D3D_OK;
}

HRESULT AnimationController::SetTrackSpeed(UINT track, FLOAT speed)
{
    if (track >= m_tracks.size()) {
        return D3DERR_INVALIDCALL;
    }
    m_tracks[track].desc.Speed = speed;
    return D3D_OK;
}

HRESULT AnimationController::SetTrackWeight(UINT track, FLOAT weight)
{
    if (track >= m_tracks.size()) {
        return D3DERR_INVALIDCALL;
    }
    m_tracks[track].desc.Weight = weight;
    return D3D_OK;
}

HRESULT AnimationController::SetTrackPosition(UINT track, double position)
{
    if (track >= m_tracks.size()) {
        return D3DERR_INVALIDCALL;
    }
    m_tracks[track].desc.Position = position;
    return D3D_OK;
}

HRESULT AnimationController::SetTrackEnable(UINT track, BOOL enable)
{
    if (track >= m_tracks.size()) {
        return D3DERR_INVALIDCALL;
    }
    m_tracks[track].desc.Enable = enable;
    return D3D_OK;
}

HRESULT AnimationController::SetTrackDesc(UINT track, D3DXTRACK_DESC* desc)
{
    if (!desc || track >= m_tracks.size()) {
        return D3DERR_INVALIDCALL;
    }
    m_tracks[track].desc = *desc;
    return D3D_OK;
}

HRESULT AnimationController::GetTrackDesc(UINT track, D3DXTRACK_DESC* desc)
{
    if (!desc || track >= m_tracks.size()) {
        return D3DERR_INVALIDCALL;
    }
    *desc = m_tracks[track].desc;
    return D3D_OK;
}

DWORD AnimationController::AddEvent(Event event)
{
    if (event.track >= m_tracks.size() || m_events.size() >= m_maxEvents) {
        return 0;
    }
    event.handle = m_nextHandle++;
    if (m_nextHandle == 0) {
        m_nextHandle = 1;
    }
    m_events.push_back(event);
    return event.handle;
}

DWORD AnimationController::KeyTrackSpeed(UINT track, FLOAT speed, double start, double duration,
                                         D3DXTRANSITION_TYPE transition)
{
    return AddEvent({0, track, EventKind::Speed, start, duration, speed, FALSE, transition, false, 0.0f});
}

DWORD AnimationController::KeyTrackWeight(UINT track, FLOAT weight, double start, double duration,
                                          D3DXTRANSITION_TYPE transition)
{
    return AddEvent({0, track, EventKind::Weight, start, duration, weight, FALSE, transition, false, 0.0f});
}

DWORD AnimationController::KeyTrackEnable(UINT track, BOOL enable, double start)
{
    return AddEvent({0, track, EventKind::Enable, start, 0.0, 0.0f, enable, D3DXTRANSITION_LINEAR, false, 0.0f});
}

HRESULT AnimationController::UnkeyAllTrackEvents(UINT track)
{
    if (track >= m_tracks.size()) {
        return D3DERR_INVALIDCALL;
    }
    std::erase_if(m_events, [track](const Event& event) { return event.track == track; });
    return D3D_OK;
}

// A speed or weight event goes from the track's value when it starts to its value at start + duration, and is then
// done; an enable event is done when it starts.
void AnimationController::RunEvents()
{
    std::erase_if(m_events, [this](Event& event) {
        if (m_time < event.start) {
            return false;
        }
        D3DXTRACK_DESC& desc = m_tracks[event.track].desc;
        if (event.kind == EventKind::Enable) {
            desc.Enable = event.enable;
            return true;
        }
        float& value = event.kind == EventKind::Speed ? desc.Speed : desc.Weight;
        if (!event.started) {
            event.started = true;
            event.from = value;
        }
        float f = event.duration > 0.0 ? static_cast<float>(std::min(1.0, (m_time - event.start) / event.duration))
                                       : 1.0f;
        const bool done = f >= 1.0f;
        if (event.transition == D3DXTRANSITION_EASEINEASEOUT) {
            f = f * f * (3.0f - 2.0f * f);
        }
        value = done ? event.target : event.from + (event.target - event.from) * f;
        return done;
    });
}

void AnimationController::MapOutputs(Track& track) const
{
    track.outputs.assign(track.set->GetNumAnimations(), -1);
    for (UINT i = 0; i < track.outputs.size(); ++i) {
        LPCSTR name = nullptr;
        if (SUCCEEDED(track.set->GetAnimationNameByIndex(i, &name)) && name) {
            if (const auto found = m_outputIndex.find(name); found != m_outputIndex.end()) {
                track.outputs[i] = found->second;
            }
        }
    }
}

// Every enabled track with a set and a positive weight moves its outputs; where several tracks move one output, the
// result is their weighted average (rotations by slerp). Outputs no track moves keep their matrix.
void AnimationController::Blend()
{
    struct Sum {
        float weight = 0.0f;
        D3DXVECTOR3 scale;
        D3DXQUATERNION rotation;
        D3DXVECTOR3 translation;
    };
    std::vector<Sum> sums(m_outputs.size());
    for (Track& track : m_tracks) {
        if (!track.desc.Enable || !track.set || !(track.desc.Weight > 0.0f)) {
            continue;
        }
        if (track.outputs.size() != track.set->GetNumAnimations()) {
            MapOutputs(track);
        }
        const double position = track.set->GetPeriodicPosition(track.desc.Position);
        const float weight = track.desc.Weight;
        for (UINT i = 0; i < track.outputs.size(); ++i) {
            if (track.outputs[i] < 0) {
                continue;
            }
            D3DXVECTOR3 s, t;
            D3DXQUATERNION r;
            if (FAILED(track.set->GetSRT(position, i, &s, &r, &t))) {
                continue;
            }
            Sum& sum = sums[static_cast<std::size_t>(track.outputs[i])];
            if (sum.weight == 0.0f) {
                sum = {weight, s, r, t};
                continue;
            }
            const float f = weight / (sum.weight + weight);
            D3DXVec3Lerp(&sum.scale, &sum.scale, &s, f);
            D3DXVec3Lerp(&sum.translation, &sum.translation, &t, f);
            D3DXQuaternionSlerp(&sum.rotation, &sum.rotation, &r, f);
            sum.weight += weight;
        }
    }
    for (std::size_t o = 0; o < sums.size(); ++o) {
        if (sums[o].weight > 0.0f) {
            Compose(sums[o].scale, sums[o].rotation, sums[o].translation, *m_outputs[o].matrix);
        }
    }
}

// The same outputs, sets, tracks, events and time, within the new limits.
HRESULT AnimationController::CloneAnimationController(UINT outputs, UINT sets, UINT tracks, UINT events,
                                                      LPD3DXANIMATIONCONTROLLER* clone)
{
    if (!clone) {
        return D3DERR_INVALIDCALL;
    }
    *clone = nullptr;
    if (outputs < m_outputs.size() || sets < m_sets.size()) {
        return D3DERR_INVALIDCALL;
    }
    auto* copy = new AnimationController(outputs, sets, tracks, events);
    copy->m_outputs = m_outputs;
    copy->m_outputIndex = m_outputIndex;
    copy->m_sets = m_sets;
    for (LPD3DXANIMATIONSET set : copy->m_sets) {
        set->AddRef();
    }
    for (UINT track = 0; track < tracks && track < m_tracks.size(); ++track) {
        copy->m_tracks[track] = m_tracks[track];
        if (copy->m_tracks[track].set) {
            copy->m_tracks[track].set->AddRef();
        }
    }
    for (const Event& event : m_events) {
        if (event.track < tracks && copy->m_events.size() < events) {
            copy->m_events.push_back(event);
        }
    }
    copy->m_nextHandle = m_nextHandle;
    copy->m_time = m_time;
    *clone = copy;
    return D3D_OK;
}

}  // namespace x2::d3d9

// --- D3DX functions -------------------------------------------------------------------------------------------------

HRESULT D3DXCreateAnimationController(UINT MaxNumMatrices, UINT MaxNumAnimationSets, UINT MaxNumTracks,
                                      UINT MaxNumEvents, LPD3DXANIMATIONCONTROLLER* ppAnimController)
{
    if (!ppAnimController) {
        return D3DERR_INVALIDCALL;
    }
    *ppAnimController = new x2::d3d9::AnimationController(MaxNumMatrices, MaxNumAnimationSets, MaxNumTracks,
                                                           MaxNumEvents);
    return D3D_OK;
}

namespace {

HRESULT RegisterNamed(LPD3DXFRAME frame, x2::d3d9::AnimationController& controller)
{
    for (; frame != nullptr; frame = frame->pFrameSibling) {
        if (frame->Name != nullptr) {
            if (const HRESULT hr = controller.RegisterAnimationOutput(frame->Name, &frame->TransformationMatrix);
                FAILED(hr)) {
                return hr;
            }
        }
        if (const HRESULT hr = RegisterNamed(frame->pFrameFirstChild, controller); FAILED(hr)) {
            return hr;
        }
    }
    return D3D_OK;
}

}  // namespace

HRESULT D3DXFrameRegisterNamedMatrices(LPD3DXFRAME pFrameRoot, LPD3DXANIMATIONCONTROLLER pAnimController)
{
    auto* controller = dynamic_cast<x2::d3d9::AnimationController*>(pAnimController);
    if (!pFrameRoot || !controller) {
        return D3DERR_INVALIDCALL;
    }
    return RegisterNamed(pFrameRoot, *controller);
}

HRESULT D3DXCreateKeyframedAnimationSet(LPCSTR pName, DOUBLE TicksPerSecond, D3DXPLAYBACK_TYPE Playback,
                                        UINT NumAnimations, UINT NumCallbackKeys,
                                        const D3DXKEY_CALLBACK* pCallbackKeys,
                                        LPD3DXKEYFRAMEDANIMATIONSET* ppAnimationSet)
{
    if (!ppAnimationSet) {
        return D3DERR_INVALIDCALL;
    }
    *ppAnimationSet = new x2::d3d9::KeyframedAnimationSet(pName, TicksPerSecond, Playback, NumAnimations,
                                                          NumCallbackKeys, pCallbackKeys);
    return D3D_OK;
}

// Only data from KeyframedAnimationSet::Compress: D3DX's own compressed format is not read.
HRESULT D3DXCreateCompressedAnimationSet(LPCSTR pName, DOUBLE TicksPerSecond, D3DXPLAYBACK_TYPE Playback,
                                         LPD3DXBUFFER pCompressedData, UINT NumCallbackKeys,
                                         const D3DXKEY_CALLBACK* pCallbackKeys,
                                         LPD3DXCOMPRESSEDANIMATIONSET* ppAnimationSet)
{
    if (!ppAnimationSet) {
        return D3DERR_INVALIDCALL;
    }
    *ppAnimationSet = nullptr;
    if (!pCompressedData) {
        return D3DERR_INVALIDCALL;
    }
    std::vector<x2::d3d9::SrtAnimation> animations;
    const auto* data = static_cast<const std::byte*>(pCompressedData->GetBufferPointer());
    const std::size_t size = pCompressedData->GetBufferSize();
    if (!x2::d3d9::CompressedAnimationSet::Decode(data, size, animations)) {
        return E_FAIL;
    }
    *ppAnimationSet = new x2::d3d9::CompressedAnimationSet(pName, TicksPerSecond, Playback, NumCallbackKeys,
                                                           pCallbackKeys, std::move(animations),
                                                           std::vector<std::byte>(data, data + size));
    return D3D_OK;
}
