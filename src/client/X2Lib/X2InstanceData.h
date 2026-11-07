#pragma once


#if X2_DEV_TOOLS
	//class CX2ItemCheatPopUp;
	#include "./X2ItemCheatPopUp.h"
#endif // ITEM_CHEAT_POPUP_TEST

#include "ScriptID_Code.h"
#define IF_EVENT_ENABLED(id)		if( g_pInstanceData != NULL && g_pInstanceData->IsEnableCode( id ) == true )
#define ELSE_IF_EVENT_ENABLED(id)	else if( g_pInstanceData != NULL && g_pInstanceData->IsEnableCode( id ) == true )
#define ELSE						else


class CX2MiniMapUI;
class CX2InstanceData
{

public:

	CX2InstanceData(void);
	virtual ~CX2InstanceData(void);

	void OnFrameMove( double fTime, float fElapsedTime );


#if X2_DEV_TOOLS
	CX2ItemCheatPopUp& GetItemCheatPopup() { return m_ItemCheatPopup; }
#endif // ITEM_CHEAT_POPUP_TEST


	void ResetMiniMapUI();
	CX2MiniMapUI* GetMiniMapUI(){ return m_pMiniMapUI; }
	void DeleteMiniMapUI(){ SAFE_DELETE(m_pMiniMapUI); }


	bool GetIsDungeonLounge() const { return m_bIsDungeonLounge; }
	void SetIsDungeonLounge(bool val) { m_bIsDungeonLounge = val; }

	bool GetLoadedDungeonLoungeUnits() const { return m_bLoadedDungeonLoungeUnits; }
	void SetLoadedDungeonLoungeUnits(bool val) { m_bLoadedDungeonLoungeUnits = val; }

	CX2World::WORLD_ID GetDungeonLoungeWorldID() const { return m_eDungeonLoungeWorldID; }
	void SetDungeonLoungeWorldID(CX2World::WORLD_ID val) { m_eDungeonLoungeWorldID = val; }


	const std::vector< KFieldUserInfo >& GetDungeonLoungeUnitInfo() const { return m_vecDungeonLoungeUnitInfo; }
	void AddDungeonLoungeUnitInfo( const KFieldUserInfo& val ) { m_vecDungeonLoungeUnitInfo.push_back(val); }
	void ClearDungeonLoungeUnitInfo() { m_vecDungeonLoungeUnitInfo.resize(0); }


	D3DXVECTOR3 GetLastDungeonGatePos() const { return m_vLastDungeonGatePos; }
	void SetLastDungeonGatePos(D3DXVECTOR3 val) { m_vLastDungeonGatePos = val; }

	int GetLastDungeonGatePosLineIndex() const { return m_iLastDungeonGatePosLineIndex; }
	void SetLastDungeonGatePosLineIndex(int val) { m_iLastDungeonGatePosLineIndex = val; }




	const wstring& GetUserID() const { return m_UserID; }
	void SetUserID(const wstring& val) { m_UserID = val; }

	const wstring& GetUserSessionID() const { return m_UserSessionID; }
	void SetUserSessionID(const wstring& val) { m_UserSessionID = val; }

	const wstring& GetUserPassword() const { return m_UserPassword; }
	void SetUserPassword(const wstring& val) { m_UserPassword = val; }


	//{{ kimhc // 2010-06-24 // OTP 작업
	const wstring& GetNxHWID() const { return m_wstrNxHWID; }
	void SetNxHWID(const wstring& val) { m_wstrNxHWID = val; }
	//}} kimhc // 2010-06-24 // OTP 작업

	//{{ kimhc // 2009-12-18 // 서버군 추가 작업으로 사용하지 않음
	//}} kimhc // 2009-12-18 // 서버군 추가 작업으로 사용하지 않음

	int GetConnectChannelServerID() const { return m_iConnectChannelServerID; }
	void SetConnectChannelServerID(int val) { m_iConnectChannelServerID = val; }

	bool GetFirstJoinVillage() { return m_bFirstJoinVillage; }
	void SetFirstJoinVillage(bool val) { m_bFirstJoinVillage = val; }

	wstring GetConnectChannelServerName();
#ifdef EXTEND_SERVER_GROUP_MASK
	wstring GetChannelServerName( int ChannelID, int ServerID);
#else
	wstring GetChannelServerName( int ChannelID, SERVER_GROUP_ID ServerID);
#endif // EXTEND_SERVER_GROUP_MASK
	wstring GetChannelServerName( int ChannelID );		// 09.04.20 태완

#if X2_DEV_TOOLS
	bool GetFrameScale() const { return m_bFrameScale; }
	void SetFrameScale(bool val) { m_bFrameScale = val; }
#endif

	//{{ kimhc // 실시간 엘소드 중 실시간 ED 획득 관련
	int		GetInstanceED() const { return m_iED; }
	void	SetInstanceED( int iED ) { m_iED = iED; }
	//}} kimhc // 실시간 엘소드 중 실시간 ED 획득 관련

	bool	GetShowCSPandSlotBDate() const { return m_bShowCSPandSlotBDate; }
	void	SetShowCSPandSlotBDate( bool bShow ) { m_bShowCSPandSlotBDate = bShow; }

	bool	IsChangeChannel() { return m_bIsChangeChannel; }
	void	SetChangeChannel(bool val) { m_bIsChangeChannel = val; }
	bool	IsIDVerified(){ return m_bIDVerified; }
	void	SetIDVerified(bool val){ m_bIDVerified = val; }

	//{{ 2010.12.03 조효진	캐시샵 토큰 스트링 적용
	std::wstring	GetPurchaseToken(){ return m_wstrPurchaseTok; }
	void			SetPurchaseToken(std::wstring val){ m_wstrPurchaseTok = val; }
	//}}


	const wstring		GetServerGroupName();

	// 2009-07-29 // kimhc // 헤니르 시공의 랭킹이 등록되었다는 알림을 확인하는 bool값
	bool	IsRegisteredRank() const { return m_bRegisteredRank; }
	void	SetRegisteredRank( bool bRegistered ) { m_bRegisteredRank = bRegistered; }

	//{{ kimhc // 자동결제에서 최근 열리거나 닫힌 웹페이지가 무엇인지를 판단 하는 변수 및 함수 추가
	//}} kimhc // 자동결제에서 최근 열리거나 닫힌 웹페이지가 무엇인지를 판단 하는 변수 및 함수 추가

	//{{ kimhc // 2009-12-15 // 서버군 추가 작업에 따른 채널 다이얼로그 분리


#ifdef EXTEND_SERVER_GROUP_MASK
	void				SetServerGroupID( int iServerGroupID );
	int					GetServerGroupID() const { return m_iServerGroupID; }
	void				SetServerGroupIDCashShop( int iServerGroupID ) { m_iServerGroupIDCashShop = iServerGroupID; }
	int					GetServerGroupIDCashShop() const { return m_iServerGroupIDCashShop; }
//#ifndef CHANNEL_CONGESTION_SCOPE_NO_CHEAT
	void				SetChannelDistributionByServerGroup( OUT float* pfChCongestionScopeTemp, IN const float* pfChCongestionScope, IN int iChannelIndex );
//#endif // CHANNEL_CONGESTION_SCOPE_NO_CHEAT
	void				SetServerGroupIDScript( int iServerGroupID ) { m_iServerGroupIDScript = iServerGroupID; }
	int					GetServerGroupIDScript() const { return m_iServerGroupIDScript; }
#else
	void				SetServerGroupID( SERVER_GROUP_ID eServerGroupID );
	SERVER_GROUP_ID		GetServerGroupID() const { return m_eServerGroupID; }
	void				SetServerGroupIDCashShop( SERVER_GROUP_ID eServerGroupID ) { m_eServerGroupIDCashShop = eServerGroupID; }
	SERVER_GROUP_ID		GetServerGroupIDCashShop() const { return m_eServerGroupIDCashShop; }
	void				SetChannelDistributionByServerGroup( OUT float* pfChCongestionScopeTemp, IN const float* pfChCongestionScope, IN int iChannelIndex );

	void				SetServerGroupIDScript( SERVER_GROUP_ID eServerGroupID ) { m_eServerGroupIDScript = eServerGroupID; }
	SERVER_GROUP_ID		GetServerGroupIDScript() const { return m_eServerGroupIDScript; }
#endif // EXTEND_SERVER_GROUP_MASK
	//}}  kimhc // 2009-12-15 // 서버군 추가 작업에 따른 채널 다이얼로그 분리

	// 채널 서버 인증 이후 넘어온 채널링 코드를 저장해둔다
	void				SetChannelingCode( IN int iChannelingCode_ ) { m_iChannelingCode = iChannelingCode_; }
	int					GetChannelingCode() const { return m_iChannelingCode; }
	bool			IsAeriaChanneling() const { return ((m_iChannelingCode == 6) || (m_iChannelingCode == 7)); }

	CKTDXTimer& GetTimerPacketTiming() { return m_TimerPacketTiming; }
	bool IsElapsedPacketTiming( float fTime = 1.f ) { return m_TimerPacketTiming.elapsed() > fTime; }

	void SetSendHackUser(bool bVal) { m_bSendedHackUser = bVal; }
	bool GetSendHackUser() { return m_bSendedHackUser; }
	void SetSendHackMail(bool bVal) { m_bSendedHackMail = bVal; }
	bool GetSendHackMail() { return m_bSendedHackMail; }

	void SetSendHackMail_AutoHack(bool bVal) { m_bSendedHackMailAutoHack = bVal; }
	bool GetSendHackMail_AutoHack() { return m_bSendedHackMailAutoHack; }




    void    SetHackList_MainThread( const std::vector<KHackingToolInfo>& vecHackList )
    {
        m_vecHackList_MainThread = vecHackList;
        ReSetHackList_MainThread();
    }
    void    ReSetHackList_MainThread()
    {
        std::vector<KHackingToolInfo> vecTemp = m_vecHackList_MainThread;
        {
            CSLock  lock(m_csHackList);
            m_vecHackList.swap( vecTemp );
            m_bChangeHacklist = true;
        }
    }
    bool    GetChangedHackList_ThreadSafe( OUT std::vector<KHackingToolInfo>& vecHackList )
    {
        bool    bChangeHackList = false;
        vecHackList.resize( 0 );
        if ( m_bChangeHacklist == true )
        {
            CSLock  lock(m_csHackList);
            bChangeHackList = m_bChangeHacklist;
            vecHackList.swap( m_vecHackList );
            m_bChangeHacklist = false;
        }
        if ( bChangeHackList == false )
        {
            vecHackList.resize( 0 );
        }
        return bChangeHackList;
    }
    bool    VerifyChangeHackList_ThreadSafe()
    {
        CSLock  lock(m_csHackList);
        return  m_bChangeHacklist.Verify();
    }
	int     GetHackListSize_MainThread() { return (int)m_vecHackList_MainThread.size(); }



//{{ kimhc // 2010.3.26 // 무한 스킬 버그 수정
	// STATE_CHANGE_GAME_START_NOT를 받았는가?
	void SetReceiveGameStartNotPacket( bool bValue ) { m_bReceiveGameStartNotPacket = bValue; }
	bool GetReceiveGameStartNotPacket() const { return m_bReceiveGameStartNotPacket; }
	// STATE_CHANGE_GAME_START_REQ를 전송했는가?
	void SetSendGameStartReqPacket( bool bValue ) { m_bSendGameStartReqPacket = bValue; }
	bool GetSendGameStartReqPacket() const { return m_bSendGameStartReqPacket; }
//}} kimhc // 2010.3.26 // 무한 스킬 버그 수정

	//{{ kimhc // 2010.4.9 // 비밀던전 작업(일일퀘스트 시스템)
	void SetPreCheckTimeForDailyQuest( __time64_t t ) { m_t64PreCheckedTimeForDailyQuest = t; }
	__time64_t GetPreCheckTimeForDailyQuest() const { return m_t64PreCheckedTimeForDailyQuest; }
	//}} kimhc // 2010.4.9 // 비밀던전 작업(일일퀘스트 시스템)

	bool GetVerifyNpcHp() { return m_bVerifyNpcHp; }
	void SetVerifyNpcHp(bool bVal) { m_bVerifyNpcHp = bVal; }

	float GetVerifyGageManagerTimer() { return m_fRemainedTimeByForceQuitGame; }
	void  SetRemainedTimeByForceQuitGame(float fVal) { m_fRemainedTimeByForceQuitGame = fVal; }


	void SetSendedModuleList(bool bVal) { m_bSendedLoadedModuleList = bVal; }
	bool GetSendedModuleList() { return m_bSendedLoadedModuleList; }

	wstring GetAgencyShopExpirationDate() { return m_wstrAgencyShopExpirationDate; }
	bool GetIsPShopOpen() { return m_bIsPShopOpen; }

	SEnum::AGENCY_SHOP_TYPE GetAgencyShopType() { return m_eAgencyShopType; }

	void SetPShopAgencyInfo( IN const bool bIsPShopOpen_, IN const wstring wstrExpirationDate_, IN const SEnum::AGENCY_SHOP_TYPE eAgencyShopType_)
	{
		m_bIsPShopOpen					= bIsPShopOpen_;
		m_wstrAgencyShopExpirationDate	= wstrExpirationDate_;
		m_eAgencyShopType				= eAgencyShopType_;
	}

	bool IsActiveAgencyShop()
	{
		if( m_wstrAgencyShopExpirationDate.empty() )
			return false;

		CTime	tCurrentTime	= CTime::GetCurrentTime();
		CTime	cAgencyTime;
		KncUtil::ConvertStringToCTime( m_wstrAgencyShopExpirationDate, cAgencyTime );
		if( tCurrentTime < cAgencyTime )
		{
			return true;
		}
		return false;
	}

	void SetMachineId(std::string &strMachineId) { m_strMachineId = strMachineId; }
	std::string &GetMachineId() { return m_strMachineId; }



	bool GetMoneyMonitoring() { return m_bMoneyMonitoring; }
	void SetMoneyMonitoring(bool bVal) { m_bMoneyMonitoring = bVal; }



	CKTDXCheckElapsedTime&		GetTimerForSendingPlayStatus() { return m_TimerForSendingPlayStatus; }


	void SetMaxLevel( int iMaxLevel ) { m_iMaxLevel = iMaxLevel; }
	int GetMaxLevel() { return m_iMaxLevel; }

	void SetCodeEvent( std::map< int, bool >& mapEnableCodeEnum ) { m_mapEnableCodeEnum = mapEnableCodeEnum; }
	bool IsEnableCode( IN int iEventID )
	{
		std::map< int, bool >::iterator mitEnableCodeEnum = m_mapEnableCodeEnum.find( iEventID );

		if( mitEnableCodeEnum == m_mapEnableCodeEnum.end() )
			return false;

		return mitEnableCodeEnum->second;
	}







	wstring GetChannelButtonNameByChannelName( wstring wstrChannelName );
	void AddUserUnitDataInServer( SEnum::SERVER_GROUP_ID eServerGroupID, int iNowUserUnitManyInServer, int iMaxUserUnitManyInServer );
	void ClearUserUnitDataInServer ();
	int	GetMaxUserUnitManyInServer ( SEnum::SERVER_GROUP_ID eServerGroupID );
	int	GetNowUserUnitManyInServer ( SEnum::SERVER_GROUP_ID eServerGroupID );



//{{ robobeg : 2013-12-19
    // 여러 곳에 중복되어 있는 코드를 한 곳으로 모읍니다.
    bool OpenScriptServerGroupFile();			// 이전에 플레이 했던 서버군 읽기
    bool SaveScriptServerGroupFile();
//}} robobeg : 2013-12-19


#ifdef FIX_REFORM_ENTRY_POINT_10TH		//	kimjh,  캐릭터 리스트 못받으면 재접속 유도
	bool IsConnectedChannel ( int iChannelID_ );
	void SetConnectedChannelID ( int iChannelID_ );
	void ResetConnectedChannelID ();
#endif // FIX_REFORM_ENTRY_POINT_10TH	//	kimjh,  캐릭터 리스트 못받으면 재접속 유도







private:
	CX2MiniMapUI*	m_pMiniMapUI;


	//{{ kimhc // 실시간 엘소드 중 실시간 ED 획득 관련
	int		m_iED;		// 던전 이탈시 ED에 패널티가 적용된 메뉴 효과를 보여주기위한 용도의 변수
	//}} kimhc // 실시간 엘소드 중 실시간 ED 획득 관련

	bool	m_bShowCSPandSlotBDate;		// kimhc // 캐릭터 선택 후 그노시스의 축복과 스킬슬롯B의 사용 알림

	bool m_bIsDungeonLounge;
	bool m_bLoadedDungeonLoungeUnits;
	std::vector< KFieldUserInfo > m_vecDungeonLoungeUnitInfo;

	CX2World::WORLD_ID m_eDungeonLoungeWorldID;
	D3DXVECTOR3 m_vLastDungeonGatePos;				// 던전 라운지에서 던전 게이트로 빠져나갈 때 사용할 위치
	int			m_iLastDungeonGatePosLineIndex;		// 던전 라운지에서 던전 게이트로 빠져나갈 때 사용할 위치의 line index

	wstring m_UserID;				// 로그인시 id/password
	wstring m_UserSessionID;				// 로그인시 Aeria Games 에서 넘겨준 Sesion ID
	wstring m_UserPassword;

	bool	m_bIDVerified;

	std::wstring m_wstrPurchaseTok;


	int		m_iConnectChannelServerID;		// 현재 연결된 게임서버, 즉 채널의 ID

	bool	m_bFirstJoinVillage;					// 캐릭터 선택창에서 진입한 유저



#if X2_DEV_TOOLS

	CX2ItemCheatPopUp m_ItemCheatPopup;

#endif // ITEM_CHEAT_POPUP_TEST





#if X2_DEV_TOOLS
	bool	m_bFrameScale;
#endif

	//{{ kimhc // 자동결제에서 최근 열리거나 닫힌 웹페이지가 무엇인지를 판단 하는 변수 및 함수 추가
	//}} kimhc // 자동결제에서 최근 열리거나 닫힌 웹페이지가 무엇인지를 판단 하는 변수 및 함수 추가

	bool	m_bIsChangeChannel;

	bool	m_bRegisteredRank;

public:
	std::vector< KChannelInfo > m_vecChannelInfo;

	std::map< int, KChannelBonusInfo > m_mapChannelBonusInfo;



	//{{ kimhc // 2009-12-15 // 서버군 추가 작업에 따른 채널 다이얼로그 분리
#ifdef EXTEND_SERVER_GROUP_MASK
	int					m_iServerGroupID;
	int					m_iServerGroupIDCashShop;	// 현재 가지고 있는 캐시아이템 리스트를 받아온 서버군 ID
	int					m_iServerGroupIDScript;
#else
	SERVER_GROUP_ID		m_eServerGroupID;
	SERVER_GROUP_ID		m_eServerGroupIDCashShop;	// 현재 가지고 있는 캐시아이템 리스트를 받아온 서버군 ID
	SERVER_GROUP_ID		m_eServerGroupIDScript;
#endif // EXTEND_SERVER_GROUP_MASK
	//}}  kimhc // 2009-12-15 // 서버군 추가 작업에 따른 채널 다이얼로그 분리

	std::map< int, KServerGroupInfo > m_mapServerGroupInfo;

#ifdef EXTEND_SERVER_GROUP_MASK
	std::map<int, std::wstring>					m_mapServerGroupName;
#else
	std::map<SERVER_GROUP_ID, std::wstring>		m_mapServerGroupName;
#endif // EXTEND_SERVER_GROUP_MASK

	CKTDXTimer m_TimerPacketTiming;		// 특정 패킷들이 일정시간 간격으로 연달아 보내졌을 때 오류가 생기는 것을 막기위해서 이 타이머를 참조해서 패킷을 보내거나 해당 기능이 동작할지 말지를 결정한다

	bool m_bSendedHackUser;
	bool m_bSendedHackMail;
	bool m_bSendedHackMailAutoHack;
	bool m_bFirstSelect;		// kimhc // 로그인 후 처음 캐릭터 선택인가? // PC 방 인벤토리 디폴트 탭선택 작업

private:
    std::vector<KHackingToolInfo>   m_vecHackList_MainThread;
    MemberCriticalSection           m_csHackList;
	std::vector<KHackingToolInfo>   m_vecHackList;
	KProtectedType<bool>            m_bChangeHacklist;
public:

	//{{ kimhc // 2010-03-23 // 무한 스킬 오류 수정
	bool m_bReceiveGameStartNotPacket;		// STATE_CHANGE_GAME_START_NOT를 받았는가?
	bool m_bSendGameStartReqPacket;			// STATE_CHANGE_GAME_START_REQ를 전송했는가?
	//}} kimhc // 2010-03-23 // 무한 스킬 오류 수정

	//{{ kimhc // 2010.4.9 // 비밀던전 작업(일일퀘스트 시스템)
	__time64_t		m_t64PreCheckedTimeForDailyQuest;		// 이전에 체크한 시간 (일일퀘스트용)
	//}} kimhc // 2010.4.9 // 비밀던전 작업(일일퀘스트 시스템)

	bool m_bVerifyNpcHp;

	//{{ kimhc // 2010-06-24 // OTP 작업
	wstring m_wstrNxHWID;		// 하드웨어 ID
	//}} kimhc // 2010-06-24 // OTP 작업

	KProtectedType<float>	m_fRemainedTimeByForceQuitGame;


	KProtectedType<bool>	m_bSendedLoadedModuleList;

	bool						m_bIsPShopOpen;
	wstring						m_wstrAgencyShopExpirationDate;

	SEnum::AGENCY_SHOP_TYPE		m_eAgencyShopType;		/// 대리 상점 타입 ( Free, ED, Cash )

	std::string					m_strMachineId;

	KProtectedType<bool>		m_bMoneyMonitoring;



	CKTDXCheckElapsedTime		m_TimerForSendingPlayStatus;	/// 자신의 HP, MP 등 플레이 데이타를 서버에 전송하는 간격

	// Key, pair < 현재 유닛 수, 최대 생성 가능한 유닛 수 >
	map < SEnum::SERVER_GROUP_ID, pair< int, int > > m_mapUserUnitDataInServer;



	int							m_iMaxLevel;

	std::map< int, bool >		m_mapEnableCodeEnum;










#ifdef FIX_REFORM_ENTRY_POINT_10TH		//	kimjh,  캐릭터 리스트 못받으면 재접속 유도
		vector<int>							m_vecTryConnectChannelID;
#endif // FIX_REFORM_ENTRY_POINT_10TH	//	kimjh,  캐릭터 리스트 못받으면 재접속 유도

	int							m_iChannelingCode;






};
