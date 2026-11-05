#pragma once

class CX2InformerQuest : public CX2Informer
{
	public:

		struct HouseQuestInfo
		{
			int m_HouseID;

			CKTDGParticleSystem::CParticleEventSequenceHandle  m_hSeqAccecptableQuestInfoBG;
			CKTDGParticleSystem::CParticleEventSequenceHandle  m_hSeqAccecptableQuestInfo;
			CKTDGParticleSystem::CParticleEventSequenceHandle  m_hSeqCompleteQuestInfoBG;
			CKTDGParticleSystem::CParticleEventSequenceHandle  m_hSeqCompleteQuestInfo;

			CKTDGParticleSystem::CParticleEventSequenceHandle  m_hSeqAccecptableEventQuestInfoBG;
			CKTDGParticleSystem::CParticleEventSequenceHandle  m_hSeqAccecptableEventQuestInfo;

			HouseQuestInfo();
			~HouseQuestInfo();
		};

	public:

		CX2InformerQuest(void);
		virtual ~CX2InformerQuest(void);

		HRESULT OnFrameMove( double fTime, float fElapsedTime );

		void Reset();

		void LoadHouseQuestInfo( int villageID );
		void UnLoadHouseQuestInfo();
		void CheckHouseQuestInfo(); //실시간으로 받을 퀘스트가 있거나 완료한 퀘스트가 있는지 확인해서 표현해준다.
		void UnCheckHouseQuestInfo();

		//{{ kimhc // 2010.02.09 // 일정시간마다 퀘스트가 업데이트 되는 시스템 구현
		void SetTimedEventQuest( bool bValue ) { m_bTimedEventQuest = bValue; }
		bool IsTimedEventQuest() const { return m_bTimedEventQuest; }
		//}} kimhc // 2010.02.09 // 일정시간마다 퀘스트가 업데이트 되는 시스템 구현

	protected:

		void SetNowInfo();
		bool IsNowInfoChange();
		void SetShow( bool bShow );

		int	 m_iNowInfoChangeQuestID;
		bool m_bIsEverySubQuestInQuestComplete;		/// 퀘스트내의 모든 서브퀘스트를 완료 했는지
		std::map< int, std::set<int> > m_mapNowClearSubQuestGroup; //<퀘스트아이디, 서브퀘스트 그룹 인덱스>
		set< int >	m_NowClearQuestID;
		vector< HouseQuestInfo* > m_vecHouseQuestInfo;

		//{{ kimhc // 2010.02.09 // 일정시간마다 퀘스트가 업데이트 되는 시스템 구현
		bool m_bTimedEventQuest;
		//}} kimhc // 2010.02.09 // 일정시간마다 퀘스트가 업데이트 되는 시스템 구현
};
