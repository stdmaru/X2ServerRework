#pragma once

class CX2PVPEmblem
{
	public:

		enum PVP_EMBLEM
		{
			PE_NONE			= -1,
			PE_RANK_E		= 0,
			PE_RANK_D		= 251,
			PE_RANK_C		= 551,
			PE_RANK_B		= 951,
			PE_RANK_A		= 1351,
			PE_RANK_S		= 1851,
			PE_RANK_SS		= 2351,
			PE_RANK_SSS		= 2951,
			PE_END			= 3300,
		};

		enum PVP_RANK
		{
			PVPRANK_NONE			= 0,
			PVPRANK_RANK_ARRANGE	= 1,
			PVPRANK_RANK_E			= 2,
			PVPRANK_RANK_D			= 3,
			PVPRANK_RANK_C			= 4,
			PVPRANK_RANK_B			= 5,
			PVPRANK_RANK_A			= 6,
			PVPRANK_RANK_S			= 7,
			PVPRANK_RANK_SS			= 8,
			PVPRANK_RANK_SSS		= 9,
//{{ robobeg : 2013-02-27
            PVPRANK_MAX,
//}} robobeg : 2013-02-27
		};

		struct PVPEmblemData
		{
			int m_EmblemID;
			wstring m_EmblemName;
			wstring m_TextureName;
			wstring m_TextureKey;
		};

	public:

		CX2PVPEmblem(void);
		~CX2PVPEmblem(void);

		PVPEmblemData* GetPVPEmblemData( IN PVP_RANK eEmblemID_ );
		void AddEmblemData_LUA( int emblemID, const char* pName, const char* pTextureName, const char* pTextureKey );
		PVPEmblemData* GetMyNowUnitPVPEmblem();

		CX2PVPEmblem::PVP_EMBLEM GetEmblemID( int iRating );
		CX2PVPEmblem::PVP_EMBLEM GetEmblemID( CX2PVPEmblem::PVP_RANK eRank );

	protected:

		bool OpenScript( const WCHAR* pFileName );

		map< PVP_RANK, PVPEmblemData* >	m_mapEmblemData;
		vector< CKTDXDeviceTexture* >		m_vecReadyTexture;
};
