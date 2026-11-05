// IMM32 names for the client's IME edit box (KTDXLIB/KTDGUIIMEEditBox, roadmap Phase 3): the input-context handles,
// CANDIDATELIST, INPUTCONTEXT, the IMN_*, NI_*, CPS_*, IME_CMODE_*, ATTR_* and IMM_ERROR_* constants, and the winnt.h
// language IDs the IME code switches on.
//
// Only the types and constants: no IMM32 function is declared or linked here. The client calls the IMM32 functions it
// needs through function pointers, which point to the emulation in src/client/DXUT/Ime.h on every platform (SDL3 owns
// the real IMM32 on Windows). See docs/porting/client-ime.md.
//
// On Windows they come from <imm.h> and <immdev.h>. Elsewhere they are defined here with the values and layouts they have
// on Windows x64.
#pragma once

#include "x2/platform/types.h"
#include "x2/platform/window_messages.h"

#if defined(_WIN32)

#include <imm.h>
#include <immdev.h>  // INPUTCONTEXT (imm.h no longer declares it)

#else

struct HIMC__;
typedef HIMC__* HIMC;
struct HIMCC__;
typedef HIMCC__* HIMCC;
struct HKL__;
typedef HKL__* HKL;
struct HFONT__;
typedef HFONT__* HFONT;

typedef int* PINT;
typedef UINT* PUINT;

#define APIENTRY WINAPI

// wingdi.h, for INPUTCONTEXT::lfFont
#define LF_FACESIZE 32
typedef struct tagLOGFONTA {
    LONG lfHeight;
    LONG lfWidth;
    LONG lfEscapement;
    LONG lfOrientation;
    LONG lfWeight;
    BYTE lfItalic;
    BYTE lfUnderline;
    BYTE lfStrikeOut;
    BYTE lfCharSet;
    BYTE lfOutPrecision;
    BYTE lfClipPrecision;
    BYTE lfQuality;
    BYTE lfPitchAndFamily;
    char lfFaceName[LF_FACESIZE];
} LOGFONTA;
typedef struct tagLOGFONTW {
    LONG lfHeight;
    LONG lfWidth;
    LONG lfEscapement;
    LONG lfOrientation;
    LONG lfWeight;
    BYTE lfItalic;
    BYTE lfUnderline;
    BYTE lfStrikeOut;
    BYTE lfCharSet;
    BYTE lfOutPrecision;
    BYTE lfClipPrecision;
    BYTE lfQuality;
    BYTE lfPitchAndFamily;
    WCHAR lfFaceName[LF_FACESIZE];
} LOGFONTW;

// imm.h
typedef struct tagCANDIDATELIST {
    DWORD dwSize;       // bytes of the whole list, strings included
    DWORD dwStyle;
    DWORD dwCount;
    DWORD dwSelection;
    DWORD dwPageStart;
    DWORD dwPageSize;
    DWORD dwOffset[1];  // dwCount offsets from the start of the list to NUL-terminated WCHAR strings
} CANDIDATELIST, *PCANDIDATELIST, *LPCANDIDATELIST;

typedef struct tagCOMPOSITIONFORM {
    DWORD dwStyle;
    POINT ptCurrentPos;
    RECT rcArea;
} COMPOSITIONFORM;

typedef struct tagCANDIDATEFORM {
    DWORD dwIndex;
    DWORD dwStyle;
    POINT ptCurrentPos;
    RECT rcArea;
} CANDIDATEFORM;

// immdev.h
typedef struct tagINPUTCONTEXT {
    HWND hWnd;
    BOOL fOpen;
    POINT ptStatusWndPos;
    POINT ptSoftKbdPos;
    DWORD fdwConversion;
    DWORD fdwSentence;
    union {
        LOGFONTA A;
        LOGFONTW W;
    } lfFont;
    COMPOSITIONFORM cfCompForm;
    CANDIDATEFORM cfCandForm[4];
    HIMCC hCompStr;
    HIMCC hCandInfo;
    HIMCC hGuideLine;
    HIMCC hPrivate;
    DWORD dwNumMsgBuf;
    HIMCC hMsgBuf;
    DWORD fdwInit;
    DWORD dwReserve[3];
} INPUTCONTEXT, *PINPUTCONTEXT, *LPINPUTCONTEXT;

// ImmGetCompositionString errors
#define IMM_ERROR_NODATA  (-1)
#define IMM_ERROR_GENERAL (-2)

// GCS_COMPATTR values
#define ATTR_INPUT               0x00
#define ATTR_TARGET_CONVERTED    0x01
#define ATTR_CONVERTED           0x02
#define ATTR_TARGET_NOTCONVERTED 0x03
#define ATTR_INPUT_ERROR         0x04
#define ATTR_FIXEDCONVERTED      0x05

// ImmNotifyIME actions and their CPS_* values
#define NI_OPENCANDIDATE      0x0010
#define NI_CLOSECANDIDATE     0x0011
#define NI_SELECTCANDIDATESTR 0x0012
#define NI_CHANGECANDIDATELIST 0x0013
#define NI_FINALIZECONVERSIONRESULT 0x0014
#define NI_COMPOSITIONSTR     0x0015
#define NI_SETCANDIDATE_PAGESTART 0x0016
#define NI_SETCANDIDATE_PAGESIZE 0x0017
#define CPS_COMPLETE 0x0001
#define CPS_CONVERT  0x0002
#define CPS_REVERT   0x0003
#define CPS_CANCEL   0x0004

// WM_IME_NOTIFY wParam
#define IMN_CLOSESTATUSWINDOW    0x0001
#define IMN_OPENSTATUSWINDOW     0x0002
#define IMN_CHANGECANDIDATE      0x0003
#define IMN_CLOSECANDIDATE       0x0004
#define IMN_OPENCANDIDATE        0x0005
#define IMN_SETCONVERSIONMODE    0x0006
#define IMN_SETSENTENCEMODE      0x0007
#define IMN_SETOPENSTATUS        0x0008
#define IMN_SETCANDIDATEPOS      0x0009
#define IMN_SETCOMPOSITIONFONT   0x000A
#define IMN_SETCOMPOSITIONWINDOW 0x000B
#define IMN_SETSTATUSWINDOWPOS   0x000C
#define IMN_GUIDELINE            0x000D
#define IMN_PRIVATE              0x000E

// Conversion modes
#define IME_CMODE_ALPHANUMERIC 0x0000
#define IME_CMODE_NATIVE       0x0001
#define IME_CMODE_CHINESE      IME_CMODE_NATIVE
#define IME_CMODE_HANGUL       IME_CMODE_NATIVE
#define IME_CMODE_HANGEUL      IME_CMODE_NATIVE
#define IME_CMODE_JAPANESE     IME_CMODE_NATIVE
#define IME_CMODE_KATAKANA     0x0002
#define IME_CMODE_FULLSHAPE    0x0008

// winnt.h language IDs
#define MAKELANGID(p, s) ((static_cast<WORD>(s) << 10) | static_cast<WORD>(p))
#define PRIMARYLANGID(lgid) (static_cast<WORD>(lgid) & 0x3ff)
#define SUBLANGID(lgid) (static_cast<WORD>(lgid) >> 10)
#define LANG_NEUTRAL  0x00
#define LANG_CHINESE  0x04
#define LANG_ENGLISH  0x09
#define LANG_JAPANESE 0x11
#define LANG_KOREAN   0x12
#define SUBLANG_DEFAULT             0x01
#define SUBLANG_ENGLISH_US          0x01
#define SUBLANG_CHINESE_TRADITIONAL 0x01
#define SUBLANG_CHINESE_SIMPLIFIED  0x02

#endif  // _WIN32
