#pragma once

#include <KncSmartPtr.h>
#include "Event.h"
#include "CommonPacket.h"
//{{ 2010. 01. 15  최육사	boost allocator
	#include <ObjectPool/BoostAllocator.h>
//}}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


#define KGSPartyPtr KGSParty*
#undef KGSPartyPtr
SmartPointer( KGSParty );

//////////////////////////////////////////////////////////////////////////
BOOST_ALOC_CLASS( KGSParty )
//////////////////////////////////////////////////////////////////////////
{
public:
	KGSParty( const KPartyInfo& kInfo );
    virtual ~KGSParty();

	// 정보 저장 함수
	void	SetInfo( IN const KPartyInfo& kInfo );
	void	AddMember( IN const KPartyUserInfo& kUserInfo );
	void	DeleteMember( IN UidType nCID );

	// 정보 획득 함수
	void	GetInfo( OUT KPartyInfo& kInfo );

	//{{ 2011. 06. 21	최육사	대전 개편
	char	GetPartyType()						{ return m_kPartyInfo.m_cPartyType; }
	//}}
	UidType GetPartyUID()						{ return m_kPartyInfo.m_iPartyUID; }
	const std::wstring& GetPartyName()			{ return m_kPartyInfo.m_wstrPartyName; }
	int		GetDungeonID()						{ return m_kPartyInfo.m_iDungeonID; }
	char	GetDungeonDif()						{ return m_kPartyInfo.m_DifficultyLevel; }
	char	GetDungeonMode()					{ return m_kPartyInfo.m_cDungeonMode; }
	//{{ 2012. 02. 28	최육사	배틀필드 시스템
	short	GetPartyFever() const 				{ return m_kPartyInfo.m_sPartyFever; }
	//}}

	bool	IsMember( IN UidType iUnitUID )		{ return ( m_mapMember.find( iUnitUID ) != m_mapMember.end() ); }
	int		GetMaxMember()						{ return static_cast<int>(m_kPartyInfo.m_MaxSlot); }
	int		GetNumMember()						{ return static_cast<int>(m_mapMember.size()); }

	UidType GetHostUID();

	void	GetListUserInfoList( OUT std::vector< KPartyListUserInfo >& vecUserInfo );

	float	GetPartyFeverExpRate();

	//{{ 2012. 02. 28	최육사	배틀필드 시스템
	void	SetPartyFever( IN const short sPartyFever )	{ m_kPartyInfo.m_sPartyFever = sPartyFever; }
	//}}

	// 파티 멤버 정보 갱신
	void	UpdatePartyUserInfo( IN const KPartyUserInfo& kInfo );

	//{{ 2011. 03. 21  김민성	던전 난이도 보정
	bool	GetCheckLowLevel()							{ return m_kPartyInfo.m_bCheckLowLevel; }
	bool	CheckPartyMinLevel( IN const int& iDungeonMinLevel );
	//}}

protected:
	KPartyInfo								m_kPartyInfo;
    std::map< UidType, KPartyUserInfo >		m_mapMember;
};


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
