#include "stdafx.h"
#include "./KTDXDeviceManager.h"


#define	KTDXDEVICEMANAGER_GC_INTERVAL_IN_MILISECS	1000

//{{ seojt // 2008-10-22, 14:22
//static bool CompareKTDGObjectAccessTime( const CKTDXDeviceManager::RESOURCE_REQUEST_NODE& left_, const CKTDXDeviceManager::RESOURCE_REQUEST_NODE& right_ )
//{
//	CKTDGObjectPtr pLeft = left_.m_pObject.lock();
//	CKTDGObjectPtr pRight = right_.m_pObject.lock();
//
//	ASSERT( pLeft != NULL );
//	ASSERT( pRight != NULL );
//
//	return pLeft->GetLastAccessTime() >= pRight->GetLastAccessTime();
//
//}//CompareKTDGObjectAccessTime()
//}} seojt // 2008-10-22, 14:22


#if X2_DEV_TOOLS // 2008-10-24
IMPLEMENT_HELP_SYSTEM( CKTDXDeviceManager )
{
    CKTDXDeviceManager* pDeviceManager = g_pKTDXApp->GetDeviceManager();
    if ( pDeviceManager )
        pDeviceManager->Debug_RenderHelp( 16, 16 );
}//IMPLEMENT_HELP_SYSTEM( CKTDXDeviceManager )
#endif // KHELP_SYSTEM_TEST // 2008-10-24


CKTDXDeviceData::CKTDXDeviceData( const CKTDXDeviceData& rhs_ )
	: m_pDevice( NULL )
{
	m_pDevice = rhs_.m_pDevice;
	if ( m_pDevice )
		m_pDevice->RefDevice();
}

CKTDXDeviceData& CKTDXDeviceData::operator = ( const CKTDXDeviceData& rhs_ )
{
	CKTDXDevice* pDevice = m_pDevice;
	if ( pDevice != rhs_.m_pDevice )
	{
		m_pDevice = rhs_.m_pDevice;
		if ( m_pDevice )
			m_pDevice->RefDevice();
		SAFE_CLOSE( pDevice );
	}//if
	return	*this;
}

CKTDXDeviceData::~CKTDXDeviceData()
{
	SAFE_CLOSE( m_pDevice );
}

///-----------------------------------------------------------------------------
//
// constructor.
CKTDXDeviceManager::CKTDXDeviceManager( IDirect3DDevice9* pd3dDevice )
{
	// The game folder's subfolders for user files, music and movies; with X2_DEV_TOOLS also its data/ loose assets.
	m_MassFileManager.SetSubDirectory();
	m_TexDetailLevel = CKTDXDeviceTexture::DL_HIGH;
	m_HighTexName.clear();

    m_dwBackgroundThreadLoadSize = 0;

	InitializeCriticalSection( &m_DeviceLock );


    _InitializeThreadRelatedObject();
//#ifdef BACKGROUND_LOADING_TEST // 2008-10-15
    // 리소스 로딩 쓰레드를 시작한다.
    __super::BeginThread( NULL, NULL, true );
    //SuspendThread();
    //SuspendThread();
//#endif // BACKGROUND_LOADING_TEST // 2008-10-15
    //}} seojt // 2008-10-14, 16:18

}//CKTDXDeviceManager::CKTDXDeviceManager()


// destructor.
CKTDXDeviceManager::~CKTDXDeviceManager(void)
{



	//{{ seojt // 2008-10-14, 16:20
    // 리소스 로딩 쓰레드를 종료시킨다.
    if ( __super::GetThreadHandle() != NULL )
    {
        ::ResumeThread( __super::GetThreadHandle() );
    }//if
    if ( m_ahThreadEvents[ TET_KILL_THREAD ] != NULL )
        ::SetEvent( m_ahThreadEvents[ TET_KILL_THREAD ] );
    __super::EndThread();
    _FinalizeThreadRelatedObject(); // thread 내부에서 사용하는 object가 있으므로, thread 종료후에 호출해야 한다.
	//}} seojt // 2008-10-14, 16:20

	EnterCriticalSection( &m_DeviceLock );



	DeviceMap::iterator mit;
	for ( mit = m_DeviceMap.begin(); mit != m_DeviceMap.end(); mit++ )
	{
		SAFE_DELETE( mit->second );
	}
	m_DeviceMap.clear();

	LeaveCriticalSection( &m_DeviceLock );

	DeleteCriticalSection( &m_DeviceLock );
}//CKTDXDeviceManager::~CKTDXDeviceManager()


/*virtual*/
HRESULT CKTDXDeviceManager::OnFrameMove( double fTime, float fElapsedTime )
{
	if ( g_pKTDXApp->IsFirstFrameOfSimulationLoop() == true )
	{
		ProcessForegroundQueue( fElapsedTime );
	}//if
	return S_OK;
}

void    CKTDXDeviceManager::ProcessForegroundQueue( float fElapsedTime )
{
    CKTDXDevice* pDevice = NULL;
    m_iForegroundLoadCounter++;
    if ( m_iForegroundLoadCounter > 3 )
    {
		m_iForegroundLoadCounter = 0;
        PendQueue& pendQueue = m_aPendQueue[ PRIORITY_BLOCKING ];
        CSLock locker( pendQueue.m_csPendingDevice );
        if ( pendQueue.m_listPendingDevice.empty() == false )
        {
            pDevice = pendQueue.m_listPendingDevice.front();
            pendQueue.m_listPendingDevice.pop_front();
        }//if
    }
    if ( pDevice != NULL )
    {
        LoadAndCloseDevice( pDevice
			, false
			);
    }//if

}//CKTDXDeviceManager::OnFrameMove()
HRESULT CKTDXDeviceManager::OnResetDevice()
{
	CSLock locker( m_DeviceLock );

	DeviceMap::iterator i;
	for ( i = m_DeviceMap.begin(); i != m_DeviceMap.end(); i++ )
	{
        // 이럴 필요는 없지만 실제로 크래쉬가 발생하는 루틴이 여기라... robobeg
        // OnLostDevice, OnResetDevice 처리하는 device 는 CKTDXDeviceRenderTargetTexture 뿐이다.
        if ( i->second != NULL && i->second->GetDeviceType() == CKTDXDevice::DT_RENDER_TARGET_TEXTURE )
		    i->second->OnResetDevice();
	}



	return S_OK;
}

HRESULT CKTDXDeviceManager::OnLostDevice()
{
	CSLock locker( m_DeviceLock );

	DeviceMap::iterator i;
	for ( i = m_DeviceMap.begin(); i != m_DeviceMap.end(); i++ )
	{
        // 이럴 필요는 없지만 실제로 크래쉬가 발생하는 루틴이 여기라... robobeg
        // OnLostDevice, OnResetDevice 처리하는 device 는 CKTDXDeviceRenderTargetTexture 뿐이다.
        if ( i->second != NULL && i->second->GetDeviceType() == CKTDXDevice::DT_RENDER_TARGET_TEXTURE )
		    i->second->OnLostDevice();
	}



	return S_OK;
}


bool    CKTDXDeviceManager::BeginFullSync()
{
    LONG    lFullSync = ::InterlockedCompareExchange( &m_lFullSyncStatus, 1, 0 );
    if ( lFullSync != 0 )
        return false;
    if ( m_ahThreadEvents[ TET_PEND_DEVICE ] != NULL )
    {
        ::SetEvent( m_ahThreadEvents[ TET_PEND_DEVICE ] );
    }//if
    if ( m_hFullSyncEventForMainThread != NULL )
    {
        ::WaitForSingleObject( m_hFullSyncEventForMainThread, INFINITE );
    }//if

    return true;
}//CKTDXDeviceManager::BeginFullSync()


bool    CKTDXDeviceManager::EndFullSync()
{
    LONG    lFullSync = ::InterlockedCompareExchange( &m_lFullSyncStatus, 0, 2 );
    if ( lFullSync != 2 )
        return false;
    if ( m_hFullSyncEventForDeviceThread != NULL )
    {
        ::SetEvent( m_hFullSyncEventForDeviceThread );
    }//if

	return true;
}//CKTDXDeviceManager::EndFullSync()




void CKTDXDeviceManager::SetTextureDetailLevel( CKTDXDeviceTexture::DETAIL_LEVEL detailLevel )
{
    BeginFullSync();

	CSLock locker( m_DeviceLock );


	m_TexDetailLevel = detailLevel;
	DeviceMap::iterator i;
	for ( i = m_DeviceMap.begin(); i != m_DeviceMap.end(); i++ )
	{
		if( i->second->GetDeviceType() == CKTDXDevice::DT_TEXTURE )
		{
			CKTDXDeviceTexture* pTex = (CKTDXDeviceTexture*)i->second;
			pTex->SetDetailLevel( detailLevel );
		}
	}

    g_pKTDXApp->SkipFrame();

    EndFullSync();
}

void CKTDXDeviceManager::CloseDevice( const WCHAR* deviceID )
{
    if ( deviceID == NULL || deviceID[0] == NULL )
        return;

    WCHAR    deviceIDUpper[MAX_PATH];
    MakeUpperCase( deviceIDUpper, ARRAY_SIZE( deviceIDUpper ), deviceID );

    CKTDXDevice*    pDeviceToBeDestroyed = NULL;
    {
		CSLock locker( m_DeviceLock );

		DeviceMap::iterator device;
        device = m_DeviceMap.find( deviceIDUpper );

		if( device == m_DeviceMap.end() )
		{
			return;
		}
		else
		{
            ASSERT( device->second != NULL );
			if ( device->second->UnrefDevice() == true )
			{
                pDeviceToBeDestroyed = device->second;
                m_DeviceMap.erase( device );
			}
		}
    }
    if ( pDeviceToBeDestroyed != NULL )
    {
        SAFE_DELETE( pDeviceToBeDestroyed );
    }//if

}
void    CKTDXDeviceManager::LoadAndCloseDevice( CKTDXDevice* pDevice_
				, bool bBackgroundQueueing
				)
{
    if ( pDevice_ == NULL )
        return;

    bool    bDelete = false;
    {
		CSLock locker( m_DeviceLock );

		DeviceMap::iterator device;
        device = m_DeviceMap.find( pDevice_->GetDeviceID() );

        if ( device == m_DeviceMap.end() )
        {
            return;
        }
        else
        {
            ASSERT( device->second != NULL );
            ASSERT( device->second == pDevice_ );
            if ( device->second != pDevice_ )
                return;
            if ( pDevice_->UnrefDeviceIfReferedOnce() == true )
            {
                bDelete = true;
                m_DeviceMap.erase( device );
            }//if
        }//if.. else..
    }

    if ( bDelete == true )
    {
        SAFE_DELETE( pDevice_ );
        return;
    }//if

    pDevice_->LoadDevice(
			bBackgroundQueueing
		);

    bool    bSleep = false;
    {
		CSLock locker( m_DeviceLock );

		DeviceMap::iterator device;
        device = m_DeviceMap.find( pDevice_->GetDeviceID() );

        if ( device == m_DeviceMap.end() )
        {
            return;
        }
        else
        {
            ASSERT( device->second != NULL );
            ASSERT( device->second == pDevice_ );
            if ( device->second == NULL
                || device->second != pDevice_ )
                return;
			if ( pDevice_->UnrefDevice() == true )
			{
                bDelete = true;
                m_DeviceMap.erase( device );
			}
            else if ( bBackgroundQueueing == true )
            {
                m_dwBackgroundThreadLoadSize += pDevice_->GetSize();
                if ( pDevice_->GetDeviceType() != CKTDXDevice::DT_XET
                    && m_dwBackgroundThreadLoadSize >= ( 512 * 1024 ) )
                {
                    m_dwBackgroundThreadLoadSize = 0;
                    bSleep = true;
                }
            }
        }//if.. else..
    }

    if ( bSleep == true )
    {

            Sleep( 0 );
    }//if

    if ( bDelete == true )
    {
        SAFE_DELETE( pDevice_ );
        return;
    }//if

}//CKTDXDeviceManager::LoadAndCloseDevice()


CKTDXDevice*     CKTDXDeviceManager::OpenDevice( wstring deviceID, CKTDXDevice::DEVICETYPE eType )
{
   if ( deviceID.empty() )
   		return NULL;

    MakeUpperCase( deviceID );
    CKTDXDevice* pDevice = NULL;
    {
		CSLock locker( m_DeviceLock );
		DeviceMap::iterator device;
        device = m_DeviceMap.find( deviceID );
		if( device == m_DeviceMap.end() )
            return NULL;
        pDevice = device->second;
        ASSERT( pDevice != NULL );
        if ( pDevice == NULL || pDevice->GetDeviceType() != eType )
            return NULL;
        pDevice->RefDevice();
    }
    if ( pDevice->LoadDevice() == true )
    {
        return pDevice;
    }
    return NULL;
}
CKTDXDevice*    CKTDXDeviceManager::CreateAndOpenDevice( CKTDXDevice* pDevice )
{
    if ( pDevice == NULL || pDevice->GetDeviceID().empty() == true )
    {
        ErrorLogMsg( KEM_ERROR70, "" );
        SAFE_DELETE( pDevice );
        return NULL;
    }//if
    CKTDXDevice::DEVICETYPE  eDeviceType = pDevice->GetDeviceType();

	{
        bool            bNew = false;
        CKTDXDevice*    pActualDevice = NULL;
        {
	        CSLock locker( m_DeviceLock );
            std::pair<DeviceMap::iterator, bool> ret = m_DeviceMap.insert( DeviceMap::value_type( pDevice->GetDeviceID(), pDevice ) );
            bNew = ret.second;
            pActualDevice = ret.first->second;
            if ( pActualDevice != NULL )
            {
                if ( pActualDevice->GetDeviceType() == eDeviceType )
                    pActualDevice->RefDevice();
                else
                    pActualDevice = NULL;
            }//if
        }
        if( bNew == false )
		{
			ErrorLogMsg( KEM_ERROR70, pDevice->GetDeviceID().c_str() );
            SAFE_DELETE( pDevice );
		}
        if ( pActualDevice == NULL )
            return NULL;
        if ( pActualDevice->LoadDevice() == true )
        {
            CKTDXDevice::EDeviceState eDeviceState = pActualDevice->GetDeviceState();
            ASSERT( eDeviceState != CKTDXDevice::DEVICE_STATE_LOADING );
            return pActualDevice;
        }//if
        return NULL;
	}

}


CKTDXDevice*     CKTDXDeviceManager::PendDevice( const WCHAR* deviceID, CKTDXDevice::DEVICETYPE eType, EPriority ePriority_ )
{
    if ( deviceID == NULL || deviceID[0] == NULL )
        return NULL;

    if ( false == ( (int) eType >= 0 && (int) eType < (int) CKTDXDevice::DT_NUM ) )
        return NULL;

    if ( false == ( (int) ePriority_ >= 0 && (int) ePriority_ < (int) PRIORITY_NUM ) )
        return NULL;

    WCHAR    deviceIDUpper[MAX_PATH];
    MakeUpperCase( deviceIDUpper, ARRAY_SIZE( deviceIDUpper ), deviceID );

    CKTDXDevice* pDevice = NULL;
    {
		CSLock locker( m_DeviceLock );
		DeviceMap::iterator device;
        device = m_DeviceMap.find( deviceIDUpper );
		if( device == m_DeviceMap.end() )
            return NULL;
        pDevice = device->second;
        ASSERT( pDevice != NULL );
        if ( pDevice == NULL || pDevice->GetDeviceType() != eType )
            return NULL;
        pDevice->RefDevice();
    }
    CKTDXDevice::EDeviceState eDeviceState = pDevice->PendDevice();
    switch( eDeviceState )
    {
    case CKTDXDevice::DEVICE_STATE_INIT:
        pDevice->RefDevice();

        {
            PendQueue& pendQueue = m_aPendQueue[ ePriority_ ];
            {
		        CSLock locker( pendQueue.m_csPendingDevice );
                pendQueue.m_listPendingDevice.push_back( pDevice );
            }
            if ( ePriority_ != PRIORITY_BLOCKING )
            {
                if ( m_ahThreadEvents[ TET_PEND_DEVICE ] != NULL )
                    ::SetEvent( m_ahThreadEvents[ TET_PEND_DEVICE ] );
            }//if
        }
        break;
    //case CKTDXDevice::DEVICE_STATE_PENDING:
    case CKTDXDevice::DEVICE_STATE_LOADING:
    case CKTDXDevice::DEVICE_STATE_LOADED:
        break;
    case CKTDXDevice::DEVICE_STATE_FAILED:
        return NULL;
    }
    return  pDevice;
}

CKTDXDevice*    CKTDXDeviceManager::CreateAndPendDevice( CKTDXDevice* pDevice, EPriority ePriority_ )
{
    if ( pDevice == NULL || pDevice->GetDeviceID().empty() == true )
    {
        ErrorLogMsg( KEM_ERROR70, "" );
        SAFE_DELETE( pDevice );
        return NULL;
    }//if

    if ( false == ( ePriority_ >= EPriority( 0 ) && ePriority_ < PRIORITY_NUM ) )
    {
        ErrorLogMsg( KEM_ERROR70, "" );
        SAFE_DELETE( pDevice );
        return NULL;
    }//if

    CKTDXDevice::DEVICETYPE  eDeviceType = pDevice->GetDeviceType();

    bool            bNew = false;
    CKTDXDevice*    pActualDevice = NULL;
    {
	    CSLock locker( m_DeviceLock );
        std::pair<DeviceMap::iterator, bool> ret = m_DeviceMap.insert( DeviceMap::value_type( pDevice->GetDeviceID(), pDevice ) );
        bNew = ret.second;
        pActualDevice = ret.first->second;
        if ( pActualDevice != NULL )
        {
            if ( pActualDevice->GetDeviceType() == eDeviceType )
                pActualDevice->RefDevice();
            else
                pActualDevice = NULL;
        }//if
    }
    if( bNew == false )
	{
		ErrorLogMsg( KEM_ERROR70, pDevice->GetDeviceID().c_str() );
        SAFE_DELETE( pDevice );
	}
    if ( pActualDevice == NULL )
        return NULL;

    CKTDXDevice::EDeviceState eDeviceState = pActualDevice->PendDevice();
    switch( eDeviceState )
    {
    case CKTDXDevice::DEVICE_STATE_INIT:
        pActualDevice->RefDevice();
        {
            PendQueue& pendQueue = m_aPendQueue[ ePriority_ ];
            {
		        CSLock locker( pendQueue.m_csPendingDevice );
                pendQueue.m_listPendingDevice.push_back( pActualDevice );
            }
            if ( ePriority_ != PRIORITY_BLOCKING )
            {
                if ( m_ahThreadEvents[ TET_PEND_DEVICE ] != NULL )
                    ::SetEvent( m_ahThreadEvents[ TET_PEND_DEVICE ] );
            }//if
        }
        break;
    //case CKTDXDevice::DEVICE_STATE_PENDING:
    case CKTDXDevice::DEVICE_STATE_LOADING:
    case CKTDXDevice::DEVICE_STATE_LOADED:
        break;
    case CKTDXDevice::DEVICE_STATE_FAILED:
        return NULL;
    }
    return  pActualDevice;
}




CKTDXDeviceTexture*	CKTDXDeviceManager::OpenTexture( wstring deviceID, D3DFORMAT texFormat, bool bAlwaysHigh
	#if X2_DEV_TOOLS
	, bool bLog /*= true*/
	#endif // ADD_RESOURCE_ERROR_LOG
	)
{
	if( deviceID.empty() )
	{
		return NULL;
	}

    MakeUpperCase( deviceID );

	CKTDXDeviceTexture* pTexture = static_cast<CKTDXDeviceTexture*> ( OpenDevice( deviceID, CKTDXDevice::DT_TEXTURE ) );

	if( pTexture == NULL && m_MassFileManager.IsValidFile( deviceID ) == true )
	{
		for( int i = 0; i < (int)m_HighTexName.size(); i++ )
		{
			const wstring& texName = m_HighTexName[i];
			if( texName.size() != 0 && deviceID.find( texName.c_str() ) != -1 )
			{
				bAlwaysHigh = true;
				break;
			}
		}

		pTexture = new CKTDXDeviceTexture( g_pKTDXApp->GetDevice(), deviceID, m_TexDetailLevel, texFormat, bAlwaysHigh );
        pTexture = static_cast<CKTDXDeviceTexture*> ( CreateAndOpenDevice( pTexture ) );
	}


	ASSERT( NULL == pTexture || CKTDXDevice::DT_TEXTURE == pTexture->GetDeviceType() );

#if X2_DEV_TOOLS
	if( true == bLog && NULL == pTexture )
	{
		if( m_setEmptyDeviceID.end() == m_setEmptyDeviceID.find( deviceID ) )
		{
			m_setEmptyDeviceID.insert( deviceID );

			wstring errorMsg;
			errorMsg = L"OpenTexture실패! 파일 이름: ";
			errorMsg += deviceID;
			ErrorLogMsg( KEM_ERROR429, errorMsg.c_str() );
		}
	}
#endif //ADD_RESOURCE_ERROR_LOG
	return pTexture;
}

CKTDXDeviceXSkinMesh* CKTDXDeviceManager::OpenXSkinMesh( wstring deviceID, wstring moveBoneName )
{
	if( true == deviceID.empty() )
		return NULL;

	MakeUpperCase( deviceID );

	CKTDXDeviceXSkinMesh* pXSkinMesh = static_cast<CKTDXDeviceXSkinMesh*>( OpenDevice( deviceID, CKTDXDevice::DT_XSKIN_MESH ) );


	if( pXSkinMesh == NULL && m_MassFileManager.IsValidFile( deviceID ) == true )
	{
		pXSkinMesh = new CKTDXDeviceXSkinMesh( g_pKTDXApp->GetDevice(), deviceID, moveBoneName );
        ASSERT( pXSkinMesh != NULL );
        pXSkinMesh = static_cast<CKTDXDeviceXSkinMesh*>( CreateAndOpenDevice( pXSkinMesh ) );
	}

	ASSERT( NULL == pXSkinMesh || CKTDXDevice::DT_XSKIN_MESH == pXSkinMesh->GetDeviceType() );

#if X2_DEV_TOOLS
	if( NULL == pXSkinMesh )
	{
		if( m_setEmptyDeviceID.end() == m_setEmptyDeviceID.find( deviceID ) )
		{
			m_setEmptyDeviceID.insert( deviceID );

			wstring errorMsg;
			errorMsg = L"OpenXSkinMesh실패! 파일 이름: ";
			errorMsg += deviceID;
			ErrorLogMsg( KEM_ERROR428, errorMsg.c_str() );
		}
	}
#endif //ADD_RESOURCE_ERROR_LOG
	return pXSkinMesh;
}







CKTDXDeviceXMesh* CKTDXDeviceManager::OpenXMesh( wstring deviceID )
{
	KTDXPROFILE();
	if( true == deviceID.empty() )
		return NULL;

	MakeUpperCase(deviceID);

   CKTDXDeviceXMesh* pXMesh = static_cast<CKTDXDeviceXMesh*>( OpenDevice( deviceID, CKTDXDevice::DT_XMESH ) );

    /** m_MassFileManager에서 관리 대상이 되는 valid한 파일인지를 검사한다.
        IsValidFile()내부에서 파일을 모두 읽는데, 파일의 존재 여부만을 check하도록 수정했다.
        - jintaeks on 2008-10-17, 16:35 */
	if( pXMesh == NULL && m_MassFileManager.IsValidFile( deviceID ) == true )
	{
		pXMesh = new CKTDXDeviceXMesh( g_pKTDXApp->GetDevice(), deviceID );
        ASSERT( pXMesh != NULL );
        pXMesh = static_cast<CKTDXDeviceXMesh*>( CreateAndOpenDevice( pXMesh ) );
	}//if

	ASSERT( NULL == pXMesh || CKTDXDevice::DT_XMESH == pXMesh->GetDeviceType() );

#if X2_DEV_TOOLS
	if( NULL == pXMesh )
	{
		if( m_setEmptyDeviceID.end() == m_setEmptyDeviceID.find( deviceID ) )
		{
			m_setEmptyDeviceID.insert( deviceID );

			wstring errorMsg;
			errorMsg = L"OpenXMesh실패! 파일 이름: ";
			errorMsg += deviceID;
			ErrorLogMsg( KEM_ERROR428, errorMsg.c_str() );
		}
	}
#endif //ADD_RESOURCE_ERROR_LOG
	return pXMesh;
}//CKTDXDeviceManager::OpenXMesh()

CKTDXDeviceRenderTargetTexture*	CKTDXDeviceManager::OpenRenderTargetTexture( wstring deviceID, int width, int height, D3DFORMAT fmt )
{
	if ( deviceID.empty() )
		return NULL;
	MakeUpperCase(deviceID);

	CKTDXDeviceRenderTargetTexture* pRenderTargetTexture = static_cast<CKTDXDeviceRenderTargetTexture*> (
        OpenDevice( deviceID, CKTDXDevice::DT_RENDER_TARGET_TEXTURE ) );

	if( pRenderTargetTexture == NULL )
	{
		pRenderTargetTexture = new CKTDXDeviceRenderTargetTexture( g_pKTDXApp->GetDevice(), deviceID, width, height, fmt );
        ASSERT( pRenderTargetTexture != NULL );
        pRenderTargetTexture = static_cast<CKTDXDeviceRenderTargetTexture*>(
            CreateAndOpenDevice( pRenderTargetTexture ) );
	}

	return pRenderTargetTexture;
}

CKTDXDeviceSound* CKTDXDeviceManager::OpenSound( wstring deviceID, int bufCount, bool bUse3D, float fMax3DDistance /*= -1.f*/ )
{
	if( true == deviceID.empty() )
	{
		return NULL;
	}

	MakeUpperCase(deviceID);

	CKTDXDeviceSound* pSound = static_cast<CKTDXDeviceSound*>( OpenDevice( deviceID, CKTDXDevice::DT_SOUND ) );

	if( pSound == NULL && m_MassFileManager.IsValidFile( deviceID ) == true )
	{
		if( g_pKTDXApp->GetDSManager()->GetInit() == false )
		{
			return NULL;
		}
		else
		{
			pSound = new CKTDXDeviceSound( g_pKTDXApp->GetDSManager()->GetSystem(), deviceID, bUse3D );
            ASSERT( pSound != NULL );
            pSound = static_cast<CKTDXDeviceSound*>( CreateAndOpenDevice( pSound ) );
		}
	}

	if( NULL != pSound &&
		fMax3DDistance > 0.f )
	{
		pSound->SetMax3DDistance( fMax3DDistance );
	}

	return pSound;
}


CKTDXDeviceXET*	CKTDXDeviceManager::OpenXET( wstring deviceID, bool bWithoutFile /* = false */ )
{
	if( true == deviceID.empty() )
		return NULL;
	MakeUpperCase(deviceID);

	CKTDXDeviceXET* pXET = static_cast<CKTDXDeviceXET*>( OpenDevice( deviceID, CKTDXDevice::DT_XET ) );

	if( pXET == NULL )
	{
		if( bWithoutFile == true || m_MassFileManager.IsValidFile( deviceID ) == true )
		{
			pXET = new CKTDXDeviceXET( deviceID.c_str() );
            ASSERT( pXET != NULL );
			pXET->SetWithoutFile( bWithoutFile );
            pXET = static_cast<CKTDXDeviceXET*>( CreateAndOpenDevice( pXET ) );
		}
	}
#if X2_DEV_TOOLS
	if( NULL == pXET )
	{
		if( m_setEmptyDeviceID.end() == m_setEmptyDeviceID.find( deviceID ) )
		{
			m_setEmptyDeviceID.insert( deviceID );

			wstring errorMsg;
			errorMsg = L"OpenXET실패! 파일 이름: ";
			errorMsg += deviceID;
// 			CKTDXDeviceManager::CreateAndOpenDevice()
// 			CKTDXDeviceXSkinMesh::XSkinMeshProxy::Load_LuaData()
// 			위 함수를 통해 파일 생성 시 무조건 Xet를 열도록 처리되어 있기 때문에
// 			의미 있는 에러로그가 될 수 없어서 제거.
			ErrorLogMsg( KEM_ERROR431, errorMsg.c_str() );

		}
	}
#endif //ADD_RESOURCE_ERROR_LOG
	return pXET;
}

CKTDXDeviceTexture*	CKTDXDeviceManager::OpenTextureInBackground( wstring  deviceID_, EPriority ePriority, D3DFORMAT texFormat, bool bAlwaysHigh )
{
	if( deviceID_.empty() )
		return NULL;
    if ( false == ( (int) ePriority >= 0 && (int) ePriority < (int) PRIORITY_NUM ) )
        return NULL;
	if ( ePriority == PRIORITY_BLOCKING )
		return	OpenTexture( deviceID_, texFormat, bAlwaysHigh );

    WCHAR    deviceIDUpper[MAX_PATH];
	MakeUpperCase( deviceIDUpper, ARRAY_SIZE( deviceIDUpper ), deviceID_.c_str() );
	CKTDXDeviceTexture* pTexture = static_cast<CKTDXDeviceTexture*> ( PendDevice( deviceIDUpper, CKTDXDevice::DT_TEXTURE, ePriority ) );
	if( pTexture == NULL && m_MassFileManager.IsValidFile( deviceIDUpper ) == true )
	{
		for( int i = 0; i < (int)m_HighTexName.size(); i++ )
		{
            const wstring& texName = m_HighTexName[i];
			if( texName.size() != 0 && texName.compare( deviceIDUpper ) == 0 )
			{
				bAlwaysHigh = true;
				break;
			}
		}
		pTexture = new CKTDXDeviceTexture( g_pKTDXApp->GetDevice(), deviceIDUpper, m_TexDetailLevel, texFormat, bAlwaysHigh );
        pTexture = static_cast<CKTDXDeviceTexture*> ( CreateAndPendDevice( pTexture, ePriority ) );
	}

	ASSERT( NULL == pTexture || CKTDXDevice::DT_TEXTURE == pTexture->GetDeviceType() );

	return pTexture;
}
CKTDXDeviceXSkinMesh* CKTDXDeviceManager::OpenXSkinMeshInBackground(wstring deviceID, EPriority ePriority, const WCHAR* moveBoneName_ )
{
    if ( deviceID.empty() )
        return NULL;
    if ( false == ( (int) ePriority >= 0 && (int) ePriority < (int) PRIORITY_NUM ) )
        return NULL;
	if ( ePriority == PRIORITY_BLOCKING )
		return	OpenXSkinMesh( deviceID,moveBoneName_ );

    MakeUpperCase( deviceID );

	CKTDXDeviceXSkinMesh* pXSkinMesh = static_cast<CKTDXDeviceXSkinMesh*>( PendDevice( deviceID.c_str(), CKTDXDevice::DT_XSKIN_MESH, ePriority ) );

	if( pXSkinMesh == NULL && m_MassFileManager.IsValidFile( deviceID ) == true )
	{
		pXSkinMesh = new CKTDXDeviceXSkinMesh( g_pKTDXApp->GetDevice(), deviceID, moveBoneName_ );
        ASSERT( pXSkinMesh != NULL );
        pXSkinMesh = static_cast<CKTDXDeviceXSkinMesh*>( CreateAndPendDevice( pXSkinMesh, ePriority ) );
	}

	ASSERT( NULL == pXSkinMesh || CKTDXDevice::DT_XSKIN_MESH == pXSkinMesh->GetDeviceType() );

	return pXSkinMesh;

}


CKTDXDeviceXMesh* CKTDXDeviceManager::OpenXMeshInBackground( wstring deviceID, EPriority ePriority )
{
	KTDXPROFILE();

    if ( deviceID.empty() )
        return NULL;
    if ( false == ( (int) ePriority >= 0 && (int) ePriority < (int) PRIORITY_NUM ) )
        return NULL;
	if ( ePriority == PRIORITY_BLOCKING )
		return	OpenXMesh( deviceID );

	MakeUpperCase( deviceID );

	CKTDXDeviceXMesh* pXMesh = static_cast<CKTDXDeviceXMesh*>( PendDevice( deviceID.c_str(), CKTDXDevice::DT_XMESH, ePriority ) );

    /** m_MassFileManager에서 관리 대상이 되는 valid한 파일인지를 검사한다.
        IsValidFile()내부에서 파일을 모두 읽는데, 파일의 존재 여부만을 check하도록 수정했다.
        - jintaeks on 2008-10-17, 16:35 */
	if( pXMesh == NULL && m_MassFileManager.IsValidFile( deviceID ) == true )
	{
		pXMesh = new CKTDXDeviceXMesh( g_pKTDXApp->GetDevice(), deviceID );
        ASSERT( pXMesh != NULL );
        pXMesh = static_cast<CKTDXDeviceXMesh*>( CreateAndPendDevice( pXMesh, ePriority ) );
	}//if

	ASSERT( NULL == pXMesh || CKTDXDevice::DT_XMESH == pXMesh->GetDeviceType() );

	return pXMesh;

}//CKTDXDeviceManager::OpenXMesh()


CKTDXDeviceSound* CKTDXDeviceManager::OpenSoundInBackground( wstring deviceID,
	EPriority ePriority,
	int bufCount, bool bUse3D )
{
    if ( deviceID.empty() )
        return NULL;
    if ( false == ( (int) ePriority >= 0 && (int) ePriority < (int) PRIORITY_NUM ) )
        return NULL;
	if ( ePriority == PRIORITY_BLOCKING )
		return	OpenSound( deviceID, bufCount, bUse3D );

    MakeUpperCase( deviceID );

	CKTDXDeviceSound* pSound = static_cast<CKTDXDeviceSound*>( PendDevice( deviceID.c_str(), CKTDXDevice::DT_SOUND, ePriority ) );

	if( pSound == NULL && m_MassFileManager.IsValidFile( deviceID ) == true )
	{
		if( g_pKTDXApp->GetDSManager()->GetInit() == false )
		{
			return NULL;
		}
		else
		{
			pSound = new CKTDXDeviceSound( g_pKTDXApp->GetDSManager()->GetSystem(), deviceID, bUse3D );
            ASSERT( pSound != NULL );
            pSound = static_cast<CKTDXDeviceSound*>( CreateAndPendDevice( pSound, ePriority ) );
		}
	}
	return pSound;
}


CKTDXDeviceXET*	CKTDXDeviceManager::OpenXETInBackground( wstring deviceID, EPriority ePriority, bool bWithoutFile /* = false */ )
{
    if ( deviceID.empty() )
        return NULL;
    if ( false == ( (int) ePriority >= 0 && (int) ePriority < (int) PRIORITY_NUM ) )
        return NULL;
	if ( ePriority == PRIORITY_BLOCKING )
		return	OpenXET( deviceID, bWithoutFile );

    MakeUpperCase( deviceID );

	CKTDXDeviceXET* pXET = static_cast<CKTDXDeviceXET*>( PendDevice( deviceID.c_str(), CKTDXDevice::DT_XET, ePriority ) );

	if( pXET == NULL )
	{
		if( bWithoutFile == true || m_MassFileManager.IsValidFile( deviceID ) == true )
		{
			pXET = new CKTDXDeviceXET( deviceID.c_str() );
            ASSERT( pXET != NULL );
			pXET->SetWithoutFile( bWithoutFile );
            pXET = static_cast<CKTDXDeviceXET*>( CreateAndPendDevice( pXET, ePriority ) );
		}
	}

	return pXET;



}

bool CKTDXDeviceManager::IsPlaying( const WCHAR* pFileName )
{
	if ( pFileName == NULL || pFileName[0] == NULL )
		return false;

	WCHAR    deviceIDUpper[MAX_PATH];
	MakeUpperCase( deviceIDUpper, ARRAY_SIZE( deviceIDUpper ), pFileName );


	CKTDXDeviceSound* pSound = NULL;
	{
		CSLock locker( m_DeviceLock );
		DeviceMap::iterator device;
		device = m_DeviceMap.find( deviceIDUpper );
		if( device == m_DeviceMap.end() )
			return false;

		CKTDXDevice* pDevice = device->second;
		ASSERT( pDevice != NULL );
		if ( pDevice == NULL || pDevice->GetDeviceType() != CKTDXDevice::DT_SOUND )
			return false;

		pSound = static_cast<CKTDXDeviceSound*>( pDevice );
	}

	if ( pSound == NULL )
		return false;

	return pSound->IsPlaying();
}

CKTDXDeviceSound* CKTDXDeviceManager::PlaySound( const WCHAR* pFileName, bool bLoop /* = false */, bool b3DSound /*= true*/  )
{
    if ( pFileName == NULL || pFileName[0] == NULL )
        return NULL;

    WCHAR    deviceIDUpper[MAX_PATH];
    MakeUpperCase( deviceIDUpper, ARRAY_SIZE( deviceIDUpper ), pFileName );


    CKTDXDeviceSound* pSound = NULL;
    {
	    CSLock locker( m_DeviceLock );
	    DeviceMap::iterator device;
        device = m_DeviceMap.find( deviceIDUpper );
	    if( device == m_DeviceMap.end() )
            goto out;
        CKTDXDevice* pDevice = device->second;
        ASSERT( pDevice != NULL );
        if ( pDevice == NULL || pDevice->GetDeviceType() != CKTDXDevice::DT_SOUND )
            return NULL;
        pSound = static_cast<CKTDXDeviceSound*>( pDevice );
    }

    if ( pSound != NULL )
    {
        pSound->LoadDevice();
        pSound->Play( bLoop, b3DSound );
        return pSound;
    }//if

out:
    {
		pSound = OpenSound( deviceIDUpper, 10, b3DSound );
		if( pSound != NULL )
		{
			pSound->Play( bLoop, b3DSound );
			return pSound;
			//MessageBox(g_pKTDXApp->GetHWND(), pFileName, L"미리 로드 안된 사운드", MB_OK );
			//CloseDevice( pFileName );
		}
		else
		{
			ErrorLogMsg( KEM_ERROR351, pFileName );
		}
	}

	return NULL;
}

void CKTDXDeviceManager::StopSound( const WCHAR* pFileName )
{
    if ( pFileName == NULL || pFileName[0] == NULL )
        return;

    WCHAR    deviceID[MAX_PATH];
    MakeUpperCase( deviceID, ARRAY_SIZE( deviceID ), pFileName );


	CSLock locker( m_DeviceLock );


	DeviceMap::iterator pDevice;
	pDevice = m_DeviceMap.find( deviceID );

	if( pDevice != m_DeviceMap.end() )
	{
		if( pDevice->second->GetDeviceType() == CKTDXDevice::DT_SOUND )
		{
			CKTDXDeviceSound* pSound = (CKTDXDeviceSound*)pDevice->second;
			pSound->Stop();
		}
	}


}



void CKTDXDeviceManager::CloseDeviceDirectory( wstring wstrPath )
{
    if ( wstrPath.empty() )
        return;

	CSLock locker( m_DeviceLock );

	// file path와 name을 분리
	WCHAR drive[_MAX_DRIVE];
	WCHAR dir[_MAX_DIR];
	WCHAR fname[_MAX_FNAME];
	WCHAR ext[_MAX_EXT];

#ifdef	CONVERSION_VS
	_wsplitpath_s( wstrPath.c_str(), drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT );
#else
	_wsplitpath( wstrPath.c_str(), drive, dir, fname, ext );
#endif // CONVERSION_VS

	WCHAR wszCurrDir[256] = L"";
	string strCurrDir;

#ifdef	CONVERSION_VS
	wcscat_s( wszCurrDir, _countof(wszCurrDir), drive );
	wcscat_s( wszCurrDir, _countof(wszCurrDir), dir );
#else
	wcscat( wszCurrDir, drive );
	wcscat( wszCurrDir, dir );
#endif // CONVERSION_VS

	ConvertWCHARToChar( strCurrDir, wszCurrDir );
	//g_pKTDXApp->GetDeviceManager()->GetMassFileManager()->AddDataDirectory( strCurrDir );

#ifdef	CONVERSION_VS
	wcscat_s( wszCurrDir, _countof(wszCurrDir), L"*.*" );
#else
	wcscat( wszCurrDir, L"*.*" );
#endif // CONVERSION_VS

	ConvertWCHARToChar( strCurrDir, wszCurrDir );

	HANDLE				hSearch;
	WIN32_FIND_DATAA	fd;

	hSearch = FindFirstFileA( strCurrDir.c_str(), &fd);

	if(hSearch == INVALID_HANDLE_VALUE)
		return;

	wstring deviceID;
	do
	{
		if( 0 != strcmp(fd.cFileName, ".") && 0 != strcmp(fd.cFileName, "..") )
		{
			if( fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
			{
				;
			}
			else
			{
				ConvertCharToWCHAR( deviceID, fd.cFileName );
				MakeUpperCase( deviceID );

				int iResult;
				if( (iResult = deviceID.find( L".X" )) > 0 )
				{
					CloseDevice(deviceID);
					//m_OpenedDeviceMap.erase(deviceID);
				}
				else if( (iResult = deviceID.find( L".TGA" )) > 0 || (iResult = deviceID.find( L".DDS" )) > 0 )
				{
					CloseDevice(deviceID);
					//m_OpenedDeviceMap.erase(deviceID);
				}
				else if( (iResult = deviceID.find( L".WAV" )) > 0 )
				{
					CloseDevice(deviceID);
					//m_OpenedDeviceMap.erase(deviceID);
				}
			}
		}
	} while( FindNextFileA(hSearch, &fd) );

	FindClose(hSearch);


}


void CKTDXDeviceManager::OpenDeviceDirectory( wstring wstrPath )
{
    if ( wstrPath.empty() )
        return;

	CSLock locker( m_DeviceLock );

	// file path와 name을 분리
	WCHAR drive[_MAX_DRIVE];
	WCHAR dir[_MAX_DIR];
	WCHAR fname[_MAX_FNAME];
	WCHAR ext[_MAX_EXT];

#ifdef	CONVERSION_VS
	_wsplitpath_s( wstrPath.c_str(), drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT );
#else
	_wsplitpath( wstrPath.c_str(), drive, dir, fname, ext );
#endif // CONVERSION_VS

	WCHAR wszCurrDir[256] = L"";
	string strCurrDir;
#ifdef	CONVERSION_VS
	wcscat_s( wszCurrDir, _countof(wszCurrDir), drive );
	wcscat_s( wszCurrDir, _countof(wszCurrDir), dir );
#else
	wcscat( wszCurrDir, drive );
	wcscat( wszCurrDir, dir );
#endif // CONVERSION_VS

	ConvertWCHARToChar( strCurrDir, wszCurrDir );
	//g_pKTDXApp->GetDeviceManager()->GetMassFileManager()->AddDataDirectory( strCurrDir );
#ifdef	CONVERSION_VS
	wcscat_s( wszCurrDir, _countof(wszCurrDir), L"*.*" );
#else
	wcscat( wszCurrDir, L"*.*" );
#endif // CONVERSION_VS

	ConvertWCHARToChar( strCurrDir, wszCurrDir );

	HANDLE				hSearch;
	WIN32_FIND_DATAA	fd;

	hSearch = FindFirstFileA( strCurrDir.c_str(), &fd);

	if(hSearch == INVALID_HANDLE_VALUE)
		return;

	wstring deviceID;
	do
	{
		if( 0 != strcmp(fd.cFileName, ".") && 0 != strcmp(fd.cFileName, "..") )
		{
			if( fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
			{
				;
			}
			else
			{
				ConvertCharToWCHAR( deviceID, fd.cFileName );
				MakeUpperCase( deviceID );

				int iResult;
				if( (iResult = deviceID.find( L".X" )) > 0 )
				{
                    // qfff - jintaeks on 2008-10-08, 14:28
					CKTDXDeviceXSkinMesh* pXSkinMesh = OpenXSkinMesh( deviceID );
					if( true == pXSkinMesh->GetIsOnlyModelData() )
					{
						SAFE_CLOSE( pXSkinMesh );
						CKTDXDeviceXMesh* pXMesh = OpenXMesh( deviceID );
						//m_OpenedDeviceMap.insert( std::make_pair(deviceID, pXMesh) );
					}
					else
					{
						//m_OpenedDeviceMap.insert( std::make_pair(deviceID, pXSkinMesh) );
					}
				}
				else if( (iResult = deviceID.find(L".TGA")) > 0 || (iResult = deviceID.find(L".DDS")) > 0 )
				{
					CKTDXDeviceTexture* pTexture = OpenTexture( deviceID );
					//m_OpenedDeviceMap.insert( std::make_pair(deviceID, pTexture) );
				}
				else if( (iResult = deviceID.find( L".WAV" )) > 0 )
				{
					CKTDXDeviceSound* pSound = OpenSound( deviceID );
					//m_OpenedDeviceMap.insert( std::make_pair(deviceID, pSound) );
				}
			}
		}
	} while( FindNextFileA(hSearch, &fd) );

	FindClose(hSearch);


}




//{{ 2007. 11. 12  최육사  device name 출력
void CKTDXDeviceManager::GetDeviceNameAndRefCount( std::vector< std::pair< wstring, std::pair< int, int > > >& vecLoadedDeviceInfo )
{

	CSLock locker( m_DeviceLock );

	DeviceMap::iterator mit = m_DeviceMap.begin();
	for( ; mit != m_DeviceMap.end(); mit++ )
	{
		CKTDXDevice* pKTDXDevice = mit->second;
		if( pKTDXDevice == NULL )
			continue;

		vecLoadedDeviceInfo.push_back( std::pair< wstring, std::pair< int, int > >( pKTDXDevice->GetDeviceID()
			, std::pair< int, int >( pKTDXDevice->GetRefCount(), pKTDXDevice->GetSize() ) ) );
	}


}
//}}

void        CKTDXDeviceManager::_CheckFullSyncInDeviceThread()
{
	LONG lFullSync = ::InterlockedCompareExchange( &m_lFullSyncStatus, 2L, 1L );
	if ( lFullSync == 1L )
	{
		if ( m_hFullSyncEventForMainThread != NULL )
		{
			::SetEvent( m_hFullSyncEventForMainThread );
		}//if
		if ( m_hFullSyncEventForDeviceThread != NULL )
		{
			::WaitForSingleObject( m_hFullSyncEventForDeviceThread, INFINITE );
		}//if
	}//if
}//CKTDXDeviceManager::_CheckFullSyncInDeviceThread()

///-----------------------------------------------------------------------------
//{{ resource loading thread관련 함수의 시작 - jintaeks on 2008-10-14, 16:13
//
/*virtual*/ DWORD CKTDXDeviceManager::RunThread()
{
    g_CKTDXLog.RegisterCurrentThread();

    std::list<CKTDXDevice*> aListTemp[PRIORITY_NUM];


	DWORD   dwCurrentTimestamp = KSystemTimer::GetSystemTimeInMilisecond();
	DWORD   dwNextTimestamp = dwCurrentTimestamp + KTDXDEVICEMANAGER_GC_INTERVAL_IN_MILISECS;
	LONG    lDiff = 0;

    while ( true )
    {
//#ifdef NEW_VILLAGE_UNIT_BACKGROUND_LOADING_TEST
//		//const DWORD MAGIC_SLEEP_TIME = 4;
//		//Sleep( MAGIC_SLEEP_TIME );
//#endif NEW_VILLAGE_UNIT_BACKGROUND_LOADING_TEST

        _CheckFullSyncInDeviceThread();


        DWORD   dwRet = ::WaitForMultipleObjects( TET_MAX, m_ahThreadEvents, FALSE, KTDXDEVICEMANAGER_GC_INTERVAL_IN_MILISECS );

        switch( dwRet )
        {
        case WAIT_FAILED:
        case WAIT_IO_COMPLETION:
            break;
        case WAIT_TIMEOUT:
            break;
        case WAIT_OBJECT_0: // TET_PEND_DEVICE
            {
				for( int iPriority = PRIORITY_HIGH; iPriority != PRIORITY_NUM; iPriority++ )
				{
					aListTemp[ iPriority ].clear();
					PendQueue& pendQueue = m_aPendQueue[ iPriority ];
					CSLock	lock( pendQueue.m_csPendingDevice );
					aListTemp[ iPriority ].swap( pendQueue.m_listPendingDevice );
				}//for

                for( int iPriority = PRIORITY_HIGH; iPriority != PRIORITY_NUM; iPriority++ )
                {
					std::list<CKTDXDevice*>& listTemp = aListTemp[ iPriority ];
                    if ( listTemp.empty() == false )
                    {
                        while( listTemp.empty() == false )
                        {
                            CKTDXDevice* pDevice = listTemp.front();
                            listTemp.pop_front();
                            if ( pDevice == NULL )
                                continue;
							LoadAndCloseDevice( pDevice
								, true
								);
                        }//while
                    }//if
                }//for
            }
            break;
        case WAIT_OBJECT_0 + 1: // TET_KILL_THREAD
            goto out;
        }//switch

    }//while
out:

    DXUTTRACE( L"end of thread" );


    return 0L;
}//CKTDXDeviceManager::RunThread()


void CKTDXDeviceManager::_InitializeThreadRelatedObject()
{
    m_ahThreadEvents[ TET_PEND_DEVICE ] = ::CreateEvent( NULL, FALSE, FALSE, NULL );
    m_ahThreadEvents[ TET_KILL_THREAD ] = ::CreateEvent( NULL, FALSE, FALSE, NULL );
    //m_bThreadSuspended = true;

    m_lFullSyncStatus = 0;
    m_hFullSyncEventForMainThread = ::CreateEvent( NULL, FALSE, FALSE, NULL );
    m_hFullSyncEventForDeviceThread = ::CreateEvent( NULL, FALSE, FALSE, NULL );

    m_iForegroundLoadCounter = 0;

}//CKTDXDeviceManager::_InitializeThreadRelatedObject()


void CKTDXDeviceManager::_FinalizeThreadRelatedObject()
{
    for( int i = 0; i < TET_MAX; i++ )
    {
        if ( m_ahThreadEvents[ i ] != NULL )
        {
            ::CloseHandle( m_ahThreadEvents[ i ] );
            m_ahThreadEvents[ i ] = NULL;
        }//if
    }//for

    if ( m_hFullSyncEventForMainThread != NULL )
    {
        ::CloseHandle( m_hFullSyncEventForMainThread );
    }//if
    if ( m_hFullSyncEventForDeviceThread != NULL )
    {
        ::CloseHandle( m_hFullSyncEventForDeviceThread );
    }//if

}//CKTDXDeviceManager::_FinalizeThreadRelatedObject()


void CKTDXDeviceManager::ResumeThread()
{
    if ( __super::GetThreadHandle() )
    {
        //::SetEvent( m_hThreadEvent );
        //m_bThreadSuspended = false;
        ::ResumeThread( __super::GetThreadHandle() );
    }//if
}//CKTDXDeviceManager::ResumeThread()





//{{ seojt // 2008-10-24, 14:43
void CKTDXDeviceManager::Debug_RenderHelp(int ix_, int iy_)
{
    int     ix = ix_ + 16;
    int     iy = iy_ + 16;
    WCHAR   wbuffer[1024];
    UINT    uiNumPendingResources = 0;
    UINT    uiNumPendingLuaData = 0;

    if ( g_pKTDXApp == NULL )
        return;

    CKTDGFontManager::CUKFont*  pFontForDebug = g_pKTDXApp->GetDGManager()->GetDialogManager()->GetUKFont( 0 );

    if ( pFontForDebug == NULL )
        return;

    StringCchPrintfW( wbuffer, ARRAY_SIZE(wbuffer), L"%S", __FUNCTION__ );
	pFontForDebug->OutTextMultiline( ix, iy, wbuffer, D3DXCOLOR( 1, 1, 0, 1),
		CKTDGFontManager::FS_SHELL, D3DXCOLOR( 0, 0, 0, 1 ) );


//#ifdef NEW_VILLAGE_UNIT_BACKGROUND_LOADING_TEST
//	string pendingResource = "";
//#endif NEW_VILLAGE_UNIT_BACKGROUND_LOADING_TEST
//
//
//    synchronized( m_csQueueResPending )
//    {
//        uiNumPendingResources = m_queueResPending.size();
//
//#ifdef NEW_VILLAGE_UNIT_BACKGROUND_LOADING_TEST
//		if( false == m_queueResPending.empty() )
//		{
//			const RESOURCE_REQUEST_NODE& node = m_queueResPending.front();
//			pendingResource = node.m_strDeviceID;
//		}
//#endif NEW_VILLAGE_UNIT_BACKGROUND_LOADING_TEST
//
//    }//block



//#ifdef NEW_VILLAGE_UNIT_BACKGROUND_LOADING_TEST
//	CKTDXDevice* pDevice_loadingLua = NULL;
//#endif NEW_VILLAGE_UNIT_BACKGROUND_LOADING_TEST

//    synchronized( m_csQueueLuaData )
//    {
//        uiNumPendingLuaData = m_queueLuaData.size();
//
//#ifdef NEW_VILLAGE_UNIT_BACKGROUND_LOADING_TEST
//		if( false == m_queueLuaData.empty() )
//		{
//			pDevice_loadingLua = m_queueLuaData.front().m_pDeviceRef;
//		}
//#endif NEW_VILLAGE_UNIT_BACKGROUND_LOADING_TEST
//    }//block


	//StringCchPrintfW( wbuffer, ARRAY_SIZE(wbuffer),
	//	L"uiNumPendingResources = %i", uiNumPendingResources );
	//pFontForDebug->OutTextXY( ix, iy+=16, wbuffer, D3DXCOLOR( 1, 1, 0, 1),
	//	CKTDGFontManager::FS_SHELL, D3DXCOLOR( 0, 0, 0, 1 ) );



//#ifdef NEW_VILLAGE_UNIT_BACKGROUND_LOADING_TEST
//	if( false == pendingResource.empty() )
//	{
//        std::wstring    wstrTemp;
//        ConvertCharToWCHAR( wstrTemp, pendingResource );
//		pFontForDebug->OutTextXY( ix, iy+=16, wstrTemp.c_str(), D3DXCOLOR( 1, 1, 0, 1),
//			CKTDGFontManager::FS_SHELL, D3DXCOLOR( 0, 0, 0, 1 ) );
//	}
//#endif NEW_VILLAGE_UNIT_BACKGROUND_LOADING_TEST



	//StringCchPrintfW( wbuffer, ARRAY_SIZE(wbuffer),
	//	L"uiNumPendingLuaData = %i", uiNumPendingLuaData );
	//pFontForDebug->OutTextXY( ix, iy+=16, wbuffer, D3DXCOLOR( 1, 1, 0, 1),
	//	CKTDGFontManager::FS_SHELL, D3DXCOLOR( 0, 0, 0, 1 ) );

//#ifdef NEW_VILLAGE_UNIT_BACKGROUND_LOADING_TEST
//	if( NULL != pDevice_loadingLua &&
//		g_pKTDXApp->GetDeviceManager()->IsValidDevice( pDevice_loadingLua ) )
//	{
//        std::wstring    wstrTemp;
//        ConvertCharToWCHAR( wstrTemp, pDevice_loadingLua->GetDeviceID() );
//		pFontForDebug->OutTextXY( ix, iy+=16, wstrTemp.c_str(), D3DXCOLOR( 1, 1, 0, 1),
//			CKTDGFontManager::FS_SHELL, D3DXCOLOR( 0, 0, 0, 1 ) );
//	}
//#endif NEW_VILLAGE_UNIT_BACKGROUND_LOADING_TEST


}//CKTDXDeviceManager::Debug_RenderHelp()
//}} seojt // 2008-10-24, 14:43


//UINT  CKTDXDeviceManager::GetPendingResourceCount()
//{
//	UINT uiCount = 0;
//
//	synchronized( m_csQueueResPending )
//	{
//		uiCount = m_queueResPending.size();
//	}
//
//	return uiCount;
//}
//

//{{ robobeg : 2012-11-15
		//CX2UnitLoader 에서 옮겨옴

void	CKTDXDeviceManager::AppendXSkinMesh( CKTDXDeviceDataList& listOut_, const std::wstring& wstrName_, CKTDXDeviceManager::EPriority ePriority )
{
    if ( wstrName_.empty() == true )
        return;

	CKTDXDevice* pDevice = ( ePriority == PRIORITY_BLOCKING )
		? OpenXSkinMesh( wstrName_ ) : OpenXSkinMeshInBackground( wstrName_, ePriority );
    if ( pDevice != NULL )
    {
		listOut_.resize( listOut_.size() + 1 );
		listOut_.back().m_pDevice = pDevice;
    }//if
}

void	CKTDXDeviceManager::AppendXMesh( CKTDXDeviceDataList& listOut_, const std::wstring& wstrName_, CKTDXDeviceManager::EPriority ePriority )
{
    if ( wstrName_.empty() == true )
        return;

	CKTDXDevice* pDevice = ( ePriority == PRIORITY_BLOCKING )
		? OpenXMesh( wstrName_ ) : OpenXMeshInBackground( wstrName_, ePriority );
    if ( pDevice != NULL )
    {
		listOut_.resize( listOut_.size() + 1 );
		listOut_.back().m_pDevice = pDevice;
    }//if
}

void	CKTDXDeviceManager::AppendTexture( CKTDXDeviceDataList& listOut_, const std::wstring& wstrName_, CKTDXDeviceManager::EPriority ePriority )
{
    if ( wstrName_.empty() == true )
        return;
	CKTDXDevice* pDevice = ( ePriority == PRIORITY_BLOCKING )
		? OpenTexture( wstrName_ ) : OpenTextureInBackground( wstrName_, ePriority );
    if ( pDevice != NULL )
    {
		listOut_.resize( listOut_.size() + 1 );
		listOut_.back().m_pDevice = pDevice;
    }//if
}
void	CKTDXDeviceManager::AppendXET( CKTDXDeviceDataList& listOut_, const std::wstring& wstrName_, CKTDXDeviceManager::EPriority ePriority )
{
    if ( wstrName_.empty() == true )
        return;
	CKTDXDevice* pDevice = ( ePriority == PRIORITY_BLOCKING )
		? OpenXET( wstrName_ ) : OpenXETInBackground( wstrName_, ePriority );
    if ( pDevice != NULL )
    {
		listOut_.resize( listOut_.size() + 1 );
		listOut_.back().m_pDevice = pDevice;
    }//if
}
void	CKTDXDeviceManager::AppendSound( CKTDXDeviceDataList& listOut_, const std::wstring& wstrName_ )
{
    if ( wstrName_.empty() == true )
        return;
	CKTDXDevice* pDevice = OpenSoundInBackground( wstrName_ );
    if ( pDevice != NULL )
    {
		listOut_.resize( listOut_.size() + 1 );
		listOut_.back().m_pDevice = pDevice;
    }//if
}

void	CKTDXDeviceManager::AppendXSkinMesh( CKTDXDeviceDataList& listOut_, const WCHAR* pName_, CKTDXDeviceManager::EPriority ePriority )
{
    if ( pName_ == NULL || pName_[0] == NULL )
        return;

	CKTDXDevice* pDevice = ( ePriority == PRIORITY_BLOCKING )
		? OpenXSkinMesh( pName_ ) : OpenXSkinMeshInBackground( pName_, ePriority );
    if ( pDevice != NULL )
    {
		listOut_.resize( listOut_.size() + 1 );
		listOut_.back().m_pDevice = pDevice;
    }//if
}

void	CKTDXDeviceManager::AppendXMesh( CKTDXDeviceDataList& listOut_, const WCHAR* pName_, CKTDXDeviceManager::EPriority ePriority )
{
    if ( pName_ == NULL || pName_[0] == NULL )
        return;

	CKTDXDevice* pDevice = ( ePriority == PRIORITY_BLOCKING )
		? OpenXMesh( pName_ ) : OpenXMeshInBackground( pName_, ePriority );
    if ( pDevice != NULL )
    {
		listOut_.resize( listOut_.size() + 1 );
		listOut_.back().m_pDevice = pDevice;
    }//if
}

void	CKTDXDeviceManager::AppendTexture( CKTDXDeviceDataList& listOut_, const WCHAR* pName_, CKTDXDeviceManager::EPriority ePriority )
{
    if ( pName_ == NULL || pName_[0] == NULL )
        return;
	CKTDXDevice* pDevice = ( ePriority == PRIORITY_BLOCKING )
		? OpenTexture( pName_ ) : OpenTextureInBackground( pName_, ePriority );
    if ( pDevice != NULL )
    {
		listOut_.resize( listOut_.size() + 1 );
		listOut_.back().m_pDevice = pDevice;
    }//if
}

void	CKTDXDeviceManager::AppendXET( CKTDXDeviceDataList& listOut_, const WCHAR* pName_, CKTDXDeviceManager::EPriority ePriority )
{
    if ( pName_ == NULL || pName_[0] == NULL )
        return;
	CKTDXDevice* pDevice = ( ePriority == PRIORITY_BLOCKING )
		? OpenXET( pName_ ) : OpenXETInBackground( pName_, ePriority );
    if ( pDevice != NULL )
    {
		listOut_.resize( listOut_.size() + 1 );
		listOut_.back().m_pDevice = pDevice;
    }//if
}

void	CKTDXDeviceManager::AppendSound( CKTDXDeviceDataList& listOut_, const WCHAR* pName_ )
{
    if ( pName_ == NULL || pName_[0] == NULL )
        return;
	CKTDXDevice* pDevice = OpenSoundInBackground( pName_ );
    if ( pDevice != NULL )
    {
		listOut_.resize( listOut_.size() + 1 );
		listOut_.back().m_pDevice = pDevice;
    }//if
}
//}} robobeg : 2012-11-15

//#ifdef	X2OPTIMIZE_MASS_FILE_BUFFER_MANAGER
void	CKTDXDeviceManager::ReleaseAllMemoryBuffers()
{
	m_MassFileManager.ReleaseAllMemoryBuffers();
}
//#endif	X2OPTIMIZE_MASS_FILE_BUFFER_MANAGER
