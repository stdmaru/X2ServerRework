// Entry point of the client while KTDXLIB and X2Lib are not compiled yet (client/skeleton, roadmap Phase 3): the start
// of X2.cpp's VirtualWinMain on the DXUT replacement (src/client/DXUT), with the callbacks X2.cpp installs. It opens the
// window, runs the frame loop and delivers the translated window messages to MsgProc and KeyboardProc. X2.cpp takes
// its place once CKTDXApp and CX2Main build. See docs/porting/client-x2.md.

#include "DXUT.h"

#include <SDL3/SDL_log.h>
#include <SDL3/SDL_main.h>

#include <cmath>
#include <cstring>
#include <memory>

namespace {

bool g_traceMessages = false;  // --trace-messages: log every message MsgProc receives

const char* MessageName(UINT uMsg)
{
    switch (uMsg) {
    case WM_KEYDOWN: return "WM_KEYDOWN";
    case WM_KEYUP: return "WM_KEYUP";
    case WM_SYSKEYDOWN: return "WM_SYSKEYDOWN";
    case WM_SYSKEYUP: return "WM_SYSKEYUP";
    case WM_CHAR: return "WM_CHAR";
    case WM_IME_STARTCOMPOSITION: return "WM_IME_STARTCOMPOSITION";
    case WM_IME_COMPOSITION: return "WM_IME_COMPOSITION";
    case WM_IME_ENDCOMPOSITION: return "WM_IME_ENDCOMPOSITION";
    case WM_MOUSEMOVE: return "WM_MOUSEMOVE";
    case WM_LBUTTONDOWN: return "WM_LBUTTONDOWN";
    case WM_LBUTTONUP: return "WM_LBUTTONUP";
    case WM_LBUTTONDBLCLK: return "WM_LBUTTONDBLCLK";
    case WM_RBUTTONDOWN: return "WM_RBUTTONDOWN";
    case WM_RBUTTONUP: return "WM_RBUTTONUP";
    case WM_RBUTTONDBLCLK: return "WM_RBUTTONDBLCLK";
    case WM_MBUTTONDOWN: return "WM_MBUTTONDOWN";
    case WM_MBUTTONUP: return "WM_MBUTTONUP";
    case WM_MBUTTONDBLCLK: return "WM_MBUTTONDBLCLK";
    case WM_XBUTTONDOWN: return "WM_XBUTTONDOWN";
    case WM_XBUTTONUP: return "WM_XBUTTONUP";
    case WM_XBUTTONDBLCLK: return "WM_XBUTTONDBLCLK";
    case WM_MOUSEWHEEL: return "WM_MOUSEWHEEL";
    case WM_ACTIVATEAPP: return "WM_ACTIVATEAPP";
    case WM_SETFOCUS: return "WM_SETFOCUS";
    case WM_KILLFOCUS: return "WM_KILLFOCUS";
    case WM_SIZE: return "WM_SIZE";
    case WM_CLOSE: return "WM_CLOSE";
    default: return "WM_?";
    }
}

LRESULT CALLBACK MsgProc(HWND, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing, void*)
{
    // X2.cpp: *pbNoFurtherProcessing = g_pKTDXApp->MsgProc( hWnd, uMsg, wParam, lParam );
    *pbNoFurtherProcessing = false;
    if (g_traceMessages && uMsg != WM_MOUSEMOVE) {
        SDL_Log("%-24s 0x%04X wParam=0x%llX lParam=0x%llX", MessageName(uMsg), uMsg, static_cast<unsigned long long>(wParam),
                static_cast<unsigned long long>(static_cast<ULONG_PTR>(lParam)));
    }
    return 0;
}

void CALLBACK KeyboardProc(UINT, bool, bool, void*)
{
    // X2.cpp toggles its debug text (VK_HOME) and the help system (VK_PRIOR/VK_NEXT) here, through g_pData.
}

void CALLBACK OnFrameMove(IDirect3DDevice9*, double, float, void*)
{
    // X2.cpp: g_pKTDXApp->OnFrameMove( fTime, fElapsedTime );
}

// A test card through KTDXLIB's D3D9 device (T38) until CKTDXApp renders: a textured, alpha-blended quad in screen
// space (XYZRHW | DIFFUSE | TEX1) and an untextured triangle fan with vertex colors.
std::unique_ptr<IDirect3DTexture9> g_testTexture;

struct ScreenVertex {
    float x, y, z, rhw;
    D3DCOLOR color;
    float u, v;
};

struct ColorVertex {
    float x, y, z, rhw;
    D3DCOLOR color;
};

IDirect3DTexture9* TestTexture(IDirect3DDevice9* device)
{
    if (!g_testTexture) {
        constexpr UINT size = 256;
        g_testTexture = device->CreateTexture(size, size, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED);
        D3DLOCKED_RECT locked;
        if (!g_testTexture || !g_testTexture->LockRect(0, &locked, nullptr, 0)) {
            SDL_Log("skeleton: %s", device->LastError().c_str());
            return nullptr;
        }
        for (UINT y = 0; y < size; ++y) {
            auto* row = reinterpret_cast<DWORD*>(static_cast<BYTE*>(locked.pBits) + y * locked.Pitch);
            for (UINT x = 0; x < size; ++x) {
                const bool check = ((x / 32) + (y / 32)) % 2 == 0;
                const DWORD alpha = x < size / 8 || y < size / 8 ? 0x80 : 0xFF;     // translucent border
                row[x] = check ? D3DCOLOR_ARGB(alpha, x, y, 255 - x) : D3DCOLOR_ARGB(alpha, 32, 32, 32);
            }
        }
        g_testTexture->UnlockRect(0);
    }
    return g_testTexture.get();
}

void CALLBACK OnFrameRender(IDirect3DDevice9* device, double time, float, void*)
{
    // X2.cpp: BeginScene, g_pKTDXApp->OnFrameRender(), EndScene; DXUT presents.
    if (!device || !device->BeginScene()) {
        return;
    }
    device->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(16, 24, 48), 1.0f, 0);
    const float w = static_cast<float>(device->BackBufferWidth());
    const float h = static_cast<float>(device->BackBufferHeight());
    device->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
    device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    // The quad: 256 x 256 texels on 256 x 256 pixels, D3D9 style (-0.5 aligns texels to pixels).
    if (IDirect3DTexture9* texture = TestTexture(device)) {
        const float x = std::floor(w / 2 - 300) - 0.5f, y = std::floor(h / 2 - 128) - 0.5f;
        const D3DCOLOR white = D3DCOLOR_ARGB(255, 255, 255, 255);
        const ScreenVertex quad[4] = {{x, y, 0, 1, white, 0, 0}, {x + 256, y, 0, 1, white, 1, 0},
                                      {x, y + 256, 0, 1, white, 0, 1}, {x + 256, y + 256, 0, 1, white, 1, 1}};
        device->SetTexture(0, texture);
        device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
        device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, quad, sizeof(ScreenVertex));
    }

    // The fan: a hexagon turning, untextured (MODULATE with no texture gives the vertex color).
    ColorVertex fan[8];
    const float cx = w / 2 + 172, cy = h / 2;
    fan[0] = {cx, cy, 0, 1, D3DCOLOR_ARGB(255, 255, 255, 255)};
    const D3DCOLOR colors[3] = {D3DCOLOR_ARGB(255, 255, 64, 64), D3DCOLOR_ARGB(255, 64, 255, 64),
                                D3DCOLOR_ARGB(255, 64, 64, 255)};
    for (int i = 0; i <= 6; ++i) {
        const float a = static_cast<float>(time) + static_cast<float>(i) * D3DX_PI / 3;
        fan[i + 1] = {cx + 128 * std::cos(a), cy + 128 * std::sin(a), 0, 1, colors[i % 3]};
    }
    device->SetTexture(0, nullptr);
    device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
    device->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 6, fan, sizeof(ColorVertex));
    device->EndScene();
}

}  // namespace

int main(int argc, char* argv[])
{
    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "--trace-messages") == 0) {
            g_traceMessages = true;
        }
    }

    DXUTSetCallbackMsgProc(MsgProc);
    DXUTSetCallbackKeyboard(KeyboardProc);
    DXUTSetCallbackFrameRender(OnFrameRender);
    DXUTSetCallbackFrameMove(OnFrameMove);
    DXUTSetCursorSettings(true, true);

    if (FAILED(DXUTInit(false, false, true)) || FAILED(DXUTCreateWindow(L"X2"))) {
        return 1;
    }
    DXUTMainLoop();
    g_testTexture.reset();
    return DXUTGetExitCode();
}
