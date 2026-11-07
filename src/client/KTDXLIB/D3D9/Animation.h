#pragma once
// D3DX's animation: keyframed and compressed animation sets, and the animation controller that blends them on tracks
// into the frames' matrices. docs/porting/client-d3d9.md#animation-2026-11-07-clientx-animation

#include "D3DXTypes.h"

#include <string>
#include <unordered_map>
#include <vector>

namespace x2::d3d9 {

// One animation of a set: the keys of the frame (output) it moves. Key times are in ticks.
struct SrtAnimation {
    std::string name;
    std::vector<D3DXKEY_VECTOR3> scales;
    std::vector<D3DXKEY_QUATERNION> rotations;
    std::vector<D3DXKEY_VECTOR3> translations;
};

// What keyframed and compressed sets share: the animations, the period and the interpolation.
class SrtAnimations {
public:
    SrtAnimations(LPCSTR name, double ticksPerSecond, D3DXPLAYBACK_TYPE playback, UINT capacity,
                  UINT callbackKeys, const D3DXKEY_CALLBACK* callbacks);

    LPCSTR Name() const { return m_named ? m_name.c_str() : nullptr; }
    double Period() const;
    double PeriodicPosition(double position) const;
    UINT Count() const { return static_cast<UINT>(m_animations.size()); }
    const SrtAnimation* At(UINT index) const { return index < m_animations.size() ? &m_animations[index] : nullptr; }
    HRESULT NameByIndex(UINT index, LPCSTR* name) const;
    HRESULT IndexByName(LPCSTR name, UINT* index) const;
    HRESULT Srt(double periodicPosition, UINT index, D3DXVECTOR3* scale, D3DXQUATERNION* rotation,
                D3DXVECTOR3* translation) const;
    HRESULT Add(SrtAnimation animation, DWORD* index);

    D3DXPLAYBACK_TYPE Playback() const { return m_playback; }
    double TicksPerSecond() const { return m_ticksPerSecond; }
    UINT Capacity() const { return m_capacity; }
    const std::vector<D3DXKEY_CALLBACK>& Callbacks() const { return m_callbacks; }

private:
    std::string m_name;
    bool m_named;
    double m_ticksPerSecond;
    D3DXPLAYBACK_TYPE m_playback;
    UINT m_capacity;
    std::vector<D3DXKEY_CALLBACK> m_callbacks;
    std::vector<SrtAnimation> m_animations;
};

class KeyframedAnimationSet final : public ID3DXKeyframedAnimationSet {
public:
    KeyframedAnimationSet(LPCSTR name, double ticksPerSecond, D3DXPLAYBACK_TYPE playback, UINT animations,
                          UINT callbackKeys, const D3DXKEY_CALLBACK* callbacks)
        : m_data(name, ticksPerSecond, playback, animations, callbackKeys, callbacks)
    {
    }

    ULONG AddRef() override { return ++m_references; }
    ULONG Release() override;
    LPCSTR GetName() override { return m_data.Name(); }
    DOUBLE GetPeriod() override { return m_data.Period(); }
    DOUBLE GetPeriodicPosition(DOUBLE position) override { return m_data.PeriodicPosition(position); }
    UINT GetNumAnimations() override { return m_data.Count(); }
    HRESULT GetAnimationNameByIndex(UINT index, LPCSTR* name) override { return m_data.NameByIndex(index, name); }
    HRESULT GetAnimationIndexByName(LPCSTR name, UINT* index) override { return m_data.IndexByName(name, index); }
    HRESULT GetSRT(DOUBLE position, UINT animation, D3DXVECTOR3* scale, D3DXQUATERNION* rotation,
                   D3DXVECTOR3* translation) override
    {
        return m_data.Srt(position, animation, scale, rotation, translation);
    }
    D3DXPLAYBACK_TYPE GetPlaybackType() override { return m_data.Playback(); }
    DOUBLE GetSourceTicksPerSecond() override { return m_data.TicksPerSecond(); }
    UINT GetNumScaleKeys(UINT animation) override;
    HRESULT GetScaleKeys(UINT animation, LPD3DXKEY_VECTOR3 keys) override;
    UINT GetNumRotationKeys(UINT animation) override;
    HRESULT GetRotationKeys(UINT animation, LPD3DXKEY_QUATERNION keys) override;
    UINT GetNumTranslationKeys(UINT animation) override;
    HRESULT GetTranslationKeys(UINT animation, LPD3DXKEY_VECTOR3 keys) override;
    HRESULT RegisterAnimationSRTKeys(LPCSTR name, UINT scaleCount, UINT rotationCount, UINT translationCount,
                                     const D3DXKEY_VECTOR3* scales, const D3DXKEY_QUATERNION* rotations,
                                     const D3DXKEY_VECTOR3* translations, DWORD* index) override;
    HRESULT Compress(DWORD flags, FLOAT lossiness, LPD3DXFRAME hierarchy, LPD3DXBUFFER* compressed) override;

    SrtAnimations& Data() { return m_data; }

private:
    SrtAnimations m_data;
    ULONG m_references = 1;
};

// The "compressed" data is our own lossless serialization of the keys (KeyframedAnimationSet::Compress), not
// D3DX's format: no file of the client holds D3DX compressed data (2026-11-07).
class CompressedAnimationSet final : public ID3DXCompressedAnimationSet {
public:
    CompressedAnimationSet(LPCSTR name, double ticksPerSecond, D3DXPLAYBACK_TYPE playback, UINT callbackKeys,
                           const D3DXKEY_CALLBACK* callbacks, std::vector<SrtAnimation> animations,
                           std::vector<std::byte> bytes);

    ULONG AddRef() override { return ++m_references; }
    ULONG Release() override;
    LPCSTR GetName() override { return m_data.Name(); }
    DOUBLE GetPeriod() override { return m_data.Period(); }
    DOUBLE GetPeriodicPosition(DOUBLE position) override { return m_data.PeriodicPosition(position); }
    UINT GetNumAnimations() override { return m_data.Count(); }
    HRESULT GetAnimationNameByIndex(UINT index, LPCSTR* name) override { return m_data.NameByIndex(index, name); }
    HRESULT GetAnimationIndexByName(LPCSTR name, UINT* index) override { return m_data.IndexByName(name, index); }
    HRESULT GetSRT(DOUBLE position, UINT animation, D3DXVECTOR3* scale, D3DXQUATERNION* rotation,
                   D3DXVECTOR3* translation) override
    {
        return m_data.Srt(position, animation, scale, rotation, translation);
    }
    D3DXPLAYBACK_TYPE GetPlaybackType() override { return m_data.Playback(); }
    DOUBLE GetSourceTicksPerSecond() override { return m_data.TicksPerSecond(); }
    HRESULT GetCompressedData(LPD3DXBUFFER* compressed) override;

    // The animations in compressed data; false for data that is not ours.
    static bool Decode(const void* data, std::size_t size, std::vector<SrtAnimation>& animations);

private:
    SrtAnimations m_data;
    std::vector<std::byte> m_bytes;
    ULONG m_references = 1;
};

class AnimationController final : public ID3DXAnimationController {
public:
    AnimationController(UINT outputs, UINT sets, UINT tracks, UINT events);
    ~AnimationController() override;
    AnimationController(const AnimationController&) = delete;
    AnimationController& operator=(const AnimationController&) = delete;

    ULONG AddRef() override { return ++m_references; }
    ULONG Release() override;
    UINT GetMaxNumAnimationOutputs() override { return m_maxOutputs; }
    UINT GetMaxNumAnimationSets() override { return m_maxSets; }
    UINT GetMaxNumTracks() override { return static_cast<UINT>(m_tracks.size()); }
    UINT GetMaxNumEvents() override { return m_maxEvents; }
    HRESULT RegisterAnimationSet(LPD3DXANIMATIONSET set) override;
    HRESULT UnregisterAnimationSet(LPD3DXANIMATIONSET set) override;
    UINT GetNumAnimationSets() override { return static_cast<UINT>(m_sets.size()); }
    HRESULT GetAnimationSet(UINT index, LPD3DXANIMATIONSET* set) override;
    HRESULT GetAnimationSetByName(LPCSTR name, LPD3DXANIMATIONSET* set) override;
    HRESULT AdvanceTime(double delta, ID3DXAnimationCallbackHandler* handler) override;
    HRESULT ResetTime() override;
    double GetTime() override { return m_time; }
    HRESULT SetTrackAnimationSet(UINT track, LPD3DXANIMATIONSET set) override;
    HRESULT GetTrackAnimationSet(UINT track, LPD3DXANIMATIONSET* set) override;
    HRESULT SetTrackPriority(UINT track, D3DXPRIORITY_TYPE priority) override;
    HRESULT SetTrackSpeed(UINT track, FLOAT speed) override;
    HRESULT SetTrackWeight(UINT track, FLOAT weight) override;
    HRESULT SetTrackPosition(UINT track, double position) override;
    HRESULT SetTrackEnable(UINT track, BOOL enable) override;
    HRESULT SetTrackDesc(UINT track, D3DXTRACK_DESC* desc) override;
    HRESULT GetTrackDesc(UINT track, D3DXTRACK_DESC* desc) override;
    DWORD KeyTrackSpeed(UINT track, FLOAT speed, double start, double duration,
                        D3DXTRANSITION_TYPE transition) override;
    DWORD KeyTrackWeight(UINT track, FLOAT weight, double start, double duration,
                         D3DXTRANSITION_TYPE transition) override;
    DWORD KeyTrackEnable(UINT track, BOOL enable, double start) override;
    HRESULT UnkeyAllTrackEvents(UINT track) override;
    HRESULT CloneAnimationController(UINT outputs, UINT sets, UINT tracks, UINT events,
                                     LPD3DXANIMATIONCONTROLLER* clone) override;

    // ID3DXAnimationController::RegisterAnimationOutput for a matrix (what D3DXFrameRegisterNamedMatrices uses).
    HRESULT RegisterAnimationOutput(LPCSTR name, D3DXMATRIX* matrix);
    UINT OutputCount() const { return static_cast<UINT>(m_outputs.size()); }

private:
    struct Output {
        std::string name;
        D3DXMATRIX* matrix;
    };
    struct Track {
        LPD3DXANIMATIONSET set = nullptr;
        D3DXTRACK_DESC desc{D3DXPRIORITY_LOW, 1.0f, 1.0f, 0.0, TRUE};
        std::vector<int> outputs;   // per animation of the set, its output (-1: none)
    };
    enum class EventKind { Speed, Weight, Enable };
    struct Event {
        DWORD handle;
        UINT track;
        EventKind kind;
        double start;
        double duration;
        float target;
        BOOL enable;
        D3DXTRANSITION_TYPE transition;
        bool started = false;
        float from = 0.0f;
    };

    void MapOutputs(Track& track) const;
    DWORD AddEvent(Event event);
    void RunEvents();
    void Blend();

    UINT m_maxOutputs;
    UINT m_maxSets;
    UINT m_maxEvents;
    std::vector<Output> m_outputs;
    std::unordered_map<std::string, int> m_outputIndex;
    std::vector<LPD3DXANIMATIONSET> m_sets;
    std::vector<Track> m_tracks;
    std::vector<Event> m_events;
    DWORD m_nextHandle = 1;
    double m_time = 0.0;
    ULONG m_references = 1;
};

}  // namespace x2::d3d9
