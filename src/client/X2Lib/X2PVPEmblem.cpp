#include "stdafx.h"
#include "./X2PVPEmblem.h"

CX2PVPEmblem::CX2PVPEmblem(void)
{
	OpenScript( L"PVPEmblem_Season2.lua" );
}

CX2PVPEmblem::~CX2PVPEmblem(void)
{
	map< PVP_RANK, PVPEmblemData* >::iterator i;

	for ( i = m_mapEmblemData.begin(); i != m_mapEmblemData.end(); i++ )
	{
		CX2PVPEmblem::PVPEmblemData* pPVPEmblemData = i->second;
		SAFE_DELETE( pPVPEmblemData );
	}
	m_mapEmblemData.clear();

	for ( int i = 0; i < (int)m_vecReadyTexture.size(); i++ )
	{
		CKTDXDeviceTexture* pTexture = m_vecReadyTexture[i];
		SAFE_CLOSE( pTexture );
	}
	m_vecReadyTexture.clear();
}

CX2PVPEmblem::PVPEmblemData* CX2PVPEmblem::GetPVPEmblemData( IN PVP_RANK eEmblemID_ )
{
	CX2PVPEmblem::PVPEmblemData* pPVPEmblemData = NULL;

	map< CX2PVPEmblem::PVP_RANK, PVPEmblemData* >::iterator i;
	i = m_mapEmblemData.find( eEmblemID_ );

	if ( i != m_mapEmblemData.end() )
	{
		pPVPEmblemData = i->second;
	}

	return pPVPEmblemData;
}

CX2PVPEmblem::PVPEmblemData* CX2PVPEmblem::GetMyNowUnitPVPEmblem()
{
	if ( g_pData->GetMyUser() == NULL || g_pData->GetMyUser()->GetSelectUnit() == NULL )
		return NULL;

		char cRank = g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_cRank;
		return  GetPVPEmblemData( static_cast<CX2PVPEmblem::PVP_RANK>( cRank ) );
}

void CX2PVPEmblem::AddEmblemData_LUA( int emblemID, const char* pName, const char* pTextureName, const char* pTextureKey )
{
	CX2PVPEmblem::PVP_RANK pvpEmblemID = static_cast<CX2PVPEmblem::PVP_RANK>( emblemID );

	if ( m_mapEmblemData.find( pvpEmblemID ) != m_mapEmblemData.end() )
	{
		MessageBox( g_pKTDXApp->GetHWND(), GET_STRING( STR_ID_434 ), GET_STRING( STR_ID_435 ), MB_OK );
		return;
	}

	CX2PVPEmblem::PVPEmblemData* pPVPEmblemData = new CX2PVPEmblem::PVPEmblemData();
	pPVPEmblemData->m_EmblemID = pvpEmblemID;
	ConvertUtf8ToWCHAR( pPVPEmblemData->m_EmblemName, pName );

	ConvertUtf8ToWCHAR( pPVPEmblemData->m_TextureName, pTextureName );
	ConvertUtf8ToWCHAR( pPVPEmblemData->m_TextureKey, pTextureKey );

	CKTDXDeviceTexture* pTexture = g_pKTDXApp->GetDeviceManager()->OpenTexture( pPVPEmblemData->m_TextureName );
	if ( pTexture != NULL )
		m_vecReadyTexture.push_back( pTexture );

	m_mapEmblemData.insert( std::make_pair( pvpEmblemID, pPVPEmblemData ) );
}

bool CX2PVPEmblem::OpenScript( const WCHAR* pFileName )
{
	if ( pFileName == NULL )
		return false;

	//바인더에 등록
	lua_tinker::decl( g_pKTDXApp->GetLuaBinder()->GetLuaState(),  "g_pX2PVPEmblem",	this );

    if ( g_pKTDXApp->LoadLuaTinker( pFileName ) == false )
    {
		MessageBox( g_pKTDXApp->GetHWND(), pFileName, GET_STRING( STR_ID_436 ), MB_OK );
		return false;
    }

	return true;
}


CX2PVPEmblem::PVP_EMBLEM CX2PVPEmblem::GetEmblemID( int iRating )
{
	if( iRating < CX2PVPEmblem::PE_RANK_D )
		return CX2PVPEmblem::PE_RANK_E;
	else if( iRating < CX2PVPEmblem::PE_RANK_C )
		return CX2PVPEmblem::PE_RANK_D;
	else if( iRating < CX2PVPEmblem::PE_RANK_B )
		return CX2PVPEmblem::PE_RANK_C;
	else if( iRating < CX2PVPEmblem::PE_RANK_A )
		return CX2PVPEmblem::PE_RANK_B;
	else if( iRating < CX2PVPEmblem::PE_RANK_S )
		return CX2PVPEmblem::PE_RANK_S;
	else if( iRating < CX2PVPEmblem::PE_RANK_SS )
		return CX2PVPEmblem::PE_RANK_S;
	else if( iRating < CX2PVPEmblem::PE_RANK_SSS )
		return CX2PVPEmblem::PE_RANK_SS;
	else
		return CX2PVPEmblem::PE_RANK_SSS;
}
CX2PVPEmblem::PVP_EMBLEM CX2PVPEmblem::GetEmblemID( CX2PVPEmblem::PVP_RANK eRank )
{
	switch( eRank )
	{
	case PVPRANK_NONE:
		return PE_NONE;
	case PVPRANK_RANK_E:
		return PE_RANK_E;
	case PVPRANK_RANK_D:
		return PE_RANK_D;
	case PVPRANK_RANK_C:
		return PE_RANK_C;
	case PVPRANK_RANK_B:
		return PE_RANK_B;
	case PVPRANK_RANK_A:
		return PE_RANK_A;
	case PVPRANK_RANK_S:
		return PE_RANK_S;
	case PVPRANK_RANK_SS:
		return PE_RANK_SS;
	case PVPRANK_RANK_SSS:
		return PE_RANK_SSS;
	default:
		return PE_NONE;
	}
}
