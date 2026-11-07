// D3DX's animation on KTDXLIB's device (docs/porting/client-d3d9.md#animation-2026-11-07-clientx-animation):
// keyframed and compressed animation sets, the animation controller, .x animation sets and, with
// X2_MASSFILE_CLIENT_DATA set, every animated .x file of the client.

#include "Animation.h"
#include "D3DXTypes.h"
#include "Device.h"
#include "XFile.h"

#include "x2/kom/mount.h"
#include "x2/render/null.h"

#include <doctest/doctest.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <string>
#include <thread>
#include <vector>

namespace {

std::unique_ptr<x2::render::Device> NullRender()
{
    std::string error;
    x2::render::DeviceOptions options;
    options.backend = x2::render::Backend::Null;
    auto device = x2::render::Device::Create(options, error);
    REQUIRE_MESSAGE(device, error);
    return device;
}

template <class T>
struct Ref {
    T* p = nullptr;
    Ref() = default;
    Ref(const Ref&) = delete;
    Ref& operator=(const Ref&) = delete;
    ~Ref()
    {
        if (p) {
            p->Release();
        }
    }
    T* operator->() const { return p; }
};

char* Copy(const char* text)
{
    if (text == nullptr) {
        return nullptr;
    }
    char* copy = new char[std::strlen(text) + 1];
    std::memcpy(copy, text, std::strlen(text) + 1);
    return copy;
}

// Frames with copied names; mesh containers keep their mesh.
struct Allocator : ID3DXAllocateHierarchy {
    std::atomic<int> frames{0};
    STDMETHOD(CreateFrame)(THIS_ LPCSTR name, LPD3DXFRAME* frame) override
    {
        auto* f = new D3DXFRAME{};
        f->Name = Copy(name);
        *frame = f;
        ++frames;
        return D3D_OK;
    }
    STDMETHOD(CreateMeshContainer)(THIS_ LPCSTR, const D3DXMESHDATA* data, const D3DXMATERIAL*,
                                   const D3DXEFFECTINSTANCE*, DWORD, const DWORD*, LPD3DXSKININFO,
                                   LPD3DXMESHCONTAINER* container) override
    {
        auto* c = new D3DXMESHCONTAINER{};
        c->MeshData = *data;
        c->MeshData.pMesh->AddRef();
        *container = c;
        return D3D_OK;
    }
    STDMETHOD(DestroyFrame)(THIS_ LPD3DXFRAME frame) override
    {
        delete[] frame->Name;
        delete frame;
        --frames;
        return D3D_OK;
    }
    STDMETHOD(DestroyMeshContainer)(THIS_ LPD3DXMESHCONTAINER c) override
    {
        c->MeshData.pMesh->Release();
        delete c;
        return D3D_OK;
    }
};

D3DXMATRIX Srt(const D3DXVECTOR3& s, const D3DXQUATERNION& r, const D3DXVECTOR3& t)
{
    D3DXMATRIX m;
    D3DXMatrixTransformation(&m, nullptr, nullptr, &s, nullptr, &r, &t);
    return m;
}

bool Near(const D3DXMATRIX& a, const D3DXMATRIX& b, float tolerance = 1e-4f)
{
    for (int i = 0; i < 16; ++i) {
        const float x = a.m[i / 4][i % 4], y = b.m[i / 4][i % 4];
        if (!(std::fabs(x - y) <= tolerance * std::max(1.0f, std::fabs(y)))) {
            return false;
        }
    }
    return true;
}

D3DXQUATERNION Conjugate(const D3DXQUATERNION& q)
{
    return D3DXQUATERNION(-q.x, -q.y, -q.z, q.w);
}

// The rotation by `angle` about a unit axis.
D3DXQUATERNION AxisAngle(float x, float y, float z, float angle)
{
    const float s = std::sin(angle / 2);
    return D3DXQUATERNION(x * s, y * s, z * s, std::cos(angle / 2));
}

// A set "Walk" with one animation of frame "Bone" over 0..480 ticks at 4800 ticks/s (0.1 s): scale 1 -> 3,
// translation (0,0,0) -> (10,0,0), rotation 0 -> 90 degrees about z (stored conjugated, as .x files store them).
x2::d3d9::KeyframedAnimationSet* Walk(D3DXPLAYBACK_TYPE playback = D3DXPLAY_LOOP)
{
    LPD3DXKEYFRAMEDANIMATIONSET set = nullptr;
    REQUIRE(D3DXCreateKeyframedAnimationSet("Walk", 4800, playback, 2, 0, nullptr, &set) == D3D_OK);
    const D3DXKEY_VECTOR3 scales[] = {{480, D3DXVECTOR3(3, 3, 3)}, {0, D3DXVECTOR3(1, 1, 1)}};  // unsorted
    const D3DXKEY_QUATERNION rotations[] = {{0, D3DXQUATERNION(0, 0, 0, 1)},
                                            {480, Conjugate(AxisAngle(0, 0, 1, D3DX_PI / 2))}};
    const D3DXKEY_VECTOR3 translations[] = {{0, D3DXVECTOR3(0, 0, 0)}, {480, D3DXVECTOR3(10, 0, 0)}};
    DWORD index = 99;
    REQUIRE(set->RegisterAnimationSRTKeys("Bone", 2, 2, 2, scales, rotations, translations, &index) == D3D_OK);
    CHECK(index == 0);
    return static_cast<x2::d3d9::KeyframedAnimationSet*>(set);
}

}  // namespace

TEST_CASE("d3dx: keyframed animation sets")
{
    Ref<x2::d3d9::KeyframedAnimationSet> set;
    set.p = Walk();
    CHECK(std::string(set->GetName()) == "Walk");
    CHECK(set->GetPeriod() == doctest::Approx(0.1));
    CHECK(set->GetNumAnimations() == 1);
    CHECK(set->GetSourceTicksPerSecond() == 4800.0);
    CHECK(set->GetPlaybackType() == D3DXPLAY_LOOP);
    LPCSTR name = nullptr;
    REQUIRE(set->GetAnimationNameByIndex(0, &name) == D3D_OK);
    CHECK(std::string(name) == "Bone");
    UINT index = 9;
    CHECK(set->GetAnimationIndexByName("Bone", &index) == D3D_OK);
    CHECK(index == 0);
    CHECK(set->GetAnimationIndexByName("Other", &index) == D3DERR_NOTFOUND);
    CHECK(set->GetAnimationNameByIndex(1, &name) == D3DERR_INVALIDCALL);

    // Keys come back sorted by time.
    REQUIRE(set->GetNumScaleKeys(0) == 2);
    D3DXKEY_VECTOR3 scales[2];
    REQUIRE(set->GetScaleKeys(0, scales) == D3D_OK);
    CHECK(scales[0].Time == 0.0f);
    CHECK(scales[1].Value.x == 3.0f);
    CHECK(set->GetNumRotationKeys(0) == 2);
    CHECK(set->GetNumTranslationKeys(0) == 2);
    CHECK(set->GetNumScaleKeys(5) == 0);

    // Halfway: linear scale and translation, slerped rotation.
    D3DXVECTOR3 s, t;
    D3DXQUATERNION r;
    REQUIRE(set->GetSRT(0.05, 0, &s, &r, &t) == D3D_OK);
    CHECK(s.x == doctest::Approx(2.0f));
    CHECK(t.x == doctest::Approx(5.0f));
    const D3DXQUATERNION half = Conjugate(AxisAngle(0, 0, 1, D3DX_PI / 4));
    CHECK(r.z == doctest::Approx(half.z));
    CHECK(r.w == doctest::Approx(half.w));
    // Before the first key and after the last one: those keys.
    REQUIRE(set->GetSRT(-1.0, 0, &s, &r, &t) == D3D_OK);
    CHECK(s.x == 1.0f);
    REQUIRE(set->GetSRT(5.0, 0, &s, &r, &t) == D3D_OK);
    CHECK(t.x == 10.0f);
    CHECK(set->GetSRT(0, 1, &s, &r, &t) == D3DERR_INVALIDCALL);

    // Playback: loop, once, ping-pong.
    CHECK(set->GetPeriodicPosition(0.25) == doctest::Approx(0.05));
    CHECK(set->GetPeriodicPosition(-0.03) == doctest::Approx(0.07));
    Ref<x2::d3d9::KeyframedAnimationSet> once, pingPong;
    once.p = Walk(D3DXPLAY_ONCE);
    pingPong.p = Walk(D3DXPLAY_PINGPONG);
    CHECK(once->GetPeriodicPosition(0.25) == doctest::Approx(0.1));
    CHECK(once->GetPeriodicPosition(-1) == 0.0);
    CHECK(pingPong->GetPeriodicPosition(0.13) == doctest::Approx(0.07));
    CHECK(pingPong->GetPeriodicPosition(0.23) == doctest::Approx(0.03));

    // No keys: the identity transform. More animations than declared: refused.
    REQUIRE(set->RegisterAnimationSRTKeys("Empty", 0, 0, 0, nullptr, nullptr, nullptr, nullptr) == D3D_OK);
    REQUIRE(set->GetSRT(0.05, 1, &s, &r, &t) == D3D_OK);
    CHECK(s == D3DXVECTOR3(1, 1, 1));
    CHECK(r == D3DXQUATERNION(0, 0, 0, 1));
    CHECK(t == D3DXVECTOR3(0, 0, 0));
    CHECK(set->RegisterAnimationSRTKeys("Third", 0, 0, 0, nullptr, nullptr, nullptr, nullptr) ==
          D3DERR_INVALIDCALL);

    // Compressed (our lossless data): the same animations back.
    Ref<ID3DXBuffer> data;
    REQUIRE(set->Compress(0, 0.5f, nullptr, &data.p) == D3D_OK);
    Ref<ID3DXCompressedAnimationSet> compressed;
    REQUIRE(D3DXCreateCompressedAnimationSet("Walk", 4800, D3DXPLAY_LOOP, data.p, 0, nullptr, &compressed.p) ==
            D3D_OK);
    CHECK(dynamic_cast<ID3DXKeyframedAnimationSet*>(static_cast<ID3DXAnimationSet*>(compressed.p)) == nullptr);
    CHECK(compressed->GetNumAnimations() == 2);
    CHECK(compressed->GetPeriod() == doctest::Approx(0.1));
    REQUIRE(compressed->GetSRT(0.05, 0, &s, &r, &t) == D3D_OK);
    CHECK(t.x == doctest::Approx(5.0f));
    Ref<ID3DXBuffer> again;
    REQUIRE(compressed->GetCompressedData(&again.p) == D3D_OK);
    REQUIRE(again->GetBufferSize() == data->GetBufferSize());
    CHECK(std::memcmp(again->GetBufferPointer(), data->GetBufferPointer(), data->GetBufferSize()) == 0);
    // Data that is not ours (D3DX's own compressed format) is refused.
    std::memset(data->GetBufferPointer(), 0x5A, data->GetBufferSize());
    LPD3DXCOMPRESSEDANIMATIONSET foreign = reinterpret_cast<LPD3DXCOMPRESSEDANIMATIONSET>(&data);
    CHECK(D3DXCreateCompressedAnimationSet("X", 4800, D3DXPLAY_LOOP, data.p, 0, nullptr, &foreign) == E_FAIL);
    CHECK(foreign == nullptr);
}

TEST_CASE("d3dx: the animation controller")
{
    // Root { Bone { } }, and a nameless frame that is not an output.
    D3DXFRAME root{}, bone{}, nameless{};
    char rootName[] = "Root", boneName[] = "Bone";
    root.Name = rootName;
    bone.Name = boneName;
    root.pFrameFirstChild = &bone;
    bone.pFrameSibling = &nameless;
    D3DXMatrixIdentity(&root.TransformationMatrix);
    D3DXMatrixTranslation(&bone.TransformationMatrix, 0, 7, 0);
    const D3DXMATRIX rest = bone.TransformationMatrix;
    CHECK(D3DXFrameNumNamedMatrices(&root) == 2);

    Ref<ID3DXAnimationController> controller;
    REQUIRE(D3DXCreateAnimationController(2, 2, 2, 3, &controller.p) == D3D_OK);
    CHECK(controller->GetMaxNumAnimationOutputs() == 2);
    CHECK(controller->GetMaxNumAnimationSets() == 2);
    CHECK(controller->GetMaxNumTracks() == 2);
    CHECK(controller->GetMaxNumEvents() == 3);
    Ref<x2::d3d9::KeyframedAnimationSet> walk;
    walk.p = Walk();
    REQUIRE(controller->RegisterAnimationSet(walk.p) == D3D_OK);
    CHECK(controller->RegisterAnimationSet(walk.p) == D3DERR_INVALIDCALL);  // already there
    REQUIRE(D3DXFrameRegisterNamedMatrices(&root, controller.p) == D3D_OK);
    CHECK(dynamic_cast<x2::d3d9::AnimationController*>(controller.p)->OutputCount() == 2);

    // No track has a set: nothing moves.
    REQUIRE(controller->AdvanceTime(0.05, nullptr) == D3D_OK);
    CHECK(bone.TransformationMatrix == rest);
    CHECK(controller->GetTime() == doctest::Approx(0.05));

    // Track 0 plays Walk: scale, then the conjugate of the key's rotation, then translation.
    REQUIRE(controller->SetTrackAnimationSet(0, walk.p) == D3D_OK);
    REQUIRE(controller->SetTrackPosition(0, 0.0) == D3D_OK);
    REQUIRE(controller->AdvanceTime(0.05, nullptr) == D3D_OK);
    CHECK(Near(bone.TransformationMatrix,
               Srt(D3DXVECTOR3(2, 2, 2), AxisAngle(0, 0, 1, D3DX_PI / 4), D3DXVECTOR3(5, 0, 0))));
    CHECK(root.TransformationMatrix == D3DXMATRIX(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1));
    D3DXTRACK_DESC desc{};
    REQUIRE(controller->GetTrackDesc(0, &desc) == D3D_OK);
    CHECK(desc.Position == doctest::Approx(0.05));   // track time is not wrapped
    CHECK(desc.Speed == 1.0f);
    CHECK(desc.Weight == 1.0f);
    CHECK(desc.Enable == TRUE);
    CHECK(desc.Priority == D3DXPRIORITY_LOW);
    // Speed 2 for 0.05 s: position 0.15, periodic 0.05.
    REQUIRE(controller->SetTrackSpeed(0, 2.0f) == D3D_OK);
    REQUIRE(controller->AdvanceTime(0.05, nullptr) == D3D_OK);
    REQUIRE(controller->GetTrackDesc(0, &desc) == D3D_OK);
    CHECK(desc.Position == doctest::Approx(0.15));
    CHECK(bone.TransformationMatrix._41 == doctest::Approx(5.0f));
    // A disabled track neither moves nor animates.
    bone.TransformationMatrix = rest;
    REQUIRE(controller->SetTrackEnable(0, FALSE) == D3D_OK);
    REQUIRE(controller->AdvanceTime(0.05, nullptr) == D3D_OK);
    CHECK(bone.TransformationMatrix == rest);
    REQUIRE(controller->GetTrackDesc(0, &desc) == D3D_OK);
    CHECK(desc.Position == doctest::Approx(0.15));

    // A transition as KTDGXSkinAnim::ChangeAnim keys it: track 0 fades out and stops, track 1 (the same set, held at
    // 0.075 s) fades in, over 0.1 s from global time 0.
    REQUIRE(controller->ResetTime() == D3D_OK);
    CHECK(controller->GetTime() == 0.0);
    REQUIRE(controller->SetTrackEnable(0, TRUE) == D3D_OK);
    REQUIRE(controller->SetTrackSpeed(0, 1.0f) == D3D_OK);
    REQUIRE(controller->SetTrackPosition(0, 0.0) == D3D_OK);
    CHECK(controller->KeyTrackEnable(0, FALSE, 0.1) != 0);
    CHECK(controller->KeyTrackSpeed(0, 0.0f, 0.0, 0.1, D3DXTRANSITION_LINEAR) != 0);
    CHECK(controller->KeyTrackWeight(0, 0.0f, 0.0, 0.1, D3DXTRANSITION_LINEAR) != 0);
    CHECK(controller->KeyTrackWeight(1, 1.0f, 0.0, 0.1, D3DXTRANSITION_LINEAR) == 0);  // 3 events at most
    REQUIRE(controller->SetTrackAnimationSet(1, walk.p) == D3D_OK);
    REQUIRE(controller->SetTrackPosition(1, 0.075) == D3D_OK);
    REQUIRE(controller->SetTrackSpeed(1, 0.0f) == D3D_OK);
    REQUIRE(controller->SetTrackWeight(1, 1.0f) == D3D_OK);
    REQUIRE(controller->AdvanceTime(0.025, nullptr) == D3D_OK);
    // Track 0 at speed 0.75 (keyed from 1 to 0): position 0.01875, weight 0.75; track 1 at 0.075, weight 1.
    REQUIRE(controller->GetTrackDesc(0, &desc) == D3D_OK);
    CHECK(desc.Speed == doctest::Approx(0.75f));
    CHECK(desc.Weight == doctest::Approx(0.75f));
    CHECK(desc.Position == doctest::Approx(0.01875));
    // Translation: weighted average of 1.875 (weight 0.75) and 7.5 (weight 1).
    CHECK(bone.TransformationMatrix._41 == doctest::Approx((1.875f * 0.75f + 7.5f) / 1.75f));
    REQUIRE(controller->AdvanceTime(0.1, nullptr) == D3D_OK);
    REQUIRE(controller->GetTrackDesc(0, &desc) == D3D_OK);
    CHECK(desc.Enable == FALSE);
    CHECK(desc.Weight == 0.0f);
    CHECK(Near(bone.TransformationMatrix,
               Srt(D3DXVECTOR3(2.5f, 2.5f, 2.5f), AxisAngle(0, 0, 1, 3 * D3DX_PI / 8), D3DXVECTOR3(7.5f, 0, 0))));

    // Events wait for their start; ResetTime keeps the schedule relative to the time they were keyed in.
    const DWORD later = controller->KeyTrackSpeed(1, 4.0f, controller->GetTime() + 1.0, 0.0, D3DXTRANSITION_LINEAR);
    CHECK(later != 0);
    REQUIRE(controller->ResetTime() == D3D_OK);
    REQUIRE(controller->AdvanceTime(0.5, nullptr) == D3D_OK);
    REQUIRE(controller->GetTrackDesc(1, &desc) == D3D_OK);
    CHECK(desc.Speed == 0.0f);
    REQUIRE(controller->AdvanceTime(0.5, nullptr) == D3D_OK);
    REQUIRE(controller->GetTrackDesc(1, &desc) == D3D_OK);
    CHECK(desc.Speed == 4.0f);
    CHECK(controller->KeyTrackSpeed(1, 1.0f, 5.0, 1.0, D3DXTRANSITION_EASEINEASEOUT) != 0);
    REQUIRE(controller->UnkeyAllTrackEvents(1) == D3D_OK);
    REQUIRE(controller->AdvanceTime(10.0, nullptr) == D3D_OK);
    REQUIRE(controller->GetTrackDesc(1, &desc) == D3D_OK);
    CHECK(desc.Speed == 4.0f);
    CHECK(controller->KeyTrackSpeed(2, 1.0f, 0, 0, D3DXTRANSITION_LINEAR) == 0);   // no track 2
    CHECK(controller->GetTrackDesc(2, &desc) == D3DERR_INVALIDCALL);

    // Sets: by index and name, with a reference for the caller.
    LPD3DXANIMATIONSET found = nullptr;
    REQUIRE(controller->GetAnimationSet(0, &found) == D3D_OK);
    CHECK(found == walk.p);
    found->Release();
    REQUIRE(controller->GetAnimationSetByName("Walk", &found) == D3D_OK);
    CHECK(found == walk.p);
    found->Release();
    CHECK(controller->GetAnimationSetByName("Run", &found) == D3DERR_NOTFOUND);
    CHECK(controller->GetAnimationSet(1, &found) == D3DERR_INVALIDCALL);

    // A clone with room for one more set: same outputs, sets, tracks and time; KTDGXSkinAnim then registers more.
    Ref<ID3DXAnimationController> clone;
    REQUIRE(controller->CloneAnimationController(2, 3, 2, 3, &clone.p) == D3D_OK);
    CHECK(clone->GetNumAnimationSets() == 1);
    CHECK(clone->GetMaxNumAnimationSets() == 3);
    CHECK(clone->GetTime() == controller->GetTime());
    LPD3DXANIMATIONSET trackSet = nullptr;
    REQUIRE(clone->GetTrackAnimationSet(1, &trackSet) == D3D_OK);
    CHECK(trackSet == walk.p);
    trackSet->Release();
    Ref<x2::d3d9::KeyframedAnimationSet> other;
    other.p = Walk();
    REQUIRE(clone->RegisterAnimationSet(other.p) == D3D_OK);
    CHECK(clone->GetNumAnimationSets() == 2);
    CHECK(controller->GetNumAnimationSets() == 1);
    bone.TransformationMatrix = rest;
    REQUIRE(clone->SetTrackPosition(1, 0.05) == D3D_OK);
    REQUIRE(clone->SetTrackSpeed(1, 0.0f) == D3D_OK);
    REQUIRE(clone->AdvanceTime(0.0, nullptr) == D3D_OK);
    CHECK(bone.TransformationMatrix._41 == doctest::Approx(5.0f));  // the clone moves the same frames
    LPD3DXANIMATIONCONTROLLER small = reinterpret_cast<LPD3DXANIMATIONCONTROLLER>(&root);
    CHECK(clone->CloneAnimationController(2, 1, 2, 3, &small) == D3DERR_INVALIDCALL);
    CHECK(small == nullptr);

    // Unregistering a set takes it off the tracks.
    REQUIRE(clone->UnregisterAnimationSet(walk.p) == D3D_OK);
    REQUIRE(clone->GetTrackAnimationSet(1, &trackSet) == D3D_OK);
    CHECK(trackSet == nullptr);
    CHECK(clone->UnregisterAnimationSet(walk.p) == D3DERR_INVALIDCALL);

    // More outputs than declared: refused; a second frame of a name already registered is not one more.
    Ref<ID3DXAnimationController> one;
    REQUIRE(D3DXCreateAnimationController(1, 1, 1, 1, &one.p) == D3D_OK);
    CHECK(D3DXFrameRegisterNamedMatrices(&root, one.p) == D3DERR_INVALIDCALL);
    D3DXFRAME twin{};
    twin.Name = boneName;
    bone.pFrameFirstChild = &twin;
    Ref<ID3DXAnimationController> two;
    REQUIRE(D3DXCreateAnimationController(3, 1, 1, 1, &two.p) == D3D_OK);
    CHECK(D3DXFrameRegisterNamedMatrices(&root, two.p) == D3D_OK);
    CHECK(dynamic_cast<x2::d3d9::AnimationController*>(two.p)->OutputCount() == 2);
    CHECK(D3DXFrameRegisterNamedMatrices(nullptr, two.p) == D3DERR_INVALIDCALL);
}

namespace {

// Frame Arm (rest: 90 degrees about z, at (0,5,0)) with a rotation/position set "Wave", and frame Leg with a matrix
// key set "Kick"; 480 ticks per second.
const char* const kAnimated = R"(xof 0303txt 0032
AnimTicksPerSecond { 480; }
Frame Body {
  Frame Arm {
    FrameTransformMatrix { 0,1,0,0, -1,0,0,0, 0,0,1,0, 0,5,0,1;; }
  }
  Frame Leg {
    FrameTransformMatrix { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1;; }
  }
  Mesh { 3; 0;0;0;, 1;0;0;, 0;1;0;; 1; 3;0,1,2;; }
}
AnimationSet Wave {
  Animation {
    { Arm }
    AnimationKey { 0; 2; 0; 4; 0.707107,0,0,-0.707107;;, 480; 4; 1,0,0,0;;; }
    AnimationKey { 2; 2; 0; 3; 0,5,0;;, 480; 3; 0,9,0;;; }
  }
  Animation { AnimationKey { 1; 1; 0; 3; 2,2,2;;; } }
}
AnimationSet Kick {
  Animation {
    { Leg }
    AnimationKey { 4; 1; 0; 16; 0,0,-2,0, 0,2,0,0, 2,0,0,0, 1,2,3,1;;; }
  }
}
)";

}  // namespace

TEST_CASE("d3dx: animation sets of .x files")
{
    auto render = NullRender();
    x2::d3d9::Device device(*render, nullptr);
    REQUIRE(device.Ok());
    Allocator alloc;
    LPD3DXFRAME root = nullptr;
    Ref<ID3DXAnimationController> controller;
    const std::string text = kAnimated;
    REQUIRE(D3DXLoadMeshHierarchyFromXInMemory(text.data(), static_cast<DWORD>(text.size()), D3DXMESH_MANAGED,
                                               &device, &alloc, nullptr, &root, &controller.p) == D3D_OK);
    REQUIRE(controller.p != nullptr);
    CHECK(controller->GetNumAnimationSets() == 2);
    CHECK(controller->GetMaxNumTracks() == 2);
    CHECK(controller->GetMaxNumAnimationOutputs() == D3DXFrameNumNamedMatrices(root));
    LPD3DXANIMATIONSET wave = nullptr, kick = nullptr, onTrack = nullptr;
    REQUIRE(controller->GetAnimationSet(0, &wave) == D3D_OK);
    REQUIRE(controller->GetAnimationSet(1, &kick) == D3D_OK);
    CHECK(std::string(wave->GetName()) == "Wave");
    CHECK(std::string(kick->GetName()) == "Kick");
    CHECK(wave->GetPeriod() == doctest::Approx(1.0));
    CHECK(wave->GetNumAnimations() == 1);  // the animation of no frame is left out
    REQUIRE(controller->GetTrackAnimationSet(0, &onTrack) == D3D_OK);
    CHECK(onTrack == wave);
    onTrack->Release();

    // At time 0, Arm is at its rest matrix: the file's w,x,y,z rotation key is the conjugate of the rotation.
    LPD3DXFRAME arm = D3DXFrameFind(root, "Arm");
    LPD3DXFRAME leg = D3DXFrameFind(root, "Leg");
    REQUIRE(arm != nullptr);
    REQUIRE(leg != nullptr);
    const D3DXMATRIX armRest = arm->TransformationMatrix;
    D3DXMatrixIdentity(&arm->TransformationMatrix);
    REQUIRE(controller->AdvanceTime(0.0, nullptr) == D3D_OK);
    CHECK(Near(arm->TransformationMatrix, armRest));
    REQUIRE(controller->AdvanceTime(0.5, nullptr) == D3D_OK);
    CHECK(Near(arm->TransformationMatrix,
               Srt(D3DXVECTOR3(1, 1, 1), AxisAngle(0, 0, 1, D3DX_PI / 4), D3DXVECTOR3(0, 7, 0))));

    // The matrix key comes back as the same matrix.
    REQUIRE(controller->SetTrackAnimationSet(0, kick) == D3D_OK);
    REQUIRE(controller->AdvanceTime(0.0, nullptr) == D3D_OK);
    CHECK(Near(leg->TransformationMatrix, D3DXMATRIX(0, 0, -2, 0, 0, 2, 0, 0, 2, 0, 0, 0, 1, 2, 3, 1)));
    auto* keyframed = dynamic_cast<ID3DXKeyframedAnimationSet*>(kick);
    REQUIRE(keyframed != nullptr);
    CHECK(keyframed->GetSourceTicksPerSecond() == 480.0);
    CHECK(keyframed->GetNumScaleKeys(0) == 1);
    D3DXFRAME mirror{};
    D3DXMatrixScaling(&mirror.TransformationMatrix, -1, 1, 1);
    // (A mirrored key: negative determinant.)
    const std::string mirrored = "xof 0303txt 0032 Frame M { } AnimationSet { Animation { { M } AnimationKey { 4; "
                                 "1; 0; 16; -1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1;;; } } }";
    LPD3DXFRAME mroot = nullptr;
    Ref<ID3DXAnimationController> mcontroller;
    REQUIRE(D3DXLoadMeshHierarchyFromXInMemory(mirrored.data(), static_cast<DWORD>(mirrored.size()), 0, &device,
                                               &alloc, nullptr, &mroot, &mcontroller.p) == D3D_OK);
    REQUIRE(mcontroller->AdvanceTime(0.0, nullptr) == D3D_OK);
    CHECK(Near(mroot->TransformationMatrix, mirror.TransformationMatrix));
    D3DXFrameDestroy(mroot, &alloc);
    wave->Release();
    kick->Release();

    // No AnimationSet: no controller.
    D3DXFrameDestroy(root, &alloc);
    const std::string still = "xof 0303txt 0032 Frame A { }";
    LPD3DXANIMATIONCONTROLLER none = controller.p;
    REQUIRE(D3DXLoadMeshHierarchyFromXInMemory(still.data(), static_cast<DWORD>(still.size()), 0, &device, &alloc,
                                               nullptr, &root, &none) == D3D_OK);
    CHECK(none == nullptr);
    D3DXFrameDestroy(root, &alloc);
    // A key of an unknown type fails the load.
    const std::string bad = "xof 0303txt 0032 Frame A { } AnimationSet { Animation { { A } AnimationKey { 7; 1; 0; "
                            "1; 0;;; } } }";
    CHECK(FAILED(D3DXLoadMeshHierarchyFromXInMemory(bad.data(), static_cast<DWORD>(bad.size()), 0, &device, &alloc,
                                                    nullptr, &root, &none)));
    CHECK(root == nullptr);
    CHECK(alloc.frames == 0);
}

namespace {

bool HasAnimation(const x2::d3d9::xfile::File& file)
{
    return std::any_of(file.objects.begin(), file.objects.end(),
                       [](const x2::d3d9::xfile::Object& o) { return o.type == "AnimationSet"; });
}

void Snapshot(const D3DXFRAME* frame, std::map<std::string, D3DXMATRIX>& out)
{
    for (; frame != nullptr; frame = frame->pFrameSibling) {
        if (frame->Name) {
            out.emplace(frame->Name, frame->TransformationMatrix);
        }
        Snapshot(frame->pFrameFirstChild, out);
    }
}

bool Finite(const D3DXFRAME* frame)
{
    for (; frame != nullptr; frame = frame->pFrameSibling) {
        for (int i = 0; i < 16; ++i) {
            if (!std::isfinite(frame->TransformationMatrix.m[i / 4][i % 4])) {
                return false;
            }
        }
        if (!Finite(frame->pFrameFirstChild)) {
            return false;
        }
    }
    return true;
}

}  // namespace

// Every .x of the client with animation sets loads with its controller; every set plays (finite matrices at its start
// and middle). At time 0 most animated frames are at their rest matrix: 90,885 against 34,389 (2026-11-07), and only
// 63,122 against 62,152 with the rotation keys not conjugated, so this checks the convention.
TEST_CASE("d3dx: the client's animated .x files (X2_MASSFILE_CLIENT_DATA)")
{
    const char* folder = std::getenv("X2_MASSFILE_CLIENT_DATA");
    if (folder == nullptr || *folder == '\0') {
        MESSAGE("X2_MASSFILE_CLIENT_DATA not set: skipped");
        return;
    }
    std::string error;
    x2::kom::Mount mount([](const std::string&) {});
    REQUIRE_MESSAGE(mount.AddFolder(folder, error), error);
    std::vector<std::string> names;
    std::set<std::string> seen;
    for (const auto& archive : mount.Archives()) {
        for (const x2::kom::Entry& entry : archive->Entries()) {
            std::string lower = entry.name;
            std::transform(lower.begin(), lower.end(), lower.begin(),
                           [](char c) { return static_cast<char>(c >= 'A' && c <= 'Z' ? c - 'A' + 'a' : c); });
            if ((lower.ends_with(".x") || lower.ends_with(".y")) && seen.insert(lower).second) {
                names.push_back(entry.name);
            }
        }
    }
    auto render = NullRender();
    x2::d3d9::Device device(*render, nullptr);
    REQUIRE(device.Ok());
    Allocator alloc;
    std::atomic<std::size_t> next{0};
    std::mutex mutex, deviceMutex;
    std::size_t animated = 0, sets = 0, atRest = 0, notAtRest = 0;
    std::map<std::string, std::string> failed;
    std::vector<std::thread> threads;
    for (int t = 0; t < 8; ++t) {
        threads.emplace_back([&] {
            std::vector<std::uint8_t> bytes;
            std::string readError;
            for (std::size_t i = next++; i < names.size(); i = next++) {
                if (!mount.Read(names[i], bytes, readError) || bytes.size() < 16) {
                    continue;
                }
                const std::span<const std::byte> view(reinterpret_cast<const std::byte*>(bytes.data()), bytes.size());
                if (!x2::d3d9::xfile::ReadHeader(view)) {
                    continue;
                }
                const auto file = x2::d3d9::xfile::Read(view, readError);
                if (!file || !HasAnimation(*file)) {
                    continue;
                }
                std::string why;
                std::size_t fileSets = 0, rest = 0, moved = 0;
                LPD3DXFRAME root = nullptr;
                LPD3DXANIMATIONCONTROLLER controller = nullptr;
                HRESULT hr;
                {
                    const std::lock_guard lock(deviceMutex);
                    hr = D3DXLoadMeshHierarchyFromXInMemory(bytes.data(), static_cast<DWORD>(bytes.size()),
                                                            D3DXMESH_MANAGED, &device, &alloc, nullptr, &root,
                                                            &controller);
                }
                if (FAILED(hr) || !root) {
                    why = "D3DXLoadMeshHierarchyFromXInMemory";
                } else if (!controller) {
                    why = "no controller";
                } else {
                    std::map<std::string, D3DXMATRIX> restMatrices;
                    Snapshot(root, restMatrices);
                    fileSets = controller->GetNumAnimationSets();
                    for (UINT s = 0; s < fileSets && why.empty(); ++s) {
                        LPD3DXANIMATIONSET set = nullptr;
                        controller->GetAnimationSet(s, &set);
                        controller->SetTrackAnimationSet(0, set);
                        controller->SetTrackPosition(0, 0.0);
                        controller->AdvanceTime(0.0, nullptr);
                        for (UINT a = 0; a < set->GetNumAnimations(); ++a) {
                            LPCSTR name = nullptr;
                            set->GetAnimationNameByIndex(a, &name);
                            const auto found = restMatrices.find(name);
                            const LPD3DXFRAME frame = D3DXFrameFind(root, name);
                            if (found != restMatrices.end() && frame) {
                                (Near(frame->TransformationMatrix, found->second, 2e-3f) ? rest : moved) += 1;
                            }
                        }
                        controller->SetTrackPosition(0, set->GetPeriod() / 2);
                        controller->AdvanceTime(0.0, nullptr);
                        if (!Finite(root)) {
                            why = "matrices not finite";
                        }
                        set->Release();
                    }
                    controller->Release();
                }
                if (root) {
                    const std::lock_guard lock(deviceMutex);
                    D3DXFrameDestroy(root, &alloc);
                }
                const std::lock_guard lock(mutex);
                ++animated;
                sets += fileSets;
                atRest += rest;
                notAtRest += moved;
                if (!why.empty()) {
                    failed[names[i]] = why;
                }
            }
        });
    }
    for (std::thread& thread : threads) {
        thread.join();
    }
    MESSAGE(animated << " animated .x files, " << sets << " animation sets; at time 0, " << atRest
                     << " animated frames at their rest matrix, " << notAtRest << " elsewhere");
    std::size_t shown = 0;
    for (const auto& [name, why] : failed) {
        if (shown++ < 40) {
            MESSAGE(name << ": " << why);
        }
    }
    CHECK(failed.empty());
    CHECK(animated > 0);
    CHECK(atRest > 2 * notAtRest);
    CHECK(alloc.frames == 0);
}
