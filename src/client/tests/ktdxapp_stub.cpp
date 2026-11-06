// The engine's application object for the tests of KTDXLIB's first engine files (docs/porting/client-ktdxlib.md):
// CKTDXApp's constructor and destructor stand in for KTDXApp.cpp, which is not ported yet. The stub holds the device,
// the silent sound manager and the dynamic vertex data, which is what those files take from g_pKTDXApp; the other
// managers are null.

#include "stdafx.h"

CKTDXApp* g_pKTDXApp = NULL;
LPDIRECT3DDEVICE9 g_pd3dDevice = NULL;

CKTDXApp::CKTDXApp( HWND hWnd, HINSTANCE hInstance, IDirect3DDevice9* pd3dDevice )
{
    m_hWnd = hWnd;
    m_hInstance = hInstance;
    m_pd3dDevice = pd3dDevice;
    m_pCollision = NULL;
    m_pDeviceManager = NULL;
    m_pDIManager = NULL;
    m_pDGManager = NULL;
    m_pDSManager = new CKTDSManager( hWnd, pd3dDevice );
    m_pDVBManager = new CKTDGDynamicVBManager();

    g_pd3dDevice = pd3dDevice;
    g_pKTDXApp = this;
    CKTDGStateManager::RestoreDeviceObjects();
}

CKTDXApp::~CKTDXApp()
{
    CKTDGStateManager::InvalidateDeviceObjects();
    SAFE_DELETE( m_pDVBManager );
    SAFE_DELETE( m_pDSManager );
    g_pKTDXApp = NULL;
    g_pd3dDevice = NULL;
}
