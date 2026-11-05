#pragma once

class CX2PVPRoom : public CX2Room
{
	public:
		enum PVP_GAME_TYPE
		{
			PGT_TEAM = 0,
			PGT_TEAM_DEATH,
			PGT_SURVIVAL,
			PGT_TAG,
		};

		enum TEAM_DEATH_WIN_KILL_NUM_LIMIT
		{
			DTWKNL_MIN = 4,
			DTWKNL_MAX = 28,
		};

		enum SURVIVAL_DEATH_WIN_KILL_NUM_LIMIT
		{
			SDWKNL_MIN = 3,
			SDWKNL_MAX = 12,
		};

		enum TEAM_PLAY_TIME_LIMIT
		{
			TPTL_MIN = 200,
			TPTL_MAX = 400,
		};

		enum DEATH_MATCH_PLAY_TIME_LIMIT
		{
			DMPTL_MIN = 200,
			DMPTL_MAX = 1100,
		};








		struct PVPRoomSimpleInfo : public RoomSimpleInfo
		{
			PVP_GAME_TYPE				m_PVPGameType;
			int							m_WinKillNum;
			bool						m_bCanIntrude;
			CX2World::WORLD_ID			m_WorldID;
			bool						m_bItemMode;

			PVPRoomSimpleInfo()
			{
				m_PVPGameType	= PGT_TEAM;
				m_WinKillNum	= 0;
				m_bCanIntrude	= true;
				m_WorldID		= CX2World::WI_NONE;
				m_bItemMode		= true;
			}
			virtual void Set_KRoomInfo( const KRoomInfo* pData )
			{
				RoomSimpleInfo::Set_KRoomInfo( pData );

				m_PVPGameType		= (PVP_GAME_TYPE)pData->m_PVPGameType;
				m_WinKillNum		= pData->m_WinKillNum;
				m_bCanIntrude		= pData->m_bCanIntrude;
				m_WorldID			= (CX2World::WORLD_ID)pData->m_WorldID;
				m_bItemMode			= pData->m_bIsItemMode;
			}
		};


	public:
		CX2PVPRoom();
		virtual ~CX2PVPRoom(void);

		PVP_GAME_TYPE		GetPVPGameType()							{ return m_PVPGameType;			}
		int					GetPVPGameType_LUA()						{ return (int)m_PVPGameType; }

		int					GetWinKillNum()								{ return m_WinKillNum;			}
		float				GetPlayTimeLimit()							{ return m_fPlayTime;			}
		bool				GetCanIntrude()								{ return m_bCanIntrude;			}
		int					GetWorldID()								{ return m_WorldID;				}
		int					GetWorldIDForUI()							{ return m_WorldIDForUI;		}

		void				SetPVPGameType( PVP_GAME_TYPE gameType )	{ m_PVPGameType		= gameType;		}
		void				SetWinKillNum( int killNum )				{ m_WinKillNum		= killNum;		}
		void				SetPlayTime( float fPlayTime )				{ m_fPlayTime		= fPlayTime;	}
		void				SetCanIntrude( bool bCanIntrude )			{ m_bCanIntrude		= bCanIntrude;	}
		void				SetWorldID( int worldID )					{ m_WorldID			= worldID;		}
		void				SetWorldIDForUI( int worldID )				{ m_WorldIDForUI	= worldID;		}

		void				SetItemMode(bool bVal);
		bool				GetItemMode();

		virtual void		Set_KRoomInfo( const KRoomInfo& kRoomInfo_ );

		virtual bool		Handler_EGS_CHANGE_SLOT_OPEN_NOT( KEGS_CHANGE_SLOT_OPEN_NOT& kEGS_CHANGE_SLOT_OPEN_NOT );


		bool				Handler_EGS_CHANGE_MAP_REQ( int worldID );
		bool				Handler_EGS_CHANGE_MAP_ACK( KEGS_CHANGE_MAP_ACK& kEGS_CHANGE_MAP_ACK );
		bool				Handler_EGS_CHANGE_MAP_NOT( KEGS_CHANGE_MAP_NOT& kEGS_CHANGE_MAP_NOT );

		virtual int			GetClickSlot( int x, int y );
		bool				GetClickPetSlot( int x, int y );
		bool				IsRoomMaster( SlotData* pSlotData = NULL );

		//{{ 2009. 10. 22  최육사	대전방 버그수정
		static bool			IsValidWinKillNum( char cPvpGameType, char cWinKillNum );
		static bool			IsValidPvpPlayTime( char cPvpGameType, int iPlayTime );
		//}}

		static char			GetValidWinKillNum( char cPvpGameType );
		static int			GetValidPvpPlayTime( char cPvpGameType );


		bool IsOfficialPvp() { return m_bOfficialPvp; }
		void SetOfficialPvp(bool bVal) { m_bOfficialPvp = bVal; }

		virtual void ApplyRoomPacketData();


	protected:
		PVP_GAME_TYPE				m_PVPGameType;

		int							m_WinKillNum;
		float						m_fPlayTime;
		bool						m_bCanIntrude;
		int							m_WorldID;
		int							m_WorldIDForUI;

		bool							m_bItemMode;


		bool m_bOfficialPvp;
};
