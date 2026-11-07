#pragma once

// DXUT's first-person camera (DXUTmisc.h), the part the client uses: X2Lib's free camera and its camera key frame
// editing (developer tools, X2_DEV_TOOLS and SetFreeCamera from the Lua console). Keys move (arrows/WASD/numpad,
// Q/E and Page Up/Down vertically, Home resets), Z/C roll and X levels (the client's KEYFRAME_CAMERA), and dragging
// with an enabled mouse button turns. Mouse motion comes from WM_MOUSEMOVE instead of GetCursorPos.
// See docs/porting/client-dxut.md.

#include "x2/platform/d3dx_math.h"
#include "x2/platform/win32.h"   // LRESULT, WPARAM, LPARAM, POINT (window_messages.h outside Windows)

class CFirstPersonCamera
{
public:
    CFirstPersonCamera();

    LRESULT HandleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    void FrameMove(FLOAT fElapsedTime);
    void Reset();

    void SetViewParams(const D3DXVECTOR3* pvEyePt, const D3DXVECTOR3* pvLookatPt);
    void SetViewParams(const D3DXVECTOR3* pvEyePt, const D3DXVECTOR3* pvLookatPt, const D3DXVECTOR3* pvUpVec);
    void SetProjParams(FLOAT fFOV, FLOAT fAspect, FLOAT fNearPlane, FLOAT fFarPlane);
    void SetRotateButtons(bool bLeft, bool bMiddle, bool bRight, bool bRotateWithoutButtonDown = false);
    void SetEnablePositionMovement(bool enable) { m_enablePositionMovement = enable; }
    void SetEnableYAxisMovement(bool enable) { m_enableYAxisMovement = enable; }
    void SetScalers(FLOAT fRotationScaler = 0.01f, FLOAT fMoveScaler = 5.0f)
    {
        m_rotationScaler = fRotationScaler;
        m_moveScaler = fMoveScaler;
    }

    const D3DXMATRIX* GetViewMatrix() const { return &m_view; }
    const D3DXMATRIX* GetProjMatrix() const { return &m_proj; }
    const D3DXMATRIX* GetWorldMatrix() const { return &m_cameraWorld; }
    const D3DXVECTOR3* GetWorldRight() const { return reinterpret_cast<const D3DXVECTOR3*>(&m_cameraWorld._11); }
    const D3DXVECTOR3* GetWorldUp() const { return reinterpret_cast<const D3DXVECTOR3*>(&m_cameraWorld._21); }
    const D3DXVECTOR3* GetWorldAhead() const { return reinterpret_cast<const D3DXVECTOR3*>(&m_cameraWorld._31); }
    const D3DXVECTOR3* GetEyePt() const { return reinterpret_cast<const D3DXVECTOR3*>(&m_cameraWorld._41); }
    const D3DXVECTOR3* GetLookAtPt() const { return &m_lookAt; }

private:
    enum Key { Left, Right, Forward, Backward, Up, Down, RollLeft, RollRight, RollCenter, ResetKey, KeyCount, None };
    static Key MapKey(WPARAM key);
    void SetViewFromBasis(const D3DXVECTOR3& up, bool roll);

    D3DXMATRIX m_view;
    D3DXMATRIX m_proj;
    D3DXMATRIX m_cameraWorld;   // the inverse of the view matrix
    D3DXVECTOR3 m_defaultEye, m_defaultLookAt, m_eye, m_lookAt;
    float m_yaw = 0.0f, m_pitch = 0.0f, m_roll = 0.0f;
    bool m_keys[KeyCount] = {};
    int m_buttonsDown = 0;           // 1 left, 2 middle, 4 right
    int m_rotateButtons = 0x07;
    bool m_rotateWithoutButtonDown = false;
    POINT m_mouse = {0, 0};          // the last WM_MOUSEMOVE, client pixels
    POINT m_mouseLastFrame = {0, 0};
    D3DXVECTOR2 m_mouseDelta = D3DXVECTOR2(0.0f, 0.0f);   // smoothed over two frames, as DXUT's default
    float m_rotationScaler = 0.01f;
    float m_moveScaler = 5.0f;
    bool m_enablePositionMovement = true;
    bool m_enableYAxisMovement = true;
};
