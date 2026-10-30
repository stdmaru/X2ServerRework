#pragma once

#include <KNCSingleton.h>
#include <ToString.h>


//{{ 2012. 02. 07	최육사	배틀필드 시스템


class KGSAutoPartyManager
{
	DeclareSingleton( KGSAutoPartyManager );
	DeclDump;
	DeclToStringW;

	struct SAutoPartyBonusInfo
	{
        bool	m_bIsAutoPartyBonus;

		SAutoPartyBonusInfo()
		{
			m_bIsAutoPartyBonus = false;
		}
	};

public:
    KGSAutoPartyManager();
    virtual ~KGSAutoPartyManager();

	void	UpdateAutoPartyBonusInfo( IN const int iDungeonMode, IN const int iDungeonID, IN const bool bActivatedBonus );
	__int64	GetUpdateTick() const				{ return m_iUpdateTick; }
	bool	IsAutoPartyBonus( IN const int iDungeonMode, IN const int iDungeonID ) const;

protected:
	typedef std::pair< int, int >	KAutoPartyBonusInfoKey;	// first : DungeonMode, Second : DungeonID

	std::map< KAutoPartyBonusInfoKey, SAutoPartyBonusInfo >	m_mapAutoPartyBonusInfo;
	__int64													m_iUpdateTick;
};

DefSingletonInline( KGSAutoPartyManager );


//}}
