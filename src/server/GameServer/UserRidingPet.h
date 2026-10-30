#pragma once
#include "Event.h"
#include "CommonPacket.h"

//{{ 2010. 01. 15  최육사	boost allocator
	#include <ObjectPool/BoostAllocator.h>
//}}

#define KUserRidingPetPtr KUserRidingPet*
#undef KUserRidingPetPtr
SmartPointer( KUserRidingPet );

//////////////////////////////////////////////////////////////////////////
BOOST_ALOC_CLASS( KUserRidingPet )
	//////////////////////////////////////////////////////////////////////////
{
public:
	KUserRidingPet( IN KRidingPetInfo kInfo, IN const bool bCreated );
	~KUserRidingPet(void);

	void	SetData( IN const KRidingPetInfo& kInfo, IN const bool bChanged = true )
	{
		m_kInfo		= kInfo;
		SetChanged( bChanged );
	}

	void	GetData( OUT KRidingPetInfo& kInfo ) const{	kInfo = m_kInfo;	}

	UidType	GetUID( void ) const{	return m_kInfo.m_iRidingPetUID;	}
	USHORT	GetID( void ) const{	return m_kInfo.m_usRindingPetID;	}
	bool	EnableCheck( void ) const;
	__int64	GetLastUnSummonDate( void ) const{	return m_kInfo.m_iLastUnSummonDate;	}

	void	SetChanged( IN const bool bChanged ){	m_bChanged = bChanged;	}
	bool	GetChanged( void ) const{	return m_bChanged;	}
	bool	IsExpirationRidingPet( void ) const;
	bool	CheckRidingPetStamina( IN const float fStamina );

private:
	bool			m_bChanged;
	KRidingPetInfo	m_kInfo;
};
