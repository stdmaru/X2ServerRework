#pragma once

// Rendering between two simulation ticks (T41): what is drawn is interpolated from the state of the previous tick to
// the state of the last one by the fraction of a tick elapsed since. The simulation never reads these values.

#include "x2/platform/d3dx_math.h"

namespace x2::ktdx {

// Past these, the step is a cut, a teleport or an instant turn: show the last tick as it is.
inline constexpr float kSnapDistance = 400.f;     // world units moved in one tick
inline constexpr float kSnapCosine = 0.866f;      // cos 30°: largest turn of a direction in one tick

// The view between `previous` and `last` (view matrices of a look-at camera): eye, forward and up interpolated, then
// rebuilt with D3DXMatrixLookAtLH so the result stays a rotation. False (and `out` = `last`) on a cut.
inline bool InterpolateView(const D3DXMATRIX& previous, const D3DXMATRIX& last, float t, D3DXMATRIX& out)
{
    out = last;
    if (previous == last || t >= 1.f) {
        return true;
    }
    D3DXMATRIX a, b;
    if (D3DXMatrixInverse(&a, nullptr, &previous) == nullptr || D3DXMatrixInverse(&b, nullptr, &last) == nullptr) {
        return false;
    }
    const D3DXVECTOR3 eyeA(a._41, a._42, a._43), eyeB(b._41, b._42, b._43);
    const D3DXVECTOR3 upA(a._21, a._22, a._23), upB(b._21, b._22, b._23);
    const D3DXVECTOR3 forwardA(a._31, a._32, a._33), forwardB(b._31, b._32, b._33);
    const D3DXVECTOR3 moved = eyeB - eyeA;
    if (D3DXVec3Dot(&moved, &moved) > kSnapDistance * kSnapDistance || D3DXVec3Dot(&forwardA, &forwardB) < kSnapCosine
        || D3DXVec3Dot(&upA, &upB) < kSnapCosine) {
        return false;
    }
    if (t <= 0.f) {
        out = previous;
        return true;
    }
    D3DXVECTOR3 eye, forward, up;
    D3DXVec3Lerp(&eye, &eyeA, &eyeB, t);
    D3DXVec3Lerp(&forward, &forwardA, &forwardB, t);
    D3DXVec3Lerp(&up, &upA, &upB, t);
    D3DXVec3Normalize(&forward, &forward);
    D3DXVec3Normalize(&up, &up);
    const D3DXVECTOR3 at = eye + forward;
    D3DXMatrixLookAtLH(&out, &eye, &at, &up);
    return true;
}

// Whether a world matrix moved or turned too far between two ticks to be interpolated (rows 0-2 are the axes, row 3
// the position; scale is ignored by comparing directions).
inline bool IsWorldJump(const D3DXMATRIX& previous, const D3DXMATRIX& last)
{
    const D3DXVECTOR3 moved(last._41 - previous._41, last._42 - previous._42, last._43 - previous._43);
    if (D3DXVec3Dot(&moved, &moved) > kSnapDistance * kSnapDistance) {
        return true;
    }
    for (int row = 0; row < 3; ++row) {
        D3DXVECTOR3 axisA(previous.m[row][0], previous.m[row][1], previous.m[row][2]);
        D3DXVECTOR3 axisB(last.m[row][0], last.m[row][1], last.m[row][2]);
        const float lengths = D3DXVec3Length(&axisA) * D3DXVec3Length(&axisB);
        if (lengths > 0.f && D3DXVec3Dot(&axisA, &axisB) < kSnapCosine * lengths) {
            return true;
        }
    }
    return false;
}

// Something updated at steps `previous` and `last` (and not since) is drawn `fraction` of the way between the two:
// the steps elapsed after `last`, plus the fraction `t` of the current one, over the steps between the updates. With an
// update every step this is `t`. 1 (as it is) when there is nothing to interpolate: no previous update, updates more
// than kMaxUpdateGap steps apart, or an update overdue. `stepsDone` is CKTDXApp::GetFrameMoveCount (one past the last
// step, while rendering).
inline constexpr int kMaxUpdateGap = 8;   // CX2Main's P2P batch is 7 steps; NPCs simulate at most that far apart

inline float UpdateFraction(int previous, int last, int stepsDone, float t)
{
    const int span = last - previous;
    if (previous < 0 || span <= 0 || span > kMaxUpdateGap || t >= 1.f) {
        return 1.f;
    }
    const float fraction = (static_cast<float>(stepsDone - 1 - last) + t) / static_cast<float>(span);
    return fraction < 1.f ? (fraction > 0.f ? fraction : 0.f) : 1.f;
}

// Element-wise interpolation of two matrices: exact for translation, close to a rotation for the small turns of one
// tick (IsWorldJump rules out the large ones).
inline void LerpMatrix(const D3DXMATRIX& a, const D3DXMATRIX& b, float t, D3DXMATRIX& out)
{
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            out.m[i][j] = a.m[i][j] + (b.m[i][j] - a.m[i][j]) * t;
        }
    }
}

}  // namespace x2::ktdx
