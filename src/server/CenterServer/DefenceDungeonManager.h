#pragma once


//{{ 2013. 04. 09	최육사	어둠의 문 개편


class KDefenceDungeonManager
{
public:
    KDefenceDungeonManager();
    virtual ~KDefenceDungeonManager();

	void	Clear();

	void	StartGame();
	void	EndGame();
	void	OnCloseRoom();

	// get
	bool	IsEnterDefenceDungeon() const						{ return m_bEnterDefenceDungeon; }
	void	SetOpenDefenceDungeon( IN const bool bVal )			{ m_bOpenDefenceDungeon = bVal; }
	int		GetDefenceDungeonPhase() const						{ return m_iDefenceDungeonPhase; }

	// process
	//{{ 2013. 04. 29	최육사	어둠의 문 입장 로직 변경
	bool	DecisionEnterDefenceDungeon( IN const int iDungeonID, IN const int iNpcLevel, IN const std::map< UidType, int >& mapUnitLevelList, IN const float fMultipleRate, IN bool b100PerEventOn = false );
	//}}
	bool	IncreaseDefenceDungeonPhase( IN const int iNpcID );

protected:
	bool					m_bOpenDefenceDungeon;		// 어둠의 문이 열렸는지 여부
	bool					m_bEnterDefenceDungeon;		// 어둠의 문 던전에 진입결정이 났는지 여부
	int						m_iDefenceDungeonPhase;
};


//}}
