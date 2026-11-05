#include "stdafx.h"
#include "./X2StringFilter.h"

CX2StringFilter::CX2StringFilter()
{
	m_BanNickNameList.reserve( 512 );
	m_BanWordList.reserve( 2048 );
	m_BanPostWordList.reserve( 512 );

	lua_tinker::decl( g_pKTDXApp->GetLuaBinder()->GetLuaState(),  "StringFilter", this );

	//{{ 임홍락 [2012.05.16] 클라이언트 단일화 // CLIENT_USE_NATION_FLAG 필수 포함임
    if ( g_pKTDXApp->LoadLuaTinker( L"StringFilter.lua" ) == false )
	//}}
    {
		ErrorLogMsg( XEM_ERROR65, "StringFilter.lua 파일 불러오기 오류" );
		//g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(250,300), L"StringFilter파일 불러오기 오류", this );
		MessageBox( g_pKTDXApp->GetHWND(), L"StringFilter.lua", GET_STRING( STR_ID_745 ), MB_OK );
		return;
    }

	//if( m_KLuaManager.BeginTable( "BanNickNameList" ) == true )
	//{
	//	wstring banNickName;
	//	int tableIndex = 1;

	//	while( m_KLuaManager.GetValue( tableIndex, banNickName ) == true )
	//	{
	//		MakeUpperCase( banNickName );
	//		m_BanNickNameList.push_back( banNickName );
	//		tableIndex++;
	//	}

	//	m_KLuaManager.EndTable();
	//}

	//if( m_KLuaManager.BeginTable( "BanWordList" ) == true )
	//{
	//	wstring banWord;
	//	int tableIndex = 1;

	//	while( m_KLuaManager.GetValue( tableIndex, banWord ) == true )
	//	{
	//		MakeUpperCase( banWord );
	//		m_BanWordList.push_back( banWord );
	//		tableIndex++;
	//	}

	//	m_KLuaManager.EndTable();
	//}

}

CX2StringFilter::~CX2StringFilter(void)
{

}

//{{ 임홍락 [2012.05.16] 클라이언트 단일화 // CLIENT_USE_NATION_FLAG 필수 포함임
//}}

void CX2StringFilter::AddPostWordFilter_LUA( char* szFilter )
{
	std::wstring banPostWord;
	ConvertUtf8ToWCHAR( banPostWord, szFilter );
	MakeUpperCase( banPostWord );

	m_BanPostWordList.push_back( banPostWord );
}

bool CX2StringFilter::CheckIsValidPostWordString( FILTER_TYPE filterType, const WCHAR* pString, OUT wstring &pOutString )
{
	wstring pStringForCheck = pString;
	MakeUpperCase(pStringForCheck);

	if ( filterType == FT_POST )
	{
		for ( int i = 0; i < (int)m_BanPostWordList.size(); i++ )
		{
			wstring banPostWord = m_BanPostWordList[i];
			if ( (int)pStringForCheck.find( banPostWord.c_str() ) != -1 ) //찾았다
			{
				pOutString = banPostWord;
				return false;
			}
		}
	}

	return true;
}

bool CX2StringFilter::CheckIsValidString( FILTER_TYPE filterType, const WCHAR* pString )
{
	wstring pStringForCheck = pString;
	MakeUpperCase(pStringForCheck);

	if ( filterType == FT_NICKNAME )
	{

		for ( int i = 0; i < (int)pStringForCheck.size(); i++ )
		{
			WCHAR tempChar = pStringForCheck[i];
// 09.08.24 태완 : 한글 필터링 하는 부분을 중국어 코드 필터링으로 바꿈
			if ( tempChar >= 0xAC00 && tempChar <= 0xD7AF )
			{
				return false;
			}



			if ( tempChar >= 'A' && tempChar <= 'Z' )
			{
				continue;
			}

			if ( tempChar >= '0' && tempChar <= '9' )
			{
				continue;
			}

			return false;
		}






		for ( int i = 0; i < (int)m_BanNickNameList.size(); i++ )
		{
			wstring banNickName = m_BanNickNameList[i];
			if ( pStringForCheck == banNickName )
			{
				return false;
			}
		}

		//{{ 2009. 2. 17  최육사	닉네임 포함 문자
		for( int i = 0; i < (int)m_BanNickNameWordList.size(); ++i )
		{
			wstring banNickNameWord = m_BanNickNameWordList[i];
			if( (int)pStringForCheck.find( banNickNameWord.c_str() ) != -1 ) // 찾았다
			{
				return false;
			}
		}
		//}}

		for ( int i = 0; i < (int)m_BanWordList.size(); i++ )
		{
			wstring banWord = m_BanWordList[i];
			if ( (int)pStringForCheck.find( banWord.c_str() ) != -1 ) //찾았다
			{
				return false;
			}
		}

		if ( (int)pStringForCheck.find( L" " ) != -1 )
		{
			return false;
		}

		if ( (int)pStringForCheck.find( L"\"" ) != -1 )
		{
			return false;
		}

		if ( (int)pStringForCheck.find( L"/" ) != -1 )
		{
			return false;
		}

		if ( (int)pStringForCheck.find( L"\\" ) != -1 )
		{
			return false;
		}

		if ( (int)pStringForCheck.find( L"(" ) != -1 )
		{
			return false;
		}
		if ( (int)pStringForCheck.find( L")" ) != -1 )
		{
			return false;
		}

		if ( (int)pStringForCheck.find( L"]" ) != -1 )
		{
			return false;
		}

		if ( (int)pStringForCheck.find( L"[" ) != -1 )
		{
			return false;
		}

		if ( (int)pStringForCheck.find( L"%" ) != -1 )
		{
			return false;
		}
		if ( (int)pStringForCheck.find( L"!" ) != -1 )
		{
			return false;
		}
		if ( (int)pStringForCheck.find( L"GM" ) != -1 )
		{
			return false;
		}
		if ( (int)pStringForCheck.find( GET_STRING( STR_ID_746 ) ) != -1 )
		{
			return false;
		}
/*
		WCHAR noWanSungWordList[] = L"ㄱㄲㄴㄷㄸㄹㅁㅂㅃㅅㅆㅇㅈㅉㅊㅋㅌㅍㅎㅏㅐㅑㅒㅓㅔㅕㅖㅗㅘㅙㅚㅛㅜㅝㅞㅟㅠㅡㅢㅣㄳㄵㄶㄺㄻㄼㄽㄾㄿㅀㅄ";

		for ( int i = 0; i < (int)sizeof(noWanSungWordList)/sizeof(WCHAR); i++ )
		{
			WCHAR noWanSungWord = noWanSungWordList[i];
			if ( (int)pStringForCheck.find( noWanSungWord ) != -1 )
			{
				return false;
			}
		}
*/
	}
	else if ( filterType == FT_CHAT )
	{
		for ( int i = 0; i < (int)m_BanWordList.size(); i++ )
		{
			wstring banWord = m_BanWordList[i];
			if ( (int)pStringForCheck.find( banWord.c_str() ) != -1 ) //찾았다
			{
                return false;
			}
		}
		if ( (int)pStringForCheck.find( '\r' ) != -1 )
		{
			return false;
		}

	}
	else if ( filterType == FT_POST )
	{
		for ( int i = 0; i < (int)m_BanPostWordList.size(); i++ )
		{
			wstring banPostWord = m_BanPostWordList[i];
			if ( (int)pStringForCheck.find( banPostWord.c_str() ) != -1 ) //찾았다
			{
				return false;
			}
		}
	}
	else
	{
		MessageBox( g_pKTDXApp->GetHWND(), GET_STRING( STR_ID_747 ), GET_STRING( STR_ID_748 ), MB_OK );
		return false;
	}

	return true;
}

wstring CX2StringFilter::FilteringNoteString( const WCHAR* pString, WCHAR wcharForReplace )
{
    if ( pString == NULL )
    {
        wstring filteringErrorMsg = GET_STRING( STR_ID_749 );
        return filteringErrorMsg;
    }

	wstring stringForCheck = pString;
	wstring orgStringForCheck = pString;
	MakeUpperCase( stringForCheck );

	for ( int i = 0; i < (int)m_BanWordList.size(); i++ )
	{
		wstring banWord = m_BanWordList[i];
		int iLocation = 0;

		if(banWord.compare(L"\n") == 0)
			continue;

		while( (int)stringForCheck.find( banWord.c_str(), iLocation ) != -1 ) //찾았다
		{
			int index = stringForCheck.find( banWord.c_str(), iLocation );
			int banWordSize = (int)banWord.size();
			WCHAR buff[256] = {0};
			for ( int i = 0; i < banWordSize; i++ )
			{
				buff[i] = wcharForReplace;
				buff[i + 1] = L'\0';
			}
			wstring stringForReplace = buff;
			orgStringForCheck.replace( index, banWordSize, stringForReplace );
			//stringForCheck = orgStringForCheck;
			iLocation = index + banWordSize;
		}
	}

	return orgStringForCheck;
}

wstring CX2StringFilter::FilteringChatString( const WCHAR* pString, WCHAR wcharForReplace )
{
	if ( pString == NULL )
	{
		wstring filteringErrorMsg = GET_STRING( STR_ID_749 );
		return filteringErrorMsg;
	}

	wstring stringForCheck = pString;
	wstring orgStringForCheck = pString;
	MakeUpperCase( stringForCheck );

	int nStringLength = lstrlen(pString);
	for(int i=0; i<nStringLength; ++i)
	{
		if( pString[i] == 0x1C ||
			pString[i] == 0x1D ||
			pString[i] == 0x1E ||
			pString[i] == 0x1F )
		{
			return L"♡♡♡!";
		}
	}

	for ( int i = 0; i < (int)m_BanWordList.size(); i++ )
	{
		wstring banWord = m_BanWordList[i];
		int iLocation = 0;

		while( (int)stringForCheck.find( banWord.c_str(), iLocation ) != -1 ) //찾았다
		{
			int index = stringForCheck.find( banWord.c_str(), iLocation );
			int banWordSize = (int)banWord.size();
			WCHAR buff[256] = {0};
			for ( int i = 0; i < banWordSize; i++ )
			{
				buff[i] = wcharForReplace;
				buff[i + 1] = L'\0';
			}
			wstring stringForReplace = buff;
			orgStringForCheck.replace( index, banWordSize, stringForReplace );
			//stringForCheck = orgStringForCheck;
			iLocation = index + banWordSize;
		}
	}

	return orgStringForCheck;


}

bool CX2StringFilter::CheckIsKoreanNEnglishNNumber( const WCHAR* pString )
{
	wstring pStringForCheck = pString;
	MakeUpperCase(pStringForCheck);

	for ( int i = 0; i < (int)pStringForCheck.size(); i++ )
	{
		WCHAR tempChar = pStringForCheck[i];

		if ( tempChar >= 0xAC00 && tempChar <= 0xD7AF )
		{
			return false;
		}

		if ( tempChar >= 'A' && tempChar <= 'Z' )
		{
			continue;
		}

		if ( tempChar >= '0' && tempChar <= '9' )
		{
			continue;
		}

		return false;
	}


	return true;
}


void CX2StringFilter::AddNickNameFilter_LUA( char* szFilter )
{
    std::wstring banNickName;
    ConvertUtf8ToWCHAR( banNickName, szFilter );
	MakeUpperCase( banNickName );

	m_BanNickNameList.push_back( banNickName );
}

//{{ 2009. 2. 17  최육사	닉네임 포함 문자
void CX2StringFilter::AddNickNameWordFilter_LUA( char* szFilter )
{
    std::wstring banNickNameWord;
    ConvertUtf8ToWCHAR( banNickNameWord, szFilter );
    MakeUpperCase( banNickNameWord );

	m_BanNickNameWordList.push_back( banNickNameWord );
}
//}}

void CX2StringFilter::AddWordFilter_LUA( char* szFilter )
{
    std::wstring banWord;
    ConvertUtf8ToWCHAR( banWord, szFilter );
    MakeUpperCase( banWord );

	m_BanWordList.push_back( banWord );
}
