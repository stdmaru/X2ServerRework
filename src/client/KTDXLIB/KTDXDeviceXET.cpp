#include "stdafx.h"
#include "./KTDXDeviceXET.h"

CKTDXDeviceXET::CKTDXDeviceXET( const WCHAR* pFileName )//, CKTDXDevice* pMesh )
				: CKTDXDevice( pFileName, DT_XET )
{
	//m_pMesh				= pMesh;
}

CKTDXDeviceXET::~CKTDXDeviceXET(void)
{
	_UnLoad();
}




void CKTDXDeviceXET::SetMultiTexStage1( const char* pOrgTexName, const char* pBlendTexName, int colorOP )
{
	if ( pBlendTexName == NULL || pBlendTexName[0] == NULL )
		return;

	wstring wBlendTexName = L"";
	ConvertUtf8ToWCHAR( wBlendTexName, pBlendTexName );
	MakeUpperCase( wBlendTexName );

	m_MultiTexStage1.Release();

//#ifndef X2OPTIMIZE_TET_XET_PREPROCESSING
//	ConvertUtf8ToWCHAR( m_MultiTexStage1.orgTexName, pOrgTexName );
//	MakeUpperCase( m_MultiTexStage1.orgTexName );
//#endif  X2OPTIMIZE_TET_XET_PREPROCESSING
//
	m_MultiTexStage1.pTexture = g_pKTDXApp->GetDeviceManager()->OpenTexture( wBlendTexName );
	m_MultiTexStage1.colorOP = (D3DTEXTUREOP)colorOP;
}

void CKTDXDeviceXET::SetMultiTexStage2( const char* pOrgTexName, const char* pBlendTexName, int colorOP )
{
	if ( pBlendTexName == NULL || pBlendTexName[0] == NULL )
		return;

	wstring wBlendTexName = L"";
	ConvertUtf8ToWCHAR( wBlendTexName, pBlendTexName );
	MakeUpperCase( wBlendTexName );

	m_MultiTexStage2.Release();
//#ifndef X2OPTIMIZE_TET_XET_PREPROCESSING
//	ConvertUtf8ToWCHAR( m_MultiTexStage2.orgTexName, pOrgTexName );
//	MakeUpperCase( m_MultiTexStage2.orgTexName );
//#endif  X2OPTIMIZE_TET_XET_PREPROCESSING

	m_MultiTexStage2.pTexture = g_pKTDXApp->GetDeviceManager()->OpenTexture( wBlendTexName );
	m_MultiTexStage2.colorOP = (D3DTEXTUREOP)colorOP;
}


void    CKTDXDeviceXET::MultiTexureData::Release()
{
	if( pTexture != NULL && g_pKTDXApp != NULL && g_pKTDXApp->GetAppDestroy() != true )
	{
		SAFE_CLOSE( pTexture );
	}
    pTexture = NULL;
    //orgTexName.resize( 0 );
    colorOP		= D3DTOP_MODULATE;
}//CKTDXDeviceXET::MultiTexureData::ReleaseTexture()



const CKTDXDeviceXET::MultiTexureData* CKTDXDeviceXET::GetMultiTex( int stage ) const
{
	if( stage == 1 )
	{
		return &m_MultiTexStage1;
	}
	else if( stage == 2 )
	{
		return &m_MultiTexStage2;
	}
	return NULL;
}




HRESULT CKTDXDeviceXET::_Load( bool bSkipStateCheck
				, bool
	)
{
	////CKTDXThread::CLocker locker( g_pKTDXApp->GetDeviceManager()->GetDeviceLock() );

    int iSize = 0;
    CKTDXDeviceXET_Preprocessing    preprocessing;
    if ( preprocessing.OpenFile( m_DeviceID.c_str(), iSize ) == false )
    {
		ErrorLogMsg( KEM_ERROR96, m_DeviceID.c_str());

		return E_FAIL;
    }


    std::vector<CKTDXDeviceTexture*>    vecTexture;
    std::vector<AniData>                vecAniData;
    MultiTexureData multiTexStage1;
    MultiTexureData multiTexStage2;

    {
        const KXETFormatHeader& kHeader = preprocessing.GetHeader();
        vecTexture.resize( kHeader.m_dwNumChangeTexture + kHeader.m_dwNumAniEventTexChange );
        const BYTE* pData = (const BYTE*)( &kHeader );
        DWORD   dwOffset = kHeader.GetChangeTextureOffset();
        for( DWORD dwIndex = 0; dwIndex != kHeader.m_dwNumChangeTexture; ++dwIndex, dwOffset += sizeof(KXETFormatChangeTexture) )
        {
            const KXETFormatChangeTexture& kChangeTexture = *( const KXETFormatChangeTexture* )
                ( pData + dwOffset );
            const wchar_t* pwszString = preprocessing.GetString( kChangeTexture.m_dwOffset_ChangeTexName );
            if ( pwszString != NULL && pwszString[0] != NULL )
                vecTexture[ dwIndex ] = g_pKTDXApp->GetDeviceManager()->OpenTexture( pwszString );
            else
                vecTexture[ dwIndex ] = NULL;
        }
        dwOffset = kHeader.GetAniEventTexChangeOffset();
        for( DWORD dwIndex = 0; dwIndex != kHeader.m_dwNumAniEventTexChange; ++dwIndex, dwOffset += sizeof(KXETFormatAniTexChangeEvent) )
        {
            const KXETFormatAniTexChangeEvent& kEvent = *( const KXETFormatAniTexChangeEvent* ) ( pData + dwOffset );
            const wchar_t* pwszString = preprocessing.GetString( kEvent.m_dwOffset_ChangeTexName );
            if ( pwszString != NULL && pwszString[0] != NULL )
                vecTexture[ dwIndex + kHeader.m_dwNumChangeTexture ] = g_pKTDXApp->GetDeviceManager()->OpenTexture( pwszString );
            else
                vecTexture[ dwIndex + kHeader.m_dwNumChangeTexture ] = NULL;
        }
        if ( kHeader.m_kMultiStage1.m_dwOffset_BlendTexName != 0 )
        {
            const wchar_t* pwszString = preprocessing.GetString( kHeader.m_kMultiStage1.m_dwOffset_BlendTexName );
            if ( pwszString != NULL && pwszString[0] != NULL )
                multiTexStage1.pTexture = g_pKTDXApp->GetDeviceManager()->OpenTexture( pwszString );
            multiTexStage1.colorOP = D3DTEXTUREOP( kHeader.m_kMultiStage1.m_dwColorOP );

        }
        if ( kHeader.m_kMultiStage2.m_dwOffset_BlendTexName != 0 )
        {
            const wchar_t* pwszString = preprocessing.GetString( kHeader.m_kMultiStage2.m_dwOffset_BlendTexName );
            if ( pwszString != NULL && pwszString[0] != NULL )
                multiTexStage2.pTexture = g_pKTDXApp->GetDeviceManager()->OpenTexture( pwszString );
            multiTexStage2.colorOP = D3DTEXTUREOP( kHeader.m_kMultiStage2.m_dwColorOP );
        }
        if ( kHeader.m_dwNumAniData != 0 )
        {
            vecAniData.resize( kHeader.m_dwNumAniData );
            dwOffset = kHeader.GetAniDataOffset();
            for( DWORD dwIndex = 0; dwIndex != kHeader.m_dwNumAniData; ++dwIndex, dwOffset += sizeof(KXETFormatAniData) )
            {
                const KXETFormatAniData& kFormatAniData = *( const KXETFormatAniData* ) ( pData + dwOffset );
                AniData& aniData = vecAniData[ dwIndex ];
                aniData.m_pDeviceXET = this;
                aniData.m_pAniData = &kFormatAniData;
                aniData.fTransitionTime = kFormatAniData.m_fTransitionTime;
                aniData.fAniMaxTime = kFormatAniData.m_fAniMaxTime;
            }
        }
    }

	{
        CSLock  locker( m_csDeviceState );

        if ( bSkipStateCheck == false )
	    {
		    switch( m_eDeviceState )
		    {
		    case DEVICE_STATE_INIT:
		    //case DEVICE_STATE_PENDING:
		    default:
			    ASSERT( 0 );
		    case DEVICE_STATE_LOADING:
		    case DEVICE_STATE_FAILED:
                m_Size = iSize;
                m_preprocessed.Swap( preprocessing );
                m_vecTexture.swap( vecTexture );
                m_vecAniData.swap( vecAniData );
                m_MultiTexStage1.Swap( multiTexStage1 );
                m_MultiTexStage2.Swap( multiTexStage2 );
		        m_eDeviceState = DEVICE_STATE_LOADED;
			    break;
		    case DEVICE_STATE_LOADED:
			    break;
		    }//switch
	    }
        else
        {
            m_Size = iSize;
            m_preprocessed.Swap( preprocessing );
            m_vecTexture.swap( vecTexture );
            m_vecAniData.swap( vecAniData );
            m_MultiTexStage1.Swap( multiTexStage1 );
            m_MultiTexStage2.Swap( multiTexStage2 );
        }//if.. else..
    }

    if ( vecTexture.empty() == false )
    {
        for( CKTDXDeviceTexture* pTexture : vecTexture )
        {
            SAFE_CLOSE( pTexture )
        }
    }

	return S_OK;
}

HRESULT CKTDXDeviceXET::_UnLoad()
{
    m_preprocessed.Release();
    if ( g_pKTDXApp != NULL && g_pKTDXApp->GetAppDestroy() == false )
    {
        for( CKTDXDeviceTexture* pTexture : m_vecTexture )
        {
            SAFE_CLOSE( pTexture )
        }
        m_vecTexture.resize( 0 );
    }
	m_MultiTexStage1.Release();
	m_MultiTexStage2.Release();
    m_vecAniData.resize( 0 );

	return S_OK;
}

CKTDXDeviceTexture*     CKTDXDeviceXET::GetChangeTexture( const wchar_t* pTexName )
{
    int iIndex = m_preprocessed.FindChangeTextureIndex( pTexName );
    if ( iIndex < 0 || iIndex >= (int) m_vecTexture.size() )
        return NULL;

    return m_vecTexture[ iIndex ];
}

const CKTDXDeviceXET::AniData* CKTDXDeviceXET::GetAniData( const wchar_t* pAniName )
{
    int iIndex = m_preprocessed.FindAniDataIndex( pAniName );
    if ( iIndex < 0 || iIndex >= (int) m_vecAniData.size() )
        return NULL;
    return  &m_vecAniData[ iIndex ];
}

CKTDXDeviceTexture* CKTDXDeviceXET::AniData::GetAniTexture( float fAniTime, const wchar_t* pTexName ) const
{
    if ( m_pDeviceXET == NULL || m_pAniData == NULL )
        return NULL;
    return  m_pDeviceXET->GetAniTexture( *m_pAniData, fAniTime, pTexName );
}

CKTDXDeviceTexture* CKTDXDeviceXET::GetAniTexture( const KXETFormatAniData& kAniData, float fAniTime, const wchar_t* pwszTexName ) const
{
    int iIndex = m_preprocessed.FindAniTexChangeEventIndex( kAniData, fAniTime, pwszTexName );
    if ( iIndex < 0 )
        return NULL;
    const KXETFormatHeader& kHeader = m_preprocessed.GetHeader();
    if ( iIndex >= (int) kHeader.m_dwNumAniEventTexChange )
        return NULL;
    iIndex += (int) kHeader.m_dwNumChangeTexture;
    if ( iIndex >= (int) m_vecTexture.size() )
        return NULL;
    return  m_vecTexture[ iIndex ];
}
