// DXUT's first-person camera (DXUTmisc.cpp), as the client used it. See Camera.h.

#include "Camera.h"


#include <algorithm>
#include <cmath>

CFirstPersonCamera::CFirstPersonCamera()
{
    const D3DXVECTOR3 eye(0.0f, 0.0f, 0.0f);
    const D3DXVECTOR3 lookAt(0.0f, 0.0f, 1.0f);
    SetViewParams(&eye, &lookAt);
    SetProjParams(D3DX_PI / 4, 1.0f, 1.0f, 1000.0f);
    D3DXMatrixInverse(&m_cameraWorld, nullptr, &m_view);
}

void CFirstPersonCamera::SetViewParams(const D3DXVECTOR3* pvEyePt, const D3DXVECTOR3* pvLookatPt)
{
    const D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
    if (pvEyePt != nullptr && pvLookatPt != nullptr) {
        m_defaultEye = m_eye = *pvEyePt;
        m_defaultLookAt = m_lookAt = *pvLookatPt;
        SetViewFromBasis(up, false);
    }
}

void CFirstPersonCamera::SetViewParams(const D3DXVECTOR3* pvEyePt, const D3DXVECTOR3* pvLookatPt,
                                       const D3DXVECTOR3* pvUpVec)
{
    if (pvEyePt != nullptr && pvLookatPt != nullptr && pvUpVec != nullptr) {
        m_defaultEye = m_eye = *pvEyePt;
        m_defaultLookAt = m_lookAt = *pvLookatPt;
        SetViewFromBasis(*pvUpVec, true);
    }
}

// The view from the eye, look-at and up vectors; yaw and pitch (and roll, with an up vector) from its basis.
void CFirstPersonCamera::SetViewFromBasis(const D3DXVECTOR3& up, bool roll)
{
    D3DXMatrixLookAtLH(&m_view, &m_eye, &m_lookAt, &up);
    D3DXMATRIX invView;
    D3DXMatrixInverse(&invView, nullptr, &m_view);
    const D3DXVECTOR3& z = *reinterpret_cast<const D3DXVECTOR3*>(&invView._31);
    m_yaw = std::atan2(z.x, z.z);
    m_pitch = -std::atan2(z.y, std::sqrt(z.z * z.z + z.x * z.x));
    if (roll) {
        const D3DXVECTOR3& upBasis = *reinterpret_cast<const D3DXVECTOR3*>(&invView._21);
        const D3DXVECTOR3& rightBasis = *reinterpret_cast<const D3DXVECTOR3*>(&invView._11);
        m_roll = -(std::atan2(upBasis.y, rightBasis.y) - D3DX_PI / 2);
    }
}

void CFirstPersonCamera::SetProjParams(FLOAT fFOV, FLOAT fAspect, FLOAT fNearPlane, FLOAT fFarPlane)
{
    D3DXMatrixPerspectiveFovLH(&m_proj, fFOV, fAspect, fNearPlane, fFarPlane);
}

void CFirstPersonCamera::SetRotateButtons(bool bLeft, bool bMiddle, bool bRight, bool bRotateWithoutButtonDown)
{
    m_rotateButtons = (bLeft ? 1 : 0) | (bMiddle ? 2 : 0) | (bRight ? 4 : 0);
    m_rotateWithoutButtonDown = bRotateWithoutButtonDown;
}

void CFirstPersonCamera::Reset()
{
    const D3DXVECTOR3 eye = m_defaultEye;
    const D3DXVECTOR3 lookAt = m_defaultLookAt;
    SetViewParams(&eye, &lookAt);
}

CFirstPersonCamera::Key CFirstPersonCamera::MapKey(WPARAM key)
{
    switch (key) {
    case VK_LEFT: case 'A': case VK_NUMPAD4: return Left;
    case VK_RIGHT: case 'D': case VK_NUMPAD6: return Right;
    case VK_UP: case 'W': case VK_NUMPAD8: return Forward;
    case VK_DOWN: case 'S': case VK_NUMPAD2: return Backward;
    case VK_PRIOR: case 'E': case VK_NUMPAD9: return Up;
    case VK_NEXT: case 'Q': case VK_NUMPAD3: return Down;
    case 'Z': return RollLeft;
    case 'C': return RollRight;
    case 'X': return RollCenter;
    case VK_HOME: return ResetKey;
    default: return None;
    }
}

LRESULT CFirstPersonCamera::HandleMessages(HWND /*hWnd*/, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    const POINT cursor = {static_cast<short>(LOWORD(lParam)), static_cast<short>(HIWORD(lParam))};
    switch (uMsg) {
    case WM_KEYDOWN:
    case WM_KEYUP:
        if (const Key key = MapKey(wParam); key != None)
            m_keys[key] = uMsg == WM_KEYDOWN;
        break;
    case WM_LBUTTONDOWN: case WM_LBUTTONDBLCLK: m_buttonsDown |= 1; m_mouse = m_mouseLastFrame = cursor; return TRUE;
    case WM_MBUTTONDOWN: case WM_MBUTTONDBLCLK: m_buttonsDown |= 2; m_mouse = m_mouseLastFrame = cursor; return TRUE;
    case WM_RBUTTONDOWN: case WM_RBUTTONDBLCLK: m_buttonsDown |= 4; m_mouse = m_mouseLastFrame = cursor; return TRUE;
    case WM_LBUTTONUP: m_buttonsDown &= ~1; break;
    case WM_MBUTTONUP: m_buttonsDown &= ~2; break;
    case WM_RBUTTONUP: m_buttonsDown &= ~4; break;
    case WM_MOUSEMOVE: m_mouse = cursor; break;
    default: break;
    }
    return FALSE;
}

void CFirstPersonCamera::FrameMove(FLOAT fElapsedTime)
{
    if (m_keys[ResetKey])
        Reset();

    const bool rotating = (m_rotateButtons & m_buttonsDown) != 0 || m_rotateWithoutButtonDown;
    if (rotating) {
        const D3DXVECTOR2 delta(static_cast<float>(m_mouse.x - m_mouseLastFrame.x),
                                static_cast<float>(m_mouse.y - m_mouseLastFrame.y));
        m_mouseDelta = m_mouseDelta * 0.5f + delta * 0.5f;
        m_yaw += m_mouseDelta.x * m_rotationScaler;
        m_pitch = std::clamp(m_pitch + m_mouseDelta.y * m_rotationScaler, -D3DX_PI / 2, D3DX_PI / 2);
    }
    m_mouseLastFrame = m_mouse;

    D3DXVECTOR3 accel(0.0f, 0.0f, 0.0f);
    if (m_enablePositionMovement) {
        accel.z = (m_keys[Forward] ? 1.0f : 0.0f) - (m_keys[Backward] ? 1.0f : 0.0f);
        accel.x = (m_keys[Right] ? 1.0f : 0.0f) - (m_keys[Left] ? 1.0f : 0.0f);
        if (m_enableYAxisMovement)
            accel.y = (m_keys[Up] ? 1.0f : 0.0f) - (m_keys[Down] ? 1.0f : 0.0f);
        if (m_keys[RollLeft])
            m_roll += 0.01f;
        else if (m_keys[RollRight])
            m_roll -= 0.01f;
        if (m_keys[RollCenter])
            m_roll = 0.0f;
    }
    D3DXVec3Normalize(&accel, &accel);
    const D3DXVECTOR3 posDelta = accel * m_moveScaler * fElapsedTime;

    D3DXMATRIX rotation;
    D3DXMatrixRotationYawPitchRoll(&rotation, m_yaw, m_pitch, m_roll);
    const D3DXVECTOR3 localUp(0.0f, 1.0f, 0.0f);
    const D3DXVECTOR3 localAhead(0.0f, 0.0f, 1.0f);
    D3DXVECTOR3 worldUp, worldAhead, posDeltaWorld;
    D3DXVec3TransformCoord(&worldUp, &localUp, &rotation);
    D3DXVec3TransformCoord(&worldAhead, &localAhead, &rotation);
    if (!m_enableYAxisMovement)
        D3DXMatrixRotationYawPitchRoll(&rotation, m_yaw, 0.0f, 0.0f);
    D3DXVec3TransformCoord(&posDeltaWorld, &posDelta, &rotation);

    m_eye += posDeltaWorld;
    m_lookAt = m_eye + worldAhead;
    D3DXMatrixLookAtLH(&m_view, &m_eye, &m_lookAt, &worldUp);
    D3DXMatrixInverse(&m_cameraWorld, nullptr, &m_view);
}
