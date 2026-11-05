// Entry point of the client while KTDXLIB and X2Lib are not compiled yet (client/skeleton, roadmap Phase 3): the start
// of X2.cpp's VirtualWinMain on the DXUT replacement (src/client/DXUT), with the callbacks X2.cpp installs. It opens the
// window, runs the frame loop and delivers the translated window messages to MsgProc and KeyboardProc. X2.cpp takes
// its place once CKTDXApp and CX2Main build. See docs/porting/client-x2.md.

#include "DXUT.h"

#include <SDL3/SDL_log.h>
#include <SDL3/SDL_main.h>

#include <cstring>

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

void CALLBACK OnFrameRender(IDirect3DDevice9*, double, float, void*)
{
    // X2.cpp: BeginScene, g_pKTDXApp->OnFrameRender(), EndScene. The DXUT replacement presents a cleared frame.
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
    return DXUTGetExitCode();
}
