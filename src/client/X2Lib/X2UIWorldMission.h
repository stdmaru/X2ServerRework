//{{ 허상형 : [2011/3/22/] //	월드 미션
#pragma once


const int REWARD_SUCCESS_ITEM_ID	= 60001797;	//	성공시 지급되는 아이템
const int REWARD_FAIL_ITEM_ID		= 60001798;	//	실패시 시간에따라 조각으로 지급되는 아이템

class CX2UIWorldMission
{
public:
	enum	WORLD_MISSION_UI_MSG
	{
		WMUM_REWARD_CLOSE	= 46200,
	};

	CX2UIWorldMission();
	~CX2UIWorldMission();

	//bool SetShowRewardDlg( bool bEnable );
	void SetShowTimeDlg( bool bEnable );
	//bool SetShowEXPBonusDlg( bool bEnable );

	bool UICustomEventProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

//  	void SetReservedRewardItem( bool bEnable, int iItemID = 0, int iQuantity = 0 );
//  	bool GetReservedRewardItem()	{ return m_bReservedReward; }

	void SetForceUpdateCrystalUI(){m_bForceUpdate = true;}

	void UpdateTimeDlg( int iTime );

	void UpdateNumOfCrystalsProtected( int iNumOfCrystalsProtected_ = 0, int iNumOfMaxCrystalsProtected_ = 1 );


	void SetShowCrystalPictureData( CKTDGUIStatic* pStaticTime_, int iIndexOfColor_, bool bShow_ );

	CKTDGUIDialogType			GetTimeDlg()	{ return m_pDlgTime; }
	//CKTDGUIDialogType			GetEXPBonusDlg()	{ return m_pDlgEXPBonus; }

	bool GetEnableTimeDlg() const { return m_bEnableTimeDlg; }
	void SetEnableTimeDlg( bool bEnableTimeDlg_ ) { m_bEnableTimeDlg = bEnableTimeDlg_; }

	bool GetIsFlickerDanger() const { return m_bIsFlickerDanger; }		/// 어둠의 문 월드 미션 UI의 Danger 마크가 깜빡이는 여부 반환
	void SetIsFlickerDanger(bool val) { m_bIsFlickerDanger = val; }		/// 어둠의 문 월드 미션 UI의 Danger 마크가 깜빡이는 여부 설정

private:
	//CKTDGUIDialogType			m_pDlgReward;
	CKTDGUIDialogType			m_pDlgTime;
	//CKTDGUIDialogType			m_pDlgEXPBonus;

// 	bool						m_bReservedReward;
// 	int							m_iRewardItemID;
// 	int							m_iRewardQuantity;
	bool						m_bForceUpdate;
	bool						m_bEnableTimeDlg;

	bool						m_bIsFlickerDanger;			/// 어둠의 문 월드 미션 UI의 Danger 마크가 깜빡이는 여부

};

//}} 허상형 : [2011/3/22/] //	월드 미션
