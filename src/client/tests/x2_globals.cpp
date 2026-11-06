// The engine's globals that X2/X2.cpp defines in the original (the executable is not ported yet): the device and the
// effect state manager. The tests set them as X2.cpp's device callbacks do.

#include "stdafx.h"

LPDIRECT3DDEVICE9 g_pd3dDevice = NULL;
CKTDGEffectStateManager* g_pEffectStateManager = NULL;
