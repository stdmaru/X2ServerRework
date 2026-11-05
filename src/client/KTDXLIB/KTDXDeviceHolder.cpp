#include "stdafx.h"
#include "./KTDXDeviceHolder.h"

CKTDXDeviceHolder::CKTDXDeviceHolder(void)
{
	CustomInitialize_();
}

/*virtual*/ CKTDXDeviceHolder::~CKTDXDeviceHolder(void)
{
	CustomFinalize_();
}

void CKTDXDeviceHolder::CustomInitialize_()
{
	m_listReadyDevice.clear();
}

/*virtual*/ void CKTDXDeviceHolder::CustomFinalize_()
{
	ClearAllDevice();
}

bool CKTDXDeviceHolder::TextureReady( const WCHAR* pFileName )
{
	if( NULL == pFileName || pFileName[0] == NULL )
		return false;


	CKTDXDeviceTexture* pDevice = g_pKTDXApp->GetDeviceManager()->OpenTexture( pFileName );
	if( pDevice == NULL )
		return false;

	DeviceReady( pDevice );
	return true;
}

bool CKTDXDeviceHolder::XSkinMeshReady( const WCHAR* pFileName )
{
	if( NULL == pFileName || pFileName[0] == NULL )
		return false;

	CKTDXDeviceXSkinMesh* pDevice = g_pKTDXApp->GetDeviceManager()->OpenXSkinMesh( pFileName );
	if( pDevice == NULL )
		return false;

	DeviceReady( pDevice );
	return true;
}

bool CKTDXDeviceHolder::XMeshReady( const WCHAR* pFileName )
{
	if( NULL == pFileName || pFileName[0] == NULL )
		return false;

	CKTDXDeviceXMesh* pDevice = g_pKTDXApp->GetDeviceManager()->OpenXMesh( pFileName );
	if( pDevice == NULL )
		return false;

	DeviceReady( pDevice );
	return true;
}

bool CKTDXDeviceHolder::SoundReady( const WCHAR* pFileName )
{
	if( NULL == pFileName || pFileName[0] == NULL )
		return false;

	CKTDXDeviceSound* pDevice = g_pKTDXApp->GetDeviceManager()->OpenSound( pFileName );
	if( pDevice == NULL )
		return false;

	DeviceReady( pDevice );
	return true;
}

bool CKTDXDeviceHolder::TextureReadyInBackground( const WCHAR* pFileName, int iPriority )
{

	if( NULL == pFileName || pFileName[0] == NULL )
		return false;

	CKTDXDeviceTexture* pDevice = g_pKTDXApp->GetDeviceManager()->OpenTextureInBackground( pFileName, (CKTDXDeviceManager::EPriority) iPriority );
	if( pDevice == NULL )
		return false;

	DeviceReady( pDevice );
	return true;

}

bool CKTDXDeviceHolder::XSkinMeshReadyInBackground( const WCHAR* pFileName, int iPriority )
{
	if( NULL == pFileName || pFileName[0] == NULL )
		return false;


	CKTDXDeviceXSkinMesh* pDevice = g_pKTDXApp->GetDeviceManager()->OpenXSkinMeshInBackground( pFileName, (CKTDXDeviceManager::EPriority) iPriority );
	if( pDevice == NULL )
		return false;

	DeviceReady( pDevice );
	return true;

}

bool CKTDXDeviceHolder::XMeshReadyInBackground( const WCHAR* pFileName, int iPriority )
{

	if( NULL == pFileName || pFileName[0] == NULL )
		return false;

	CKTDXDeviceXMesh* pDevice = g_pKTDXApp->GetDeviceManager()->OpenXMeshInBackground( pFileName,  (CKTDXDeviceManager::EPriority) iPriority );
	if( pDevice == NULL )
		return false;

	DeviceReady( pDevice );
	return true;
}

bool CKTDXDeviceHolder::SoundReadyInBackground( const WCHAR* pFileName
	, int iPriority
	)
{

	if( NULL == pFileName || pFileName[0] == NULL )
		return false;

	CKTDXDeviceSound* pDevice = g_pKTDXApp->GetDeviceManager()->OpenSoundInBackground( pFileName
			, (CKTDXDeviceManager::EPriority) iPriority
		);
	if( pDevice == NULL )
		return false;

	DeviceReady( pDevice );

	return true;

}

bool CKTDXDeviceHolder::DeviceReady( CKTDXDevice* pDevice )
{
	KTDXPROFILE();

	if( NULL == pDevice )
		return false;
	m_listReadyDevice.resize( m_listReadyDevice.size() + 1 );
	m_listReadyDevice.back().m_pDevice = pDevice;
	return true;
}


// vector에서 찾아야 하니까 필요한 경우에만 사용
bool CKTDXDeviceHolder::DeviceUnready( CKTDXDevice* pDevice )
{
	KTDXPROFILE();

	if( NULL == pDevice )
		return false;

	for( CKTDXDeviceDataList::iterator iter = m_listReadyDevice.begin();
		iter != m_listReadyDevice.end(); ++iter )
	{
		if( pDevice == iter->m_pDevice )
		{
			m_listReadyDevice.erase( iter );
			return true;
		}
	}

	return false;
}

void CKTDXDeviceHolder::ClearAllDevice()
{
	m_listReadyDevice.clear();
}
