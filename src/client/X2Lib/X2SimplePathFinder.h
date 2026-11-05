#pragma once


class CX2BattleFieldNpcAi;

class CX2SimplePathFinder
{
public:
	enum MOVESTATE
	{
		MS_WAIT,
		MS_LEFT,
		MS_RIGHT,
		MS_JUMP,
		MS_DOWN,
		MS_FORCE_LEFT,
		MS_FORCE_RIGHT,

		MS_JUMP_LEFT,
		MS_JUMP_RIGHT,
		MS_DOWN_LEFT,
		MS_DOWN_RIGHT,

		MS_FLY_LEFT_FRONT,		/// LEFT를 보는데 앞으로 날아갈때(?)
		MS_FLY_LEFT_FRONT_UP,
		MS_FLY_LEFT_FRONT_DOWN,
		MS_FLY_LEFT_BACK,		/// LEFT를 보는데 뒤로 날아갈때(?)
		MS_FLY_LEFT_BACK_UP,
		MS_FLY_LEFT_BACK_DOWN,
		MS_FLY_RIGHT_FRONT,
		MS_FLY_RIGHT_FRONT_UP,
		MS_FLY_RIGHT_FRONT_DOWN,
		MS_FLY_RIGHT_BACK,
		MS_FLY_RIGHT_BACK_UP,
		MS_FLY_RIGHT_BACK_DOWN,

		MS_FLY_UP,
		MS_FLY_DOWN,
	};

public:
	~CX2SimplePathFinder();
	CX2SimplePathFinder();

	void FollowTargetPathInBattleField( const CX2BattleFieldNpcAi* pBattleFieldNpcAi_, IN const CKTDGLineMap* pLineMap_, OUT bool& bTargetOnRight_ );
	void FollowTargetPath( /*float fElapsedTime,*/ const D3DXVECTOR3& vCurrPos, const int iCurrLineDataIndex,
		const D3DXVECTOR3& vFinalDestPos, const int iFinalDestLineDataIndex,
		const float fMaxJumpUp, const float fMaxJumpRight,
		IN CKTDGLineMap* pLineMap, const float fDestArriveGap, const float fDestLeaveGap, const bool bFootOnLine,
		const bool bStayOnCurrLineGroup, OUT bool& bTargetOnRight, const float fLineEndDetectRange
		, bool bIfCannotFindMoveStateDoWait = false
		);

	void EscapeTargetPath( /*float fElapsedTime,*/ const D3DXVECTOR3& vCurrPos, const int iCurrLineDataIndex,
		const D3DXVECTOR3& vFinalDestPos, const int iFinalDestLineDataIndex,
		const float fMaxJumpUp, const float fMaxJumpRight,
		IN CKTDGLineMap* pLineMap, const float fDestArriveGap, /*const float fDestLeaveGap,*/ const bool bFootOnLine,
		/*bool bStayOnCurrLineGroup,*/ OUT bool& bTargetOnRight, const bool bIsRight, const float fLineEndDetectRange );

	MOVESTATE GetMoveState() { return m_MoveState; }
	D3DXVECTOR3& GetDestPos() { return m_vDestPos; }
	int GetDestLineIndex() { return m_iDestLineDataIndex; }

	void FollowTargetPathFly( /*float fElapsedTime,*/ const D3DXVECTOR3& vCurrPos, const int iCurrLineDataIndex,
		const D3DXVECTOR3& vFinalDestPos, const int iFinalDestLineDataIndex,
		const float fMaxJumpUp, const float fMaxJumpRight,
		const float fDestArriveHeight, const float fDestArriveArea,
		IN CKTDGLineMap* pLineMap, const float fDestArriveGap, /*const float fDestLeaveGap,*/ /*bool bFootOnLine,*/
		/*bool bStayOnCurrLineGroup,*/ bool& bTargetOnRight, const float fLineEndDetectRange );

	void SimpleFollowTargetPath( float fElapsedTime, D3DXVECTOR3& vCurrPos, const int iCurrLineDataIndex,
		const D3DXVECTOR3& vFinalDestPos, const int iFinalDestLineDataIndex,
		const float fMaxJumpUp, const float fMaxJumpRight,
		CKTDGLineMap* pLineMap, const float fDestArriveGap, const float fDestLeaveGap, bool bFootOnLine,
		bool bStayOnCurrLineGroup, bool& bTargetOnRight, const float fLineEndDetectRange );
	void SimpleFollowTargetPathFly( float fElapsedTime, D3DXVECTOR3& vCurrPos, const int iCurrLineDataIndex,
		const D3DXVECTOR3& vFinalDestPos, const int iFinalDestLineDataIndex,
		const float fMaxJumpUp, const float fMaxJumpRight,
		const float fDestArriveHeight, const float fDestArriveArea,
		CKTDGLineMap* pLineMap, const float fDestArriveGap, const float fDestLeaveGap, bool bFootOnLine,
		bool bStayOnCurrLineGroup, bool& bTargetOnRight, const float fLineEndDetectRange );

private:
	CKTDGLineMap::LineGroupID m_pPrevLineGroup;
	CKTDGLineMap::LineGroupID m_pCurrLineGroup;

	MOVESTATE m_MoveState;

	D3DXVECTOR3 m_vDestPos;
	int m_iDestLineDataIndex;

	bool m_bFlyTurn;
};
