//{{ kimhc // 2009-07-09 // 헤니르의 시공에 추가되는 Ranking UI 작업
#include "stdafx.h"


#include "./X2UIRankingInfo.h"
#include < iomanip >

CX2UIRankingInfo::CX2UIRankingInfo( const CKTDXStage* const pNowStage, const WCHAR* const pFileName ) :
m_bShow( false ),
m_pNowStage( pNowStage ),
m_pDlgRankingInfo( NULL ),
m_nowRankingInfoType( RT_DAY_RANKING ),
m_uiNowPage( 1 ),
m_uiMaxPage( 1 ),
m_vPosDlg( 0, 0 ),
m_vSizeDlg( 0, 0 ),
m_pStaticCommonInfo( NULL ),
m_pStaticHenirInfo( NULL ),
m_pStaticPvpInfo( NULL ),
m_pStaticLevelInfo( NULL ),
m_pRadioButtonDaily( NULL ),
m_pRadioButtonWeekly( NULL ),
m_pRadioButtonMonthly( NULL )
//{{ 허상형 : [2009/8/21] //	신학기 이벤트 관련 컨트롤 변수 초기화
//}} 허상형 : [2009/8/21] //	신학기 이벤트 관련 컨트롤 변수 초기화
//,m_pPicCharRank( NULL )
{


	m_pDlgRankingInfo		=		new CKTDGUIDialog( const_cast< CKTDXStage* >( pNowStage ), pFileName );

	if ( m_pDlgRankingInfo == NULL )
	{
		ASSERT( false );
		return;
	}

	g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDlgRankingInfo );
	m_pDlgRankingInfo->SetDisableUnderWindow( true );

	m_vPosDlg.x			= m_pDlgRankingInfo->GetDummyPos( 0 ).x;		// 0은 posion
	m_vPosDlg.y			= m_pDlgRankingInfo->GetDummyPos( 0 ).y;

	m_vSizeDlg.x		= m_pDlgRankingInfo->GetDummyPos( 1 ).x;		// 1은 size
	m_vSizeDlg.y		= m_pDlgRankingInfo->GetDummyPos( 1 ).y;

	// 모든 타입정보에 다 쓰이는 컨트롤
	m_pStaticCommonInfo				= static_cast< CKTDGUIStatic* >( m_pDlgRankingInfo->GetControl( L"g_pCommonStatic" ) );

	if ( m_pStaticCommonInfo != NULL )
	{
		m_pStaticCommonInfo->SetString( 0, GET_STRING( STR_ID_1139 ) );		// 순위
		m_pStaticCommonInfo->SetString( 1, GET_STRING( STR_ID_3727 ) );		// 직업
		m_pStaticCommonInfo->SetString( 2, GET_STRING( STR_ID_1127 ) );		// 닉네임
	}



	// 헤니르시공 랭킹 정보에 쓰이는 컨트롤
	m_pStaticHenirInfo		= static_cast< CKTDGUIStatic* >( m_pDlgRankingInfo->GetControl( L"g_pHenirStatic" ) );
	m_pRadioButtonDaily		= static_cast< CKTDGUIRadioButton* >( m_pDlgRankingInfo->GetControl( L"RadioButton_Day" ) );
	m_pRadioButtonWeekly	= static_cast< CKTDGUIRadioButton* >( m_pDlgRankingInfo->GetControl( L"RadioButton_Week" ) );
	m_pRadioButtonMonthly	= static_cast< CKTDGUIRadioButton* >( m_pDlgRankingInfo->GetControl( L"RadioButton_Month" ) );

	//{{ 허상형 : [2009/8/20] //	신학기 관련 컨트롤 로딩
	//}} 허상형 : [2009/8/20] //	신학기 관련 컨트롤 로딩


	if ( m_pStaticHenirInfo != NULL )
	{
		m_pStaticHenirInfo->SetString( 0, GET_STRING( STR_ID_3728 ) );		// 스테이지/남은시간
	}

	// 대전 랭킹 정보에 쓰이는 컨트롤
	m_pStaticPvpInfo = static_cast< CKTDGUIStatic* >( m_pDlgRankingInfo->GetControl( L"g_pPvpStatic" ) );

	if ( m_pStaticPvpInfo != NULL )
	{
		m_pStaticPvpInfo->SetString( 0, GET_STRING( STR_ID_3729) );		// 대전랭크
		m_pStaticPvpInfo->SetString( 1, GET_STRING( STR_ID_3730) );		// 승/패
	}

	// 레벨(경험치) 랭킹 정보에 쓰이는 컨트롤
	m_pStaticLevelInfo = static_cast< CKTDGUIStatic* >( m_pDlgRankingInfo->GetControl( L"g_pStaticExp" ) );

	if ( m_pStaticLevelInfo != NULL )
	{
		m_pStaticLevelInfo->SetString( 0, GET_STRING( STR_ID_3731) );	// EXP
	}

//{{ 허상형 : [2009/8/21] //	신학기 이벤트 스태틱 컨트롤 설정
//}} 허상형 : [2009/8/21] //	신학기 이벤트 스태틱 컨트롤 설정


}

CX2UIRankingInfo::~CX2UIRankingInfo()
{
	m_pStaticCommonInfo		= NULL;
	m_pStaticHenirInfo		= NULL;
	m_pStaticPvpInfo		= NULL;
	m_pStaticLevelInfo		= NULL;

	m_pRadioButtonDaily		= NULL;
	m_pRadioButtonWeekly	= NULL;
	m_pRadioButtonMonthly	= NULL;

	//{{ 허상형 : [2009/8/21] //	신학기 이벤트 컨트롤 변수 해제
	//}} 허상형 : [2009/8/21] //	신학기 이벤크 컨트롤 변수 해제

	DestoryUIRankingInfo();
	SAFE_DELETE_DIALOG( m_pDlgRankingInfo );

}

void		CX2UIRankingInfo::InitUIRankingInfo()
{
	m_nowRankingInfoType = RT_DAY_RANKING;
	SetShowControlsByType( RT_DAY_RANKING, true );
	SetShowControlsByType( RT_PVP_RANKING, false );
	SetShowControlsByType( RT_LEVEL_RANKING, false );

	//{{ 허상형 : [2009/8/21] //
	//}} 허상형 : [2009/8/21] //
}

void		CX2UIRankingInfo::InitMyColumn()
{
	// 랭킹
	m_columnForMyRank.m_pStaticRanking		= static_cast< CKTDGUIStatic* >( m_pDlgRankingInfo->GetControl( L"g_pStaticMyScore" ) );
	m_columnForMyRank.m_pStaticRanking->SetShowEnable( false, false );

	// 캐릭터 Icon
	m_columnForMyRank.m_pStaticClass		= static_cast< CKTDGUIStatic* >( m_pDlgRankingInfo->GetControl( L"Static_My_Class" ) );
	m_columnForMyRank.m_pStaticClass->GetPicture( 0 )->SetShow( true );
	m_columnForMyRank.m_pStaticClass->GetPicture( 1 )->SetShow( true );
	m_columnForMyRank.m_pStaticClass->GetPicture( 2 )->SetShow( true );
	m_columnForMyRank.m_pStaticClass->GetPicture( 3 )->SetShow( true );
	m_columnForMyRank.m_pStaticClass->SetShowEnable( false, false );


	// 닉네임
	m_columnForMyRank.m_pStaticNickName		= static_cast< CKTDGUIStatic* >( m_pDlgRankingInfo->GetControl( L"g_pStaticMyNickName" ) );
	m_columnForMyRank.m_pStaticNickName->SetShowEnable( false, false );

	// 클리어 스테이지
	m_columnForMyRank.m_pStaticClearStage	= static_cast< CKTDGUIStatic* >( m_pDlgRankingInfo->GetControl( L"Static_Clear_Stage_My" ) );
	m_columnForMyRank.m_pStaticClearStage->SetShowEnable( false, false );

	// 클리어 했을 때의 플레이 시간
	m_columnForMyRank.m_pStaticPlayTime	= static_cast< CKTDGUIStatic* >( m_pDlgRankingInfo->GetControl( L"Static_Clear_Time_My" ) );
	m_columnForMyRank.m_pStaticPlayTime->SetShowEnable( false, false );

	// 헤니르 시공의 기록이 등록된 시간
	m_columnForMyRank.m_pButtonRegistredDate	= static_cast< CKTDGUIButton* >( m_pDlgRankingInfo->GetControl( L"ButtonMyOpacity" ) );
	m_columnForMyRank.m_pButtonRegistredDate->SetShowEnable( false, false );

	// 대전 엠블렘
	m_columnForMyRank.m_pStaticPvpEmblem	= static_cast< CKTDGUIStatic* >( m_pDlgRankingInfo->GetControl( L"Static_My_Pvp_Emblem" ) );
	m_columnForMyRank.m_pStaticPvpEmblem->GetPicture( 0 )->SetShow( true );
	m_columnForMyRank.m_pStaticPvpEmblem->SetShowEnable( false, false );

	// 대전 승 횟수
	m_columnForMyRank.m_pStaticWin			= static_cast< CKTDGUIStatic* >( m_pDlgRankingInfo->GetControl( L"Static_Win_My" ) );
	m_columnForMyRank.m_pStaticWin->SetShowEnable( false, false );

	// 대전 패 횟수
	m_columnForMyRank.m_pStaticLose			= static_cast< CKTDGUIStatic* >( m_pDlgRankingInfo->GetControl( L"Static_Lose_My" ) );
	m_columnForMyRank.m_pStaticLose->SetShowEnable( false, false );

	// 경험치
	m_columnForMyRank.m_pStaticExp			= static_cast< CKTDGUIStatic* >( m_pDlgRankingInfo->GetControl( L"Static_EXP_My" ) );
	m_columnForMyRank.m_pStaticExp->SetShowEnable( false, false );

}

void		CX2UIRankingInfo::InitOthersColumn()
{
	// 다른 유저들의 순위 column의 캐릭터 Icon 초기화

	for ( int i = 0; i < _CONST_UIRANKING_INFO_::g_iNumRankingPerPage; i++ )
	{
		// 순위표시
		m_columnForOthersRank[i].m_pStaticRanking		=
			static_cast< CKTDGUIStatic* >( m_pDlgRankingInfo->GetControl( GET_REPLACED_STRING( ( STR_ID_3738, "Li", std::wstring( L"StaticRankOthers" ), i + 1 ) ) ) );
		m_columnForOthersRank[i].m_pStaticRanking->SetShowEnable( false, false );

		// 캐릭터 Icon
		m_columnForOthersRank[i].m_pStaticClass			=
			static_cast< CKTDGUIStatic* >( m_pDlgRankingInfo->GetControl( GET_REPLACED_STRING( ( STR_ID_3738, "Li", std::wstring( L"Static_Class" ), i + 1 ) ) ) );
		m_columnForOthersRank[i].m_pStaticClass->GetPicture( 0 )->SetShow( true );	// 캐릭터 이미지
		m_columnForOthersRank[i].m_pStaticClass->GetPicture( 1 )->SetShow( true );	// 클래스 Initial
		m_columnForOthersRank[i].m_pStaticClass->GetPicture( 2 )->SetShow( true );	// 레벨 10의 자리
		m_columnForOthersRank[i].m_pStaticClass->GetPicture( 3 )->SetShow( true );	// 레벨 1의 자리
		m_columnForOthersRank[i].m_pStaticClass->SetShowEnable( false, false );

		// 닉네임
		m_columnForOthersRank[i].m_pStaticNickName		=
			static_cast< CKTDGUIStatic* >( m_pDlgRankingInfo->GetControl( GET_REPLACED_STRING( ( STR_ID_3738, "Li", std::wstring( L"g_pStaticNickName" ), i + 1 ) ) ) );
		m_columnForOthersRank[i].m_pStaticNickName->SetShowEnable( false, false );

		// 클리어 스테이지
		m_columnForOthersRank[i].m_pStaticClearStage	=
			static_cast< CKTDGUIStatic* >( m_pDlgRankingInfo->GetControl( GET_REPLACED_STRING( ( STR_ID_3738, "Li", std::wstring( L"Static_Clear_Stage" ), i + 1 ) ) ) );
		m_columnForOthersRank[i].m_pStaticClearStage->SetShowEnable( false, false );

		// 클리어 했을 때의 플레이 시간
		m_columnForOthersRank[i].m_pStaticPlayTime	=
			static_cast< CKTDGUIStatic* >( m_pDlgRankingInfo->GetControl( GET_REPLACED_STRING( ( STR_ID_3738, "Li", std::wstring( L"Static_Clear_Time" ), i + 1 ) ) ) );
		m_columnForOthersRank[i].m_pStaticPlayTime->SetShowEnable( false, false );

		// 헤니르 시공의 기록이 등록된 시간
		m_columnForOthersRank[i].m_pButtonRegistredDate	=
			static_cast< CKTDGUIButton* >( m_pDlgRankingInfo->GetControl( GET_REPLACED_STRING( ( STR_ID_3738, "Li", std::wstring( L"opacity" ), i + 1 ) ) ) );
		m_columnForOthersRank[i].m_pButtonRegistredDate->SetShowEnable( false, false );

		// 대전 엠블렘
		m_columnForOthersRank[i].m_pStaticPvpEmblem		=
			static_cast< CKTDGUIStatic* >( m_pDlgRankingInfo->GetControl( GET_REPLACED_STRING( ( STR_ID_3738, "Li", std::wstring( L"Static_Pvp_Emblem" ), i + 1 ) ) ) );
		m_columnForOthersRank[i].m_pStaticPvpEmblem->GetPicture( 0 )->SetShow( true );
		m_columnForOthersRank[i].m_pStaticPvpEmblem->SetShowEnable( false, false );

		// 대전 승 횟수
		m_columnForOthersRank[i].m_pStaticWin			=
			static_cast< CKTDGUIStatic* >( m_pDlgRankingInfo->GetControl( GET_REPLACED_STRING( ( STR_ID_3738, "Li", std::wstring( L"Static_Win" ), i + 1 ) ) ) );
		m_columnForOthersRank[i].m_pStaticWin->SetShowEnable( false, false );

		// 대전 패 횟수
		m_columnForOthersRank[i].m_pStaticLose			=
			static_cast< CKTDGUIStatic* >( m_pDlgRankingInfo->GetControl( GET_REPLACED_STRING( ( STR_ID_3738, "Li", std::wstring( L"Static_Lose" ), i + 1 ) ) ) );
		m_columnForOthersRank[i].m_pStaticLose->SetShowEnable( false, false );

		// 경험치
		m_columnForOthersRank[i].m_pStaticExp			=
			static_cast< CKTDGUIStatic* >( m_pDlgRankingInfo->GetControl( GET_REPLACED_STRING( ( STR_ID_3738, "Li", std::wstring( L"Static_EXP" ), i + 1 ) ) ) );
		m_columnForOthersRank[i].m_pStaticExp->SetShowEnable( false, false );

	}

}

void		CX2UIRankingInfo::SetShowControlsByType( RANKING_TYPE type, bool bShow )
{
    //{{ seojt // 2009-8-21, 12:15
    /** 'm_pRadioButtonDaily'등을 얻어낸 'm_pDlgRankingInfo'가 이미 invalid하다면,
        리턴한다.
        - jintaeks on 2009-08-21, 12:15 */
    if( m_pDlgRankingInfo == NULL )
    {
        return;
    }//if
    //}} seojt // 2009-8-21, 12:15

	switch ( type )
	{
	case RT_DAY_RANKING:
	case RT_WEEK_RANKING:
	case RT_MONTH_RANKING:
		{
			CKTDGUIRadioButton*		pRadioButton =
				static_cast< CKTDGUIRadioButton* >( m_pDlgRankingInfo->GetControl( L"RadioButton_Henir" ) );
			pRadioButton->SetChecked( bShow );

			if ( m_pRadioButtonDaily )
				m_pRadioButtonDaily->SetShowEnable( bShow, bShow );

            if ( m_pRadioButtonWeekly )
				m_pRadioButtonWeekly->SetShowEnable( bShow, bShow );

			if ( m_pRadioButtonMonthly )
				m_pRadioButtonMonthly->SetShowEnable( bShow, bShow );
			if ( m_pStaticHenirInfo )
				m_pStaticHenirInfo->SetShowEnable( bShow, bShow );

			if ( type == RT_DAY_RANKING )
				m_pRadioButtonDaily->SetChecked( true );

		}
		break;

	case RT_LEVEL_RANKING:
		{
			CKTDGUIRadioButton*		pRadioButton =
				static_cast< CKTDGUIRadioButton* >( m_pDlgRankingInfo->GetControl( L"RadioButton_Level" ) );
			pRadioButton->SetChecked( bShow );
			if ( m_pStaticLevelInfo )
				m_pStaticLevelInfo->SetShowEnable( bShow, bShow );
		}
		break;

	case RT_PVP_RANKING:
		{
			CKTDGUIRadioButton*		pRadioButton =
				static_cast< CKTDGUIRadioButton* >( m_pDlgRankingInfo->GetControl( L"RadioButton_VS" ) );
			pRadioButton->SetChecked( bShow );

			if ( m_pStaticPvpInfo )
				m_pStaticPvpInfo->SetShowEnable( bShow, bShow );

		}
		break;


		//{{ 허상형 : [2009/8/20] //	신학기 이벤트
		//}} 허상형 : [2009/8/20] //	신학기 이벤트

	default:	// MAX_NUM
		break;
	}
}

void		CX2UIRankingInfo::SetShowMyInfoByType( RANKING_TYPE type, bool bShow )
{
	m_columnForMyRank.m_pStaticRanking->SetShowEnable( bShow, bShow );
	m_columnForMyRank.m_pStaticClass->SetShowEnable( bShow, bShow );
	m_columnForMyRank.m_pStaticNickName->SetShowEnable( bShow, bShow );

	switch ( type )
	{
	case RT_DAY_RANKING:
	case RT_WEEK_RANKING:
	case RT_MONTH_RANKING:
		{
			m_columnForMyRank.m_pStaticClearStage->SetShowEnable( bShow, bShow );
			m_columnForMyRank.m_pStaticPlayTime->SetShowEnable( bShow, bShow );
			m_columnForMyRank.m_pButtonRegistredDate->SetShowEnable( bShow, bShow );

		}
		break;

	case RT_LEVEL_RANKING:
		{
			m_columnForMyRank.m_pStaticExp->SetShowEnable( bShow, bShow );
		}
		break;

	case RT_PVP_RANKING:
		{
			m_columnForMyRank.m_pStaticPvpEmblem->SetShowEnable( bShow, bShow );
			m_columnForMyRank.m_pStaticWin->SetShowEnable( bShow, bShow );
			m_columnForMyRank.m_pStaticLose->SetShowEnable( bShow, bShow );
		}
		break;

		//{{ 허상형 : [2009/8/21] //	신학기 이벤트 내 점수 관련 컨트롤 활성화
		//}} 허상형 : [2009/8/21] //	신학기 이벤트 내 점수 관련 컨트롤 활성화

	default:	// MAX_NUM
		{
			m_columnForMyRank.m_pStaticClearStage->SetShowEnable( bShow, bShow );
			m_columnForMyRank.m_pStaticPlayTime->SetShowEnable( bShow, bShow );
			m_columnForMyRank.m_pButtonRegistredDate->SetShowEnable( bShow, bShow );
			m_columnForMyRank.m_pStaticPvpEmblem->SetShowEnable( bShow, bShow );
			m_columnForMyRank.m_pStaticWin->SetShowEnable( bShow, bShow );
			m_columnForMyRank.m_pStaticLose->SetShowEnable( bShow, bShow );
			m_columnForMyRank.m_pStaticExp->SetShowEnable( bShow, bShow );
		}
		break;
	}
}

void		CX2UIRankingInfo::SetShowOthersInfoByType( ColumnForRank* const pColumnForOthersRank, RANKING_TYPE type, bool bShow )
{
	pColumnForOthersRank->m_pStaticRanking->SetShowEnable( bShow, bShow );
	pColumnForOthersRank->m_pStaticClass->SetShowEnable( bShow, bShow );
	pColumnForOthersRank->m_pStaticNickName->SetShowEnable( bShow, bShow );

	switch ( type )
	{
	case RT_DAY_RANKING:
	case RT_WEEK_RANKING:
	case RT_MONTH_RANKING:
		{
			pColumnForOthersRank->m_pStaticClearStage->SetShowEnable( bShow, bShow );
			pColumnForOthersRank->m_pStaticPlayTime->SetShowEnable( bShow, bShow );
			pColumnForOthersRank->m_pButtonRegistredDate->SetShowEnable( bShow, bShow );

		}
		break;

	case RT_LEVEL_RANKING:
		{
			pColumnForOthersRank->m_pStaticExp->SetShowEnable( bShow, bShow );
		}
		break;

	case RT_PVP_RANKING:
		{
			pColumnForOthersRank->m_pStaticPvpEmblem->SetShowEnable( bShow, bShow );
			pColumnForOthersRank->m_pStaticWin->SetShowEnable( bShow, bShow );
			pColumnForOthersRank->m_pStaticLose->SetShowEnable( bShow, bShow );
		}
		break;

		//{{ 허상형 : [2009/8/21] //	신학기 이벤트 내 점수 관련 컨트롤 활성화
		//}} 허상형 : [2009/8/21] //	신학기 이벤트 내 점수 관련 컨트롤 활성화

	default:	// MAX_NUM	// 이곳에서는 아무 정보도 출력하지 않아야 하는 column 용도로 사용
		{
			pColumnForOthersRank->m_pStaticClearStage->SetShowEnable( bShow, bShow );
			pColumnForOthersRank->m_pStaticPlayTime->SetShowEnable( bShow, bShow );
			pColumnForOthersRank->m_pButtonRegistredDate->SetShowEnable( bShow, bShow );
			pColumnForOthersRank->m_pStaticPvpEmblem->SetShowEnable( bShow, bShow );
			pColumnForOthersRank->m_pStaticWin->SetShowEnable( bShow, bShow );
			pColumnForOthersRank->m_pStaticLose->SetShowEnable( bShow, bShow );
			pColumnForOthersRank->m_pStaticExp->SetShowEnable( bShow, bShow );
		}
		break;
	}
}

///////////핸들러////////////////////////////////////////////////////////////////////////////////

bool		CX2UIRankingInfo::Handler_EGS_GET_RANKING_INFO_REQ()
{
	KEGS_GET_RANKING_INFO_REQ kPacket;

	kPacket.m_iRankingType	= static_cast< int >( m_nowRankingInfoType );
	kPacket.m_uiViewPage	= m_uiNowPage;

	g_pData->GetServerProtocol()->SendPacket( EGS_GET_RANKING_INFO_REQ, kPacket );
	g_pMain->AddServerPacket( EGS_GET_RANKING_INFO_ACK, 60.f );

	return true;
}

bool		CX2UIRankingInfo::Handler_EGS_GET_RANKING_INFO_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_GET_RANKING_INFO_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	g_pMain->DeleteServerPacket( EGS_GET_RANKING_INFO_ACK );
	{

		m_nowRankingInfoType	= static_cast< RANKING_TYPE >( kEvent.m_iRankingType );

		// 랭킹 정보가 우선 다 보이지 않도록 함
		for( int i = 0; i < _CONST_UIRANKING_INFO_::g_iNumRankingPerPage; i++)
			SetShowOthersInfoByType( (m_columnForOthersRank + i), RT_MAX_NUM, false );

		SetShowMyInfoByType( RT_MAX_NUM, false );


		// 현재 페이지 수와 전체 페이지 수 업데이트
		if ( kEvent.m_uiTotalPage > _CONST_UIRANKING_INFO_::g_iMaxPageLimit )
		{
			m_uiMaxPage				= _CONST_UIRANKING_INFO_::g_iMaxPageLimit;
		}
		else
		{
			m_uiMaxPage				= kEvent.m_uiTotalPage;
		}

		m_uiNowPage				= kEvent.m_uiViewPage;

		UpdatePageUI();

		switch ( m_nowRankingInfoType )
		{
		case RT_DAY_RANKING:
		case RT_WEEK_RANKING:
		case RT_MONTH_RANKING:

			{
				// 다른 유저들의 랭킹
				int i = 0;

				//BOOST_TEST_FOREACH( KHenirRankingInfo, kHenirRankingInfo, kEvent.m_vecHenirRankingPage )
				BOOST_TEST_FOREACH( KHenirRankingInfo&, kHenirRankingInfo, kEvent.m_vecHenirRankingPage )
				{
					SetRankNumImage( m_columnForOthersRank[i].m_pStaticRanking, kHenirRankingInfo.m_iRank );

					SetCharIcon( m_columnForOthersRank[i].m_pStaticClass, kHenirRankingInfo.m_cUnitClass,
						kHenirRankingInfo.m_ucLevel );		// 캐릭터 Icon

					if( NULL != g_pData->GetMyUser() && CX2User::XUAL_OPERATOR <= g_pData->GetMyUser()->GetAuthLevel() )
					{
						std::wstring wstrAddUnitUID = kHenirRankingInfo.m_wstrNickName;
						wstrAddUnitUID += L"\nUID: ";
						wstrAddUnitUID += boost::str( boost::wformat( L"%d" ) % kHenirRankingInfo.m_iUnitUID );
						m_columnForOthersRank[i].m_pStaticNickName->SetString( 0, wstrAddUnitUID.c_str() );	// 닉네임
					}
					else
					m_columnForOthersRank[i].m_pStaticNickName->SetString( 0, kHenirRankingInfo.m_wstrNickName.c_str() );	// 닉네임
					m_columnForOthersRank[i].m_pStaticClearStage->SetString( 0, GET_REPLACED_STRING( ( STR_ID_3733, "i", kHenirRankingInfo.m_iStageCount ) ) );
					m_columnForOthersRank[i].m_pStaticPlayTime->SetString( 0, ConvertSecToPlayTimeStr( kHenirRankingInfo.m_ulPlayTime ).c_str() );
					m_columnForOthersRank[i].m_pButtonRegistredDate->SetGuideDesc( ConvertTimeToRegisterDateStr( kHenirRankingInfo.m_tRegDate ).c_str() );

					SetShowOthersInfoByType( (m_columnForOthersRank + i), m_nowRankingInfoType, true );
					i++;
				}

				// 유저 자신의 랭킹 정보
				BOOST_TEST_FOREACH( KHenirRankingInfo&, kMyHenirRankingInfo, kEvent.m_vecMyHenirRanking )
				{
					wstring wstrLatestRanking = CWordLineHandler::GetStrByLineBreakColorInX2Main(GET_STRING( STR_ID_3744 ), 55, XUF_DODUM_13_SEMIBOLD);
					m_columnForMyRank.m_pStaticRanking->GetString( 0 )->fontIndex	=	XUF_DODUM_13_SEMIBOLD;
					m_columnForMyRank.m_pStaticRanking->SetString( 0, wstrLatestRanking.c_str());

					SetCharIcon( m_columnForMyRank.m_pStaticClass, kMyHenirRankingInfo.m_cUnitClass,
						kMyHenirRankingInfo.m_ucLevel );		// 캐릭터 Icon

					m_columnForMyRank.m_pStaticNickName->SetString( 0, kMyHenirRankingInfo.m_wstrNickName.c_str() );	// 닉네임
					m_columnForMyRank.m_pStaticClearStage->SetString( 0, GET_REPLACED_STRING( ( STR_ID_3733, "i", kMyHenirRankingInfo.m_iStageCount ) ) );
					m_columnForMyRank.m_pStaticPlayTime->SetString( 0, ConvertSecToPlayTimeStr( kMyHenirRankingInfo.m_ulPlayTime ).c_str() );
					m_columnForMyRank.m_pButtonRegistredDate->SetGuideDesc( ConvertTimeToRegisterDateStr( kMyHenirRankingInfo.m_tRegDate ).c_str() );

					SetShowMyInfoByType( m_nowRankingInfoType, true );
				}
			}
			break;

		case RT_LEVEL_RANKING:			// enum.h의 RANKING_TYPE의 RT_DUNGEON_RANKING과 같음
			{
				// 다른 유저들의 랭킹
				int i = 0;

				BOOST_TEST_FOREACH( KDungeonRankingInfo&, kLevelRankingInfo, kEvent.m_vecDungeonRankingPage )
				{
					SetRankNumImage( m_columnForOthersRank[i].m_pStaticRanking, kLevelRankingInfo.m_iRank );

					SetCharIcon( m_columnForOthersRank[i].m_pStaticClass, kLevelRankingInfo.m_cUnitClass,
						kLevelRankingInfo.m_ucLevel );		// 캐릭터 Icon

					if( NULL != g_pData->GetMyUser() && CX2User::XUAL_OPERATOR <= g_pData->GetMyUser()->GetAuthLevel() )
					{
						std::wstring wstrAddUnitUID = kLevelRankingInfo.m_wstrNickName;
						wstrAddUnitUID += L"\nUID: ";
						wstrAddUnitUID += boost::str( boost::wformat( L"%d" ) % kLevelRankingInfo.m_iUnitUID );
						m_columnForOthersRank[i].m_pStaticNickName->SetString( 0, wstrAddUnitUID.c_str() );	// 닉네임
					}
					else
					m_columnForOthersRank[i].m_pStaticNickName->SetString( 0, kLevelRankingInfo.m_wstrNickName.c_str() );	// 닉네임
					m_columnForOthersRank[i].m_pStaticExp->SetString( 0, GET_REPLACED_STRING( ( STR_ID_2632, "i", kLevelRankingInfo.m_iEXP ) ) );

					SetShowOthersInfoByType( (m_columnForOthersRank + i), m_nowRankingInfoType, true );
					i++;
				}

				// 유저 자신의 랭킹 정보

				BOOST_TEST_FOREACH( KDungeonRankingInfo&, kMyLevelRankingInfo, kEvent.m_vecMyDungeonRanking )
				{
					m_columnForMyRank.m_pStaticRanking->GetString( 0 )->fontIndex	=	XUF_DODUM_15_BOLD;
					if ( kMyLevelRankingInfo.m_iRank <= _CONST_UIRANKING_INFO_::g_iLimitRankingToDisplay )
						m_columnForMyRank.m_pStaticRanking->SetString( 0, GET_REPLACED_STRING( ( STR_ID_2632, "i", kMyLevelRankingInfo.m_iRank ) ) );
					else
						m_columnForMyRank.m_pStaticRanking->SetString( 0, GET_STRING( STR_ID_3734 ) );

					SetCharIcon( m_columnForMyRank.m_pStaticClass, kMyLevelRankingInfo.m_cUnitClass,
						kMyLevelRankingInfo.m_ucLevel );		// 캐릭터 Icon

					m_columnForMyRank.m_pStaticNickName->SetString( 0, kMyLevelRankingInfo.m_wstrNickName.c_str() );	// 닉네임
					m_columnForMyRank.m_pStaticExp->SetString( 0, GET_REPLACED_STRING( ( STR_ID_2632, "i", kMyLevelRankingInfo.m_iEXP ) ) );

					SetShowMyInfoByType( m_nowRankingInfoType, true );
				}
			}
			break;

		case RT_PVP_RANKING:
			{
				// 다른 유저들의 랭킹
				int i = 0;

				BOOST_TEST_FOREACH( KPvpRankingInfo&, kPvpRankingInfo, kEvent.m_vecPvpRankingPage )
				{
					SetRankNumImage( m_columnForOthersRank[i].m_pStaticRanking, kPvpRankingInfo.m_iRank );

					SetCharIcon( m_columnForOthersRank[i].m_pStaticClass, kPvpRankingInfo.m_cUnitClass,
						kPvpRankingInfo.m_ucLevel );		// 캐릭터 Icon

					if( NULL != g_pData->GetMyUser() && CX2User::XUAL_OPERATOR <= g_pData->GetMyUser()->GetAuthLevel() )
					{
						std::wstring wstrAddUnitUID = kPvpRankingInfo.m_wstrNickName;
						wstrAddUnitUID += L"\nUID: ";
						wstrAddUnitUID += boost::str( boost::wformat( L"%d" ) % kPvpRankingInfo.m_iUnitUID );
						m_columnForOthersRank[i].m_pStaticNickName->SetString( 0, wstrAddUnitUID.c_str() );	// 닉네임
					}
					else
					m_columnForOthersRank[i].m_pStaticNickName->SetString( 0, kPvpRankingInfo.m_wstrNickName.c_str() );	// 닉네임

					CX2PVPEmblem::PVPEmblemData*	pPvpEmblemData	= NULL;



					char cRank = kPvpRankingInfo.m_cRank;

					/// 랭크에 대한 범위 체크
					if ( CX2PVPEmblem::PVPRANK_NONE >= cRank || CX2PVPEmblem::PVPRANK_RANK_SSS < cRank )
						cRank = 0;

					pPvpEmblemData = g_pMain->GetPVPEmblem()->GetPVPEmblemData( static_cast<CX2PVPEmblem::PVP_RANK>( cRank ) );


					/// NULL 체크 추가
					if ( static_cast<char>( CX2PVPEmblem::PVPRANK_RANK_ARRANGE ) < cRank && NULL != pPvpEmblemData )
					{
						m_columnForOthersRank[i].m_pStaticPvpEmblem->GetPicture( 0 )->SetShow( true );
						m_columnForOthersRank[i].m_pStaticPvpEmblem->GetPicture( 0 )->SetTex( pPvpEmblemData->m_TextureName.c_str(),
							pPvpEmblemData->m_TextureKey.c_str() );
					}
					else
						m_columnForOthersRank[i].m_pStaticPvpEmblem->GetPicture( 0 )->SetShow( false );

					m_columnForOthersRank[i].m_pStaticWin->SetString( 0, GET_REPLACED_STRING( ( STR_ID_3736, "i", kPvpRankingInfo.m_iWin ) ) );
					// 랭킹점수
					m_columnForOthersRank[i].m_pStaticLose->SetString( 0, GET_REPLACED_STRING( ( STR_ID_2632, "i", kPvpRankingInfo.m_iRPoint ) ) );

					SetShowOthersInfoByType( (m_columnForOthersRank + i), m_nowRankingInfoType, true );
					i++;
				}

				// 유저 자신의 랭킹 정보
				BOOST_TEST_FOREACH( KPvpRankingInfo&, kMyPvpRankingInfo, kEvent.m_vecMyPvpRanking )
				{
					m_columnForMyRank.m_pStaticRanking->GetString( 0 )->fontIndex	=	XUF_DODUM_15_BOLD;
					if ( kMyPvpRankingInfo.m_iRank <= _CONST_UIRANKING_INFO_::g_iLimitRankingToDisplay )
						m_columnForMyRank.m_pStaticRanking->SetString( 0, GET_REPLACED_STRING( ( STR_ID_2632, "i", kMyPvpRankingInfo.m_iRank ) ) );
					else
						m_columnForMyRank.m_pStaticRanking->SetString( 0, GET_STRING( STR_ID_3734 ) );

					SetCharIcon( m_columnForMyRank.m_pStaticClass, kMyPvpRankingInfo.m_cUnitClass,
						kMyPvpRankingInfo.m_ucLevel );		// 캐릭터 Icon

					m_columnForMyRank.m_pStaticNickName->SetString( 0, kMyPvpRankingInfo.m_wstrNickName.c_str() );	// 닉네임

					CX2PVPEmblem::PVPEmblemData*	pPvpEmblemData	= NULL;

					const char cRank = kMyPvpRankingInfo.m_cRank;
					pPvpEmblemData = g_pMain->GetPVPEmblem()->GetPVPEmblemData( static_cast<CX2PVPEmblem::PVP_RANK>( cRank ) );

					if ( static_cast<char>( CX2PVPEmblem::PVPRANK_RANK_ARRANGE ) < cRank )
					{
						m_columnForMyRank.m_pStaticPvpEmblem->GetPicture( 0 )->SetShow( true );
						m_columnForMyRank.m_pStaticPvpEmblem->GetPicture( 0 )->SetTex( pPvpEmblemData->m_TextureName.c_str(),
							pPvpEmblemData->m_TextureKey.c_str() );
					}
					else
						m_columnForMyRank.m_pStaticPvpEmblem->GetPicture( 0 )->SetShow( false );

					m_columnForMyRank.m_pStaticWin->SetString( 0, GET_REPLACED_STRING( ( STR_ID_3736, "i", kMyPvpRankingInfo.m_iWin ) ) );
					m_columnForMyRank.m_pStaticLose->SetString( 0, GET_REPLACED_STRING( ( STR_ID_2632, "i", kMyPvpRankingInfo.m_iRPoint ) ) );
					SetShowMyInfoByType( m_nowRankingInfoType, true );
				}
			}
			break;

			//{{ 허상형 : [2009/8/21] //	신학기 이벤트 응답 메시지 처리
			//}} 허상형 : [2009/8/21] //	신학기 이벤트 응답 메시지 처리
		}
	}

	return true;
}

void CX2UIRankingInfo::UpdatePageUI() const
{
	if ( m_pDlgRankingInfo != NULL )
	{
		CKTDGUIStatic* pStatic = (CKTDGUIStatic*)( m_pDlgRankingInfo->GetControl( L"Static_PageNumber" ) );

		if ( pStatic != NULL )
		{
			wstringstream wstrstm;
			wstrstm << m_uiNowPage
				<< L"/" << m_uiMaxPage;
			pStatic->SetString( 0, wstrstm.str().c_str() );
		}
	}
}

std::wstring CX2UIRankingInfo::ConvertSecToPlayTimeStr( UINT uiSec ) const
{
	int iSec	= 0;
	int iMin	= 0;
	int iHour	= 0;

	//{{ 2009. 10. 1  최육사	랭킹 표시 - 롤백
	//uiSec		= static_cast< UINT >( MAX_PLAY_TIME - uiSec );		// 남은 시간이 아닌 소요시간으로 수정
	//}}

	iHour		= uiSec / 3600;
	iMin		= ( uiSec % 3600 ) / 60;
	iSec		= uiSec - ( iHour * 3600 + iMin * 60 );

	std::wstringstream strStream;

	if ( iHour >= 0 && iHour < 10 )
		strStream << 0;
	strStream	<< iHour << " : ";

	if ( iMin >= 0 && iMin < 10 )
		strStream << 0;
	strStream	<< iMin << " : " ;

	if ( iSec >= 0 && iSec < 10 )
		strStream << 0;
	strStream	<< iSec << std::endl;

	return std::wstring( strStream.str().c_str() );
}

std::wstring CX2UIRankingInfo::ConvertTimeToRegisterDateStr( __int64 tRegisterDate )
{
	CTime cTime( tRegisterDate );

	std::wstring wstrDate;

	wstrDate	=	GET_STRING( STR_ID_3735 );
	wstrDate	+=	L"\n";
	wstrDate	+=	static_cast< const wchar_t * >( cTime.Format( _T("%Y-%m-%d") ) );
	wstrDate	+=	L"\n";
	wstrDate	+=	static_cast< const wchar_t * >( cTime.Format( _T("%p %H:%M") ) );

	return wstrDate;
}

void		CX2UIRankingInfo::DestoryUIRankingInfo()
{
	for ( int i = 0; i < _CONST_UIRANKING_INFO_::g_iNumRankingPerPage; i++ )
	{
		m_columnForOthersRank[i].m_pStaticRanking		= NULL;
		m_columnForOthersRank[i].m_pStaticClass			= NULL;
		m_columnForOthersRank[i].m_pStaticNickName		= NULL;
		m_columnForOthersRank[i].m_pStaticClearStage	= NULL;
		m_columnForOthersRank[i].m_pStaticPlayTime		= NULL;
		m_columnForOthersRank[i].m_pButtonRegistredDate	= NULL;
		m_columnForOthersRank[i].m_pStaticPvpEmblem		= NULL;
		m_columnForOthersRank[i].m_pStaticWin			= NULL;
		m_columnForOthersRank[i].m_pStaticLose			= NULL;
		m_columnForOthersRank[i].m_pStaticExp			= NULL;
	}

	m_columnForMyRank.m_pStaticRanking			= NULL;
	m_columnForMyRank.m_pStaticClass			= NULL;
	m_columnForMyRank.m_pStaticNickName			= NULL;
	m_columnForMyRank.m_pStaticClearStage		= NULL;
	m_columnForMyRank.m_pStaticPlayTime			= NULL;
	m_columnForMyRank.m_pButtonRegistredDate	= NULL;
	m_columnForMyRank.m_pStaticPvpEmblem		= NULL;
	m_columnForMyRank.m_pStaticWin				= NULL;
	m_columnForMyRank.m_pStaticLose				= NULL;
	m_columnForMyRank.m_pStaticExp				= NULL;

}

void		CX2UIRankingInfo::SetCharIcon( CKTDGUIStatic* const pStaticClass, char cUnitClass, int iLevel )
{

	wstring fileName;
	wstring pieceName;
	if( true == CX2Data::GetCharacterImageName( fileName, pieceName, (CX2Unit::UNIT_CLASS) cUnitClass, CX2Data::CIT_50by50 ) )
	{
		pStaticClass->GetPicture(0)->SetTex( fileName.c_str(), pieceName.c_str() );
	}
	else
	{
		ASSERT( !"NO" );
	}


	if( true == CX2Data::GetCharacterImageName( fileName, pieceName, (CX2Unit::UNIT_CLASS) cUnitClass, CX2Data::CIT_ClassInitial ) )
	{
		if( true == pieceName.empty() )
		{
			pStaticClass->GetPicture(1)->SetShow( false );
		}
		else
		{
			pStaticClass->GetPicture(1)->SetTex( fileName.c_str(), pieceName.c_str() );
			pStaticClass->GetPicture(1)->SetShow( true );
		}
	}
	else
	{
		ASSERT( !"NO" );
	}




	int iLv10	= 0;
	int iLv1	= 0;

	iLv10		= iLevel / 10;
	iLv1		= iLevel % 10;

	SetLevelIcon( pStaticClass->GetPicture( 2 ), iLv10 );
	SetLevelIcon( pStaticClass->GetPicture( 3 ), iLv1 );

	pStaticClass->GetPicture( 2 )->SetShow( true );
	pStaticClass->GetPicture( 3 )->SetShow( true );
}

void CX2UIRankingInfo::SetLevelIcon( CKTDGUIControl::CPictureData* const pPictureData, int num )
{
	switch ( num )
	{
	case 0:
		pPictureData->SetTex( L"DLG_UI_Common_Texture02.tga", L"zero" );
		break;

	case 1:
		pPictureData->SetTex( L"DLG_UI_Common_Texture02.tga", L"one" );
		break;

	case 2:
		pPictureData->SetTex( L"DLG_UI_Common_Texture02.tga", L"two" );
		break;

	case 3:
		pPictureData->SetTex( L"DLG_UI_Common_Texture02.tga", L"three" );
		break;

	case 4:
		pPictureData->SetTex( L"DLG_UI_Common_Texture02.tga", L"four" );
		break;

	case 5:
		pPictureData->SetTex( L"DLG_UI_Common_Texture02.tga", L"five" );
		break;

	case 6:
		pPictureData->SetTex( L"DLG_UI_Common_Texture02.tga", L"six" );
		break;

	case 7:
		pPictureData->SetTex( L"DLG_UI_Common_Texture02.tga", L"seven" );
		break;

	case 8:
		pPictureData->SetTex( L"DLG_UI_Common_Texture02.tga", L"eight" );
		break;

	case 9:
		pPictureData->SetTex( L"DLG_UI_Common_Texture02.tga", L"nine" );
		break;

	default:
		ASSERT( false );
		break;
	}
}

void CX2UIRankingInfo::SetRankNumImage( CKTDGUIStatic* const pStaticRankNumImg, int iRankNum )
{
	int iRank100	= 0;
	int iRank10		= 0;
	int iRank1		= 0;

	iRank100		= iRankNum	/	100;
	iRank10			= ( iRankNum	%	100 ) / 10;
	iRank1			= iRankNum	% 10;

	for ( int i = 0; i < _CONST_UIRANKING_INFO_::g_iRankLimit; i ++ )
		pStaticRankNumImg->GetPicture( i )->SetShow( false );

	if ( iRank100 > 0 )			// 세자리 숫자면
	{
		SetNumToPictureForRank( pStaticRankNumImg->GetPicture( 0 ), iRank100 );
		SetNumToPictureForRank( pStaticRankNumImg->GetPicture( 1 ), iRank10 );
		SetNumToPictureForRank( pStaticRankNumImg->GetPicture( 2 ), iRank1 );

	}
	else if ( iRank10 > 0 )		// 두자리 숫자면
	{
		SetNumToPictureForRank( pStaticRankNumImg->GetPicture( 1 ), iRank10 );
		SetNumToPictureForRank( pStaticRankNumImg->GetPicture( 2 ), iRank1 );
	}
	else if ( iRank1 > 0 )		// 한자리 숫자면
	{
		// 한자리 일때는 가운데에 출력
		SetNumToPictureForRank( pStaticRankNumImg->GetPicture( 1 ), iRank1 );
	}
	else
	{
		// 순위가 0인 경우는 올 수 없음
		ASSERT( false );
	}

}

void CX2UIRankingInfo::SetNumToPictureForRank( CKTDGUIControl::CPictureData* const pPictureData, int num )
{
	std::wstring		wstrTexName;

	switch ( num )
	{
	case 0:
		wstrTexName		=	L"PC_0.dds";
		break;

	case 1:
		wstrTexName		=	L"PC_1.dds";
		break;

	case 2:
		wstrTexName		=	L"PC_2.dds";
		break;

	case 3:
		wstrTexName		=	L"PC_3.dds";
		break;

	case 4:
		wstrTexName		=	L"PC_4.dds";
		break;

	case 5:
		wstrTexName		=	L"PC_5.dds";
		break;

	case 6:
		wstrTexName		=	L"PC_6.dds";
		break;

	case 7:
		wstrTexName		=	L"PC_7.dds";
		break;

	case 8:
		wstrTexName		=	L"PC_8.dds";
		break;

	case 9:
		wstrTexName		=	L"PC_9.dds";
		break;

	default:
		ASSERT( false );
		break;
	}

	pPictureData->SetTex( wstrTexName.c_str() );

	pPictureData->SetSizeX( pPictureData->pTexture->pTexture->GetWidth() * 0.8f );
	pPictureData->SetSizeY( pPictureData->pTexture->pTexture->GetHeight() * 0.8f );

	pPictureData->SetShow( true );
}



HRESULT		CX2UIRankingInfo::OnFrameMove( double fTime, float fElapsedTime )
{
	return S_OK;
}

bool		CX2UIRankingInfo::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	// 앞에 모달 다이얼로그가 있으면 메시지를 처리하지 않는다
	if( true == g_pKTDXApp->GetDGManager()->GetDialogManager()->CheckFrontModalDlg( m_pDlgRankingInfo ) )
		return false;

	bool bFlag	= false;

	switch( uMsg )
	{
		// 마우스 메시지들에 대해
	case WM_MOUSEMOVE:
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
	case WM_LBUTTONDBLCLK:
	case WM_MBUTTONDBLCLK:
	case WM_RBUTTONDBLCLK:
		//case WM_MOUSEWHEEL:
		{
			if ( false == m_pDlgRankingInfo->GetIsMouseOver())
			{
				return false;
			}
			bFlag = true;
		}
		break;
	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
		{
			if ( false == m_pDlgRankingInfo->GetIsMouseOver())
			{
				return false;
			}
			g_pData->GetUIManager()->SetUILayerIndex(CX2UIManager::UI_MENU_RANKING_INFO, true);
			bFlag = true;
		}
		break;
	default:
		break;
	}

	return bFlag;
}

bool		CX2UIRankingInfo::UICustomEventProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{

	switch ( wParam )
	{
	case URICM_EXIT:
		{
			SetShow( false );

			if ( g_pTFieldGame != NULL )
			{
				CX2TFieldNpc *pJoinNpc = g_pTFieldGame->GetFieldNPC( g_pTFieldGame->GetJoinNpcIndex() );
				if ( pJoinNpc != NULL )
				{
					if ( pJoinNpc->GetNpcShop() != NULL )
						pJoinNpc->GetNpcShop()->SetKeyEvent();
				}
			}

			return true;
		}
		break;

	case URICM_SELECT_HENIR_TAP:
		{
			SetShowControlsByType( m_nowRankingInfoType, false );
			SetShowControlsByType( RT_DAY_RANKING, true );

			m_nowRankingInfoType	=	RT_DAY_RANKING;
			m_uiNowPage				=	1;

			Handler_EGS_GET_RANKING_INFO_REQ();
			return true;
		}
		break;

	case URICM_SELECT_PVP_TAP:
		{
			SetShowControlsByType( m_nowRankingInfoType, false );
			SetShowControlsByType( RT_PVP_RANKING, true );

			m_nowRankingInfoType	=	RT_PVP_RANKING;
			m_uiNowPage				=	1;

			Handler_EGS_GET_RANKING_INFO_REQ();

			return true;
		}
		break;


	case URICM_SELECT_LEVEL_TAP:
		{
			SetShowControlsByType( m_nowRankingInfoType, false );
			SetShowControlsByType( RT_LEVEL_RANKING, true );

			m_nowRankingInfoType	=	RT_LEVEL_RANKING;
			m_uiNowPage				=	1;

			Handler_EGS_GET_RANKING_INFO_REQ();
			return true;
		}
		break;

	case URICM_SELECT_DAY_RADIO_BUTTON:
		{
			m_nowRankingInfoType	=	RT_DAY_RANKING;
			m_uiNowPage				=	1;

			Handler_EGS_GET_RANKING_INFO_REQ();

			return true;
		}
		break;

	case URICM_SELECT_WEEK_RADIO_BUTTON:
		{
			m_nowRankingInfoType	=	RT_WEEK_RANKING;
			m_uiNowPage				=	1;

			Handler_EGS_GET_RANKING_INFO_REQ();

			return true;
		}
		break;

	case URICM_SELECT_MONTH_RADIO_BUTTON:
		{
			m_nowRankingInfoType	=	RT_MONTH_RANKING;
			m_uiNowPage				=	1;

			Handler_EGS_GET_RANKING_INFO_REQ();

			return true;
		}
		break;

	case URICM_PUSH_PREV_PAGE:
		{
			if ( m_uiNowPage > 1 )
			{
				m_uiNowPage--;

				Handler_EGS_GET_RANKING_INFO_REQ();
			}

			return true;
		}
		break;

	case URICM_PUSH_NEXT_PAGE:
		{
			if ( m_uiNowPage < m_uiMaxPage )
			{
				m_uiNowPage++;

				Handler_EGS_GET_RANKING_INFO_REQ();
			}

			return true;
		}
		break;

		//{{ 허상형 : [2009/8/21] //	신학기 이벤트 메시지
		//}} 허상형 : [2009/8/21] //	신학기 이벤트 메시지

	default:
		break;
	}

	return false;
}

bool		CX2UIRankingInfo::UIServerEventProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch ( wParam )
	{
	case EGS_GET_RANKING_INFO_ACK:
		{
			return Handler_EGS_GET_RANKING_INFO_ACK( hWnd, uMsg, wParam, lParam );
		}
		break;


	default:
		break;
	}

	return false;
}

bool		CX2UIRankingInfo::GetShow() const
{
	return m_bShow;
}

void		CX2UIRankingInfo::SetShow( bool bShow )
{
	m_bShow = bShow;

	if ( bShow )
	{
		UpdatePageUI();
		InitMyColumn();
		InitOthersColumn();
		InitUIRankingInfo();
		Handler_EGS_GET_RANKING_INFO_REQ();
		g_pData->GetUIManager()->UIOpened( CX2UIManager::UI_MENU_RANKING_INFO );
	}
	else
	{
		g_pData->GetUIManager()->UIClosed( CX2UIManager::UI_MENU_RANKING_INFO );
	}

	m_pDlgRankingInfo->SetShowEnable( m_bShow, m_bShow );
}

D3DXVECTOR2	CX2UIRankingInfo::GetDLGSize() const
{
	return m_vSizeDlg;		// 임시
}

void		CX2UIRankingInfo::SetPosition( D3DXVECTOR2 vec )
{
	m_vPosDlg = vec;
	m_pDlgRankingInfo->SetPos( m_vPosDlg );
}

void		CX2UIRankingInfo::SetLayer( X2_DIALOG_LAYER layer )
{
	if( m_pDlgRankingInfo != NULL)
	{
		g_pKTDXApp->GetDGManager()->GetDialogManager()->ChangeLayer( m_pDlgRankingInfo, layer);
		g_pKTDXApp->GetDGManager()->GetDialogManager()->ChangeSequence( m_pDlgRankingInfo, false );
	}
}




//{{ 허상형 : [2009/8/20] //	신학기 이벤트 컨트롤 초기화
//}} 허상형 : [2009/8/20] //	신학기 이벤트 컨트롤 초기화

//{{ kimhc // 2009-07-09 // 헤니르의 시공에 추가되는 Ranking UI 작업
