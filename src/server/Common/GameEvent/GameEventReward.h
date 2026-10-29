#pragma once

#include "GameEvent.h"
#include "X2Data/XSLUnit.h"
#include "X2Data/XSLItem.h"
//{{ 2011. 12. 06	최육사	특정 서버군 이벤트 시스템
	#include "Enum/Enum.h"
//}}


class KGameEventReward : public KGameEvent
{
public:
	struct REWARD_DATA
	{
		float	m_fEXPRate;
		float	m_fVPRate;
		float	m_fEDRate;
		float	m_fRankingPointRate;

		int		m_iDungeonID;
		int		m_iUnitLevel;
		char	m_cUnitType;
		char    m_cUnitClass;
		int		m_iPartyNum;

		// 아이템 이벤트
		float	m_fEventTime;
		int		m_iEventReward;
		bool	m_bGiveOnce;
		bool	m_bDelEndEvent;
		bool	m_bPcBangEvent;
		bool	m_bDirectReward;
		bool	m_bAccountEvent;
		int		m_iDropCount;
		bool	m_bWithPlayPcBang;
		//{{ 2011. 12. 06	최육사	특정 서버군 이벤트 시스템
		int		m_iServerGroupID;
		//}}
		//{{ 2012. 06. 20	김민성       이벤트 경험치 중첩 안됨 조건 추가
		bool	m_bAccumulateEXP;
		//}}
		//{{ 2012. 12. 17	최육사	아라 파티 플레이 보너스 경험치
		//}}
		int		m_iUnitClassLevel;

		REWARD_DATA()
		{
			m_fEXPRate			= 0.f;
			m_fVPRate			= 0.f;
			m_fEDRate			= 0.f;
			m_fRankingPointRate	= 0.0f;
			m_iDungeonID		= 0;
			m_iUnitLevel		= 0;
			m_cUnitType			= CXSLItem::UT_NONE;
			m_cUnitClass		= CXSLItem::UC_NONE;
			m_iPartyNum			= 0;
			m_fEventTime		= 0.f;
			m_iEventReward		= 0;
			m_bGiveOnce			= false;
			m_bDelEndEvent		= false;
			m_bPcBangEvent		= false;
			m_bDirectReward		= false;
			m_bAccountEvent		= false;
			m_iDropCount		= 0;
			m_bWithPlayPcBang	= false;
			//{{ 2011. 12. 06	최육사	특정 서버군 이벤트 시스템
#ifdef EXTEND_SERVER_GROUP_MASK
			m_iServerGroupID	= -1;
#else
			m_iServerGroupID	= SEnum::SGI_INVALID;
#endif // EXTEND_SERVER_GROUP_MASK
			//}}
			//{{ 2012. 06. 20	김민성       이벤트 경험치 중첩 안됨 조건 추가
			m_bAccumulateEXP	= true; // 기본 중첩 허용
			//}}
			//{{ 2012. 12. 17	최육사	아라 파티 플레이 보너스 경험치
			//}}
			m_iUnitClassLevel	= -1;
		}
	};

public:
	KGameEventReward();
	virtual ~KGameEventReward();

	//{{ 2011. 12. 06	최육사	특정 서버군 이벤트 시스템
	void	SetEXPRate( IN const float fEXPRate );
	void	SetVPRate( IN const float fVPRate );
	void	SetEDRate( IN const float fEDRate );
	void	SetRankingPointRate( IN const float fRankingPointRate );
	void	SetDungeonID( IN const int iDungeonID );
	void	SetUnitLevel( IN const int iUnitLevel );
	void	SetUnitType( IN const char cUnitType );
	void	SetUnitClass( IN const char cUnitClass );
	void	SetPartyNum( IN const int iPartyNum );
	void	SetServerGroupID( IN const int iServerGroupID );
	void	Dump();
	//}}

	float	GetEXPRate() const;
	float	GetVPRate() const;
	float	GetEDRate() const;
	float	GetRankingPointRate( void ) const;
	int		GetDungeonID() const				{ return m_sData.m_iDungeonID; }
	int		GetUnitLevel() const				{ return m_sData.m_iUnitLevel; }
	char	GetUnitType() const					{ return m_sData.m_cUnitType; }
	char	GetUnitClass() const				{ return m_sData.m_cUnitClass; }
	int		GetPartyNum() const					{ return m_sData.m_iPartyNum; }
	//{{ 2011. 12. 06	최육사	특정 서버군 이벤트 시스템
	int		GetServerGroupID() const			{ return m_sData.m_iServerGroupID; }
	//}}

	//{{ 2012. 06. 20	김민성       이벤트 경험치 중첩 안됨 조건 추가
 	void		SetAccumulateEXP( IN bool bAccumulateEXP ) 		{ m_sData.m_bAccumulateEXP = bAccumulateEXP; }
 	bool		IsAccumulateEXP() const							{ return m_sData.m_bAccumulateEXP; }
	//}}
	//{{ 2012. 12. 17	최육사	아라 파티 플레이 보너스 경험치
	//}}

	void	SetPcBangEvent( IN const bool bPcBangEvent ){	m_sData.m_bPcBangEvent = bPcBangEvent;	}
	bool	IsPcBangEvent( void ) const{	return m_sData.m_bPcBangEvent;	}

protected:
    REWARD_DATA	m_sData;
};
