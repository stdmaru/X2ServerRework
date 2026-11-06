#pragma once

#define D3DFVF_LINE_MAP_VERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE)

class CKTDGLineMap : public CKTDGObject
{
	public:

        struct  StartInfo
        {
            D3DXVECTOR3             m_vStartPos;
            bool                    m_bStartRight;
            int                     m_iStartLineIndex;

            StartInfo()
                : m_vStartPos(0,0,0)
                , m_bStartRight( true )
                , m_iStartLineIndex( -1 )
            {
            }
            StartInfo( const D3DXVECTOR3& vPos, bool bRight, int iLineIndex )
                : m_vStartPos(vPos)
                , m_bStartRight( bRight )
                , m_iStartLineIndex( iLineIndex )
            {
            }
        };//StartInfo

        typedef std::map<int,StartInfo>
                                    StartInfoMap;

		struct LINE_MAP_VERTEX
		{
			D3DXVECTOR3	pos;
			DWORD		color;
		};

		enum LINE_TYPE
		{
			LT_INVALID = -1,
			LT_NORMAL = 0,
			LT_WALL,
			LT_POTAL,
			LT_HOLE,
			LT_JUMP_UP_REL,
			LT_JUMP_UP_ABS,
			LT_BUNGEE,
//#ifdef MONSTER_ROAD
            LT_MONSTER_ROAD,
//#endif
//#ifdef UNIT_ROAD
            LT_UNIT_ROAD,
//#endif
//#ifdef OTHER_ROAD
			LT_OTHER_ROAD,
//#endif
			LT_CLIFF,
		};

		enum LINE_DUST_TYPE
		{
			LDT_DEFALT_DUST,
			LDT_WATER_SPLASH,
		};


		enum LINE_ANIM_TYPE
		{
			LAT_NONE,
			LAT_BACK_AND_FORTH,
			LAT_VIKING,
			LAT_CIRCLE,
			LAT_MOVE_IF_STEPPED_ON,
			//LAT_ROTATE_IF_STEPPED_ON,
			//LAT_SEESAW_IF_STEPPED_ON,
		};


		enum LINE_CAMERA_TYPE
		{
			LCT_INVALID = 0,
			LCT_ABSOULTE_POSITION,
			LCT_RELATIE_POSITION,
		};

		enum TRIGGER_MESH_ACTION_TYPE
		{
			TAT_MESH_NONE = 0,
			TAT_MESH_CHANGE_ANIM,
			TAT_MESH_CHANGE_SPEED,
		};
		enum TRIGGER_LINE_ACTION_TYPE
		{
			TAT_LINE_NONE = 0,
			TAT_LINE_CHANGE_ENABLE,
			TAT_LINE_CHANGE_SPEED,
			TAT_LINE_CHANGE_JUMPSPEED,
			TAT_LINE_CHANGE_CAMERA,
			TAT_LINE_CHANGE_CANPASS,
			TAT_LINE_CHANGE_TELEPORT,

		};
		enum TRIGGER_ETC_ACTION_TYPE
		{
			TAT_ETC_NONE = 0,
			TAT_ETC_PLAY_SOUND,
			TAT_ETC_DURATION_TIME,
			TAT_ETC_CAMERA,
			TAT_ETC_CAMERA_SHAKE,
			TAT_ETC_ACTIVE_LAYER,
		};

		enum LINEMAP_DIRECTION_OPTION
		{
			LDO_NONE	= 0,
			LDO_LEFT	= 1,
			LDO_RIGHT	= 2,
		};

		struct TRIGGER_DATA_MESH
		{
			TRIGGER_MESH_ACTION_TYPE	m_iMeshTriggerAction;
			wstring			m_wstrMeshName;
			float			m_fTriggerTime;
			wstring			m_wstrName;
			int				m_iPlayType;
			float			m_fAnimSpeed;
			bool			m_bDone;

			TRIGGER_DATA_MESH()
			{
				m_iMeshTriggerAction	= TAT_MESH_NONE;
				m_fTriggerTime			= 0.f;
				m_wstrName				= L"";
				m_iPlayType				= 1;
				m_wstrMeshName			= L"";
				m_fAnimSpeed			= 1.f;
				m_bDone					= false;
			}
		};
		struct TRIGGER_DATA_LINE
		{
			TRIGGER_LINE_ACTION_TYPE	m_iLineTriggerAction;
			int				m_iTriggerLineIndex;
			float			m_fTriggerTime;
			bool			m_bEnable;
			float			m_fSpeed;
			D3DXVECTOR3		m_vJumpSpeed;
			int				m_iCamera;
			bool			m_bDone;

			TRIGGER_DATA_LINE()
			{
				m_iLineTriggerAction	= TAT_LINE_NONE;
				m_iTriggerLineIndex		= -1;
				m_fTriggerTime			= 0.f;
				m_bEnable				= true;
				m_fSpeed				= 0.f;
				m_vJumpSpeed			= D3DXVECTOR3(0.f, 0.f, 0.f);
				m_iCamera				= -1;
				m_bDone					= false;
			}
		};
		struct TRIGGER_DATA_ETC
		{
			TRIGGER_ETC_ACTION_TYPE	m_iEtcTriggerAction;
			wstring			m_wstrActionName;
			float			m_fTriggerTime;
			int				m_iDummyValue01;
			float			m_fDummyValue01;
			bool			m_bDone;

			TRIGGER_DATA_ETC()
			{
				m_iEtcTriggerAction = TAT_ETC_NONE;
				m_wstrActionName	= L"";
				m_fTriggerTime		= 0.f;
				m_iDummyValue01		= -1;
				m_fDummyValue01		= 0.f;
				m_bDone					= false;
			}
		};
		struct TRIGGER_DATA
		{
			float	m_fElapsedTime;

			std::vector<TRIGGER_DATA_MESH>	m_vecTriggerMesh;
			std::vector<TRIGGER_DATA_LINE>	m_vecTriggerLine;
			std::vector<TRIGGER_DATA_ETC>	m_vecTriggerEtc;

			TRIGGER_DATA()
			{
				m_fElapsedTime = 0.f;

				m_vecTriggerMesh.clear();
				m_vecTriggerLine.clear();
				m_vecTriggerEtc.clear();
			}
		};
		struct TRIGGER_SEGMENT
		{
			int		m_iTriggerId;
			int		m_iTriggerDataIndex;
			bool	m_bLoop;
			std::vector<TRIGGER_DATA>	m_vecTriggerData;

			TRIGGER_SEGMENT()
			{
				m_iTriggerId = -1;
				m_iTriggerDataIndex = 0;
				m_bLoop = false;

				m_vecTriggerData.clear();
			}
		};
		struct LINEMAP_TRIGGER
		{
			std::vector<int>				m_vecNowTrigger;
			std::vector<TRIGGER_SEGMENT>	m_vecTriggerSeg;
		};

		struct TriggerLess
		{
			bool operator() (const TRIGGER_SEGMENT& a, const TRIGGER_SEGMENT& b)
			{
				return a.m_iTriggerId < b.m_iTriggerId;
			}
		};

		struct LineAnimData;
		typedef std::map< int, LineAnimData* > LineAnimDataMap;					// line anim index, line anim data

        struct LineGroupID
        {
            int         m_iStartLineIndex;
            int         m_iEndLineIndex;

            LineGroupID()
                : m_iStartLineIndex(-1)
                , m_iEndLineIndex(-1)
            {
            }//LineGroupID()

			void		Init()
			{
				m_iStartLineIndex = -1;
				m_iEndLineIndex = -1;
			}

            bool        IsValid() const
            {
                return m_iStartLineIndex >= 0 && m_iEndLineIndex >= 0;
            }
            bool operator == ( const LineGroupID& right_ ) const
            {
                return  m_iStartLineIndex == right_.m_iStartLineIndex
                    && m_iEndLineIndex == right_.m_iEndLineIndex;
            }
            bool operator != ( const LineGroupID& right_ ) const
            {
                return !( operator == ( right_ ) );
            }
            bool operator < ( const LineGroupID& right_ ) const
            {
                return  m_iStartLineIndex < right_.m_iStartLineIndex
                    || ( m_iStartLineIndex == right_.m_iStartLineIndex
                    && m_iEndLineIndex < right_.m_iEndLineIndex );
            }//
        };//struct LineGroupID

		struct LineData
		{
			int					lineIndex;		// line 번호

			LINE_TYPE			lineType;		//선의 종류
			bool				bEnable;		//선이 존재하는가

			LINE_DUST_TYPE		eDustType;		//뛰어다닐 때 생기는 먼지 파티클 종류
			int					potalGroup;		//이 그룹의 포탈끼리 이동 가능
			float				fStopFactor;	//마찰계수
			bool				bCanDown;		//이 선에서 내려갈 수 있는가

			int					beforeLine;
			int					nextLine;

			D3DXVECTOR3			startPos;
			D3DXVECTOR3			endPos;
            D3DXVECTOR3         m_vPosMin;
            D3DXVECTOR3         m_vPosMax;
			D3DXVECTOR3			dirVector;
			D3DXVECTOR3			dirDegree;
			D3DXVECTOR3			dirRadian;
			float				fLength;

			int					lineSet;
			float				m_fSpeed;
			D3DXVECTOR3			m_vJumpSpeed;

			int					m_iTeleportLineIndex;

			bool				m_bEnableWallJump;

			bool				m_bMonsterAllowed;		// 몬스터가 이 라인맵을 인식하고 밟을 수 있는지
			bool				m_bCanPass;				// jump up, jump down 해서 이 라인맵을 통과 할 수 있는지

			int					m_iCameraIndex;
			bool				m_bOtherRoadUp;
			LineAnimData*		m_pLineAnimData;


//#ifdef LINEMAP_SLOW_WIND_TEST
//			D3DXVECTOR2			m_vWindSpeed;
//			float				m_fWindRange;	// m_vWindSpeed가 적용되는 높이제한
//#endif LINEMAP_SLOW_WIND_TEST
//

//#ifdef LINEMAP_FAST_WIND_TEST
//			float m_fUpsideWindAccelaration;
//			float m_fUpsideWindRange;			// m_fUpsideWindAccelaration가 적용되는 높이 제한
//#endif LINEMAP_FAST_WIND_TEST





			float m_fCliffHeight;

//{{ kimhc // 2010.7.6 // 몬스터가 생성한 라인맵
			// 라인맵 스크립트에 의해서 생성된 것인지 아닌지를 구분 (false면 라인맵 스크립트에 의해서 생성된 것)
			bool				m_bTempLineMap;
//}} kimhc // 2010.7.6 // 몬스터가 생성한 라인맵

			bool	m_bUnderWater;		// 수중라인맵여부
			float	m_fWaterHeight;		// 수중높이(월드상의 절대Y위치)
			float	m_fBuoyancy;		// 부력값
			float	m_fResistance;		// 저항값
			float	m_fAnimSpeed;		// 속도값
			float	m_fVarianceOxyen;	// 산소변동량
			D3DXCOLOR m_cLineColor;		// 라인에 따른 유닛칼라
			bool	m_bForceChangeColor; // 해당 라인맵에 위치할시 강제로 유닛색상 변경
			int			m_iTriggerId;
			bool		m_bFootOnLine;


			LINEMAP_DIRECTION_OPTION	m_eDirection;
			float						m_fKeyStopTime;

			LINE_TYPE		m_eLineTypeAfterClearStage; // 지정한 스테이지 클리어 이 후 변경할 라인 타입
			int				m_iChangeAfterClearStage;	// 클리어 이후 라인타입 변경 할 스테이지
			int				m_iChangeAfterClearSubStage;// 크리어 이후 라인타입 변경 할 서브 스테이지


            LineGroupID             m_lineGroupID;

			bool			m_bIsPortalWhenActiveRaid;	// 레이드 활성화 됐을 때 포탈로 변경되야 하는 라인맵 여부

			LineData()
			{
				Init();
			}

			void Init()
			{
				lineIndex		= -1;
				lineType		= LT_NORMAL;
				bEnable			= true;

				potalGroup		= -1;
				fStopFactor		= 1.0f;
				bCanDown		= true;
				eDustType		= LDT_DEFALT_DUST;

				startPos		= D3DXVECTOR3(0,0,0);
				endPos			= D3DXVECTOR3(0,0,0);
                m_vPosMin       = D3DXVECTOR3(0,0,0);
                m_vPosMax       = D3DXVECTOR3(0,0,0);
				dirVector		= D3DXVECTOR3(0,0,0);
				dirDegree		= D3DXVECTOR3(0,0,0);
				dirRadian		= D3DXVECTOR3(0,0,0);
				fLength			= 0.0f;

				beforeLine		= -1;
				nextLine		= -1;

				lineSet			= -1;
				m_fSpeed		= 0.0f;
				m_vJumpSpeed	= D3DXVECTOR3(0,0,0);

				m_iTeleportLineIndex = -1;

				m_bEnableWallJump = false;

				m_bMonsterAllowed	= true;
				m_bCanPass			= true;

				m_iCameraIndex		= -1;

				m_pLineAnimData		= NULL;

				m_bOtherRoadUp		= false;


//#ifdef LINEMAP_SLOW_WIND_TEST
//				m_vWindSpeed		= D3DXVECTOR2( 0, 0 );
//				m_fWindRange		= 0.f;
//#endif LINEMAP_SLOW_WIND_TEST


//#ifdef LINEMAP_FAST_WIND_TEST
//				m_fUpsideWindAccelaration	= 0.f;
//				m_fUpsideWindRange			= 0.f;
//#endif LINEMAP_FAST_WIND_TEST






				m_fCliffHeight = 0;

//{{ kimhc // 2010.7.6 // 몬스터가 생성한 라인맵
				m_bTempLineMap	= false;
//}} kimhc // 2010.7.6 // 몬스터가 생성한 라인맵

				m_bUnderWater = false;
				m_fWaterHeight	= 0.f;
				m_fBuoyancy	= 0.f;
				m_fResistance = 0.f;
				m_fAnimSpeed = 1.f;
				m_fVarianceOxyen = 0.f;
				m_bForceChangeColor = false;

				m_iTriggerId = -1;
				m_bFootOnLine = true;

				m_eDirection = LDO_NONE;
				m_fKeyStopTime = 0.f;

				m_eLineTypeAfterClearStage = LT_INVALID;
				m_iChangeAfterClearStage	= -1;
				m_iChangeAfterClearSubStage = -1;


            	m_lineGroupID.Init();

			}





		}; // LineData




		struct LineAnimData
		{
			int							m_iLineAnimIndex;
			bool						m_bEnabled;
			LINE_ANIM_TYPE				m_eAnimType;
			bool						m_bMoveIfSteppedOn;
			D3DXVECTOR3					m_vPivotPosition;
			D3DXVECTOR3					m_vChildPivotPosition;
			D3DXVECTOR3					m_vChildPivotPositionDisplace;		// child pivot position - old child pivot position

			CKTDGLineMap::LineData* 	m_pLineData;


			std::vector< int >	m_vecForwardStepOnLineIndex;		// 이 벡터안에 있는 라인을 밟으면 animdata를 가지고 있는 라인이 순방향 으로 움직이게 되는 라인의 인덱스
			std::vector< int >	m_vecBackwardStepOnLineIndex;		// 이 벡터안에 있는 라인을 밟으면 animdata를 가지고 있는 라인이 역방향 으로 움직이게 되는 라인의 인덱스

			std::vector<LineAnimData*>	m_vecpChildAnimData;
			int							m_iParentAnimDataIndex;		// lua script 에서 data parsing 할 때만 사용한다.

		public:
			LineAnimData( LINE_ANIM_TYPE eAnimType );
			virtual ~LineAnimData() = default;
			virtual void OnFrameMove( double fTime, float fElapsedTime );
		};



		struct LineAnimBackAndForth : LineAnimData
		{
			D3DXVECTOR3			m_vMovingDirection;
			float				m_fMovingSpeed;
			float				m_fRange;

			float				m_fCurrentPositionValue;

			LineAnimBackAndForth();
			virtual void OnFrameMove( double fTime, float fElapsedTime );
		};


		struct LineAnimViking : LineAnimData
		{
			D3DXVECTOR3		m_vRotationAxis;
			D3DXVECTOR3		m_vEquilibriumPosition;		// 진자가 평형상태에서 움직이지 않고 있을 때의 위치, rotation Center를 중심으로 상대위치
			float			m_fPeriod;					// 진자운동 한번 왕복하는데 걸린 시간, 즉, 주기
			float			m_fMaxRotationRadian;

			float			m_fCurrentTime;

			LineAnimViking();
			virtual void OnFrameMove( double fTime, float fElapsedTime );
			void CalcLinePosition( float fCurrentTime );
		};



		struct LineAnimCircle : LineAnimData
		{
			D3DXVECTOR3		m_vRotationAxis;
			D3DXVECTOR3		m_vPositionVector;			// 원의 중심에서 라인의 중심위치를 가리키는 벡터
			float 			m_fRotationSpeed;

			float			m_fCurrentRotationRadian;

			LineAnimCircle();
			virtual void OnFrameMove( double fTime, float fElapsedTime );
			void CalcLinePosition( float fCurrentRotationRadian );
		};


		struct LineAnimMoveIfSteppedOn : LineAnimData
		{
			D3DXVECTOR3			m_vMovingDirection;
			float 				m_fAccelerationWhenSteppedOn;
			float				m_fMaxMovingRange;					// 0이면 제한이 없다
			float				m_fMaxMovingSpeed;					// 0이면 제한이 없다
			float				m_fBackwardSpeed;					// 라인위에 캐릭터가 없을 때 라인이 원위치로 돌아가는 속도

			bool				m_bSteppedOnForward;
			bool				m_bSteppedOnBackward;
			float				m_fCurrentMovingSpeed;
			float				m_fCurrentPostionValue;


			LineAnimMoveIfSteppedOn();
			virtual void OnFrameMove( double fTime, float fElapsedTime );
		};






		//struct LineAnimRotateIfSteppedOn : LineAnimData
		//{
		//	D3DXVECTOR3		m_vRotationAxis;
		//	D3DXVECTOR3		m_vRotationCenter;
		//
		//	float			m_fLeftWingLength;
		//	float			m_fRightWingLength;

		//	float			m_fMaxRotationRadian;
		//	float			m_fMaxRotationSpeed;
		//	float			m_fRotationAcceleration;
		//	bool			m_bRotateBackToOriginalWhenNotSteppedOn;
		//	float			m_fOriginalRotationRadian;

		//
		//	bool			m_bSteppedOnLeft;
		//	bool			m_bSteppedOnRight;
		//	float			m_fCurrentRotationRadian;
		//	float			m_fCurrentRotationSpeed;

		//	LineAnimRotateIfSteppedOn();
		//	void OnFrameMove( double fTime, float fElapsedTime );
		//};


		//struct LAT_SEESAW_IF_STEPPED_ON : LineAnimData
		//{
		//	D3DXVECTOR3		m_vRotationAxis;
		//	D3DXVECTOR3		m_vRotationCenter;
		//	float			m_fLeftWingLength;
		//	float			m_fRightWingLength;
		//	float			m_fMaxRotationRadian;
		//	float			m_fMaxRotationSpeed;
		//	float			m_fRotationAcceleration;
		//	bool			m_bRotateBackToOriginalWhenNotSteppedOn;
		//	float			m_fOriginalRotationRadian;


		//	bool			m_bSteppedOnLeft;
		//	bool			m_bSteppedOnRight;
		//	float			m_fCurrentRotationRadian;
		//	float			m_fCurrentRotationSpeed;

		//	LineAnimRotateIfSteppedOn();
		//	void OnFrameMove( double fTime, float fElapsedTime );
		//};

























		struct CameraData
		{
			LINE_CAMERA_TYPE m_eType;

			D3DXVECTOR3		m_vEye;
			D3DXVECTOR3		m_vLookAt;
			bool			m_bFocusUnit;
			D3DXVECTOR3		m_vRelativeEye;
			float			m_fCameraRepositionSpeed;

			CameraData()
			: m_eType( LCT_INVALID )
			, m_vEye( 0.f, 0.f, 0.f )
			, m_vLookAt( 0.f, 0.f, 0.f )
			, m_bFocusUnit( true )
			, m_vRelativeEye( 0.f, 0.f, 0.f )
			, m_fCameraRepositionSpeed( 100.f )
			{
			}
		};



	private:
		CKTDGLineMap();
		virtual ~CKTDGLineMap(void);

	public:
		static CKTDGLineMap* CreateLineMap() { return new CKTDGLineMap; }


		void SetLuaBinder( KLuabinder* pKLuabinder ) { m_pKLuabinder = pKLuabinder; }
		bool LoadLineMap( wstring fileName );


		virtual HRESULT OnFrameMove( double fTime, float fElapsedTime );



		bool GetNearOtherLine( D3DXVECTOR3 pos, D3DXVECTOR3 &outPos, int lastLine, int &lineIndex, float fDistMax = 300.f);

//{{ robobeg : 2008-10-28
		//virtual HRESULT OnFrameRender();
        virtual RENDER_HINT OnFrameRender_Prepare();
        virtual void            OnFrameRender_Draw();
//}} robobeg : 2008-10-28

		__forceinline bool IsOnLineConnect( D3DXVECTOR3 pos, int* pLastLineIndex, float fRadius = 1.0f, D3DXVECTOR3* pOutPos = NULL, bool bIsMonster = false )
		{
			KTDXPROFILE();
			D3DXVECTOR3	temp;
			float		length;
			const LineData*	pLineData;

            pLineData = m_LineList[*pLastLineIndex];
            if( bIsMonster == false && pLineData->lineType == LT_MONSTER_ROAD )
                return false;

            if( bIsMonster == true )
			{
				switch ( pLineData->lineType )
				{
				case LT_UNIT_ROAD:
				case LT_JUMP_UP_ABS:
				case LT_JUMP_UP_REL:
					return false;
					break;

				default:
					break;
				}
			}


//{{ kimhc // 2010.8.3 // WALL 타입의 라인맵 처리 변경
			if ( LT_WALL == pLineData->lineType )
				return false;
//}} kimhc // 2010.8.3 // WALL 타입의 라인맵 처리 변경

			length = GetNearest( pos, &temp, *pLastLineIndex );

			if( length < fRadius )
			{
				if( pOutPos != NULL )
					*pOutPos = temp;
				return true;
			}
			else
			{
				float fDistanceToStart	= 999999;
				float fDistanceToEnd	= 999999;
				//pLineData = m_LineList[*pLastLineIndex];

				fDistanceToStart	= GetDistance( pos, pLineData->startPos );
				fDistanceToEnd		= GetDistance( pos, pLineData->endPos );

                int iNumLineList = (int) m_LineList.size();

				if( fDistanceToStart < fDistanceToEnd )
				{
                    if ( pLineData->beforeLine >= 0
                        && pLineData->beforeLine < iNumLineList
                        && true == m_LineList[pLineData->beforeLine]->bEnable )
					{
                        if( bIsMonster == false &&
                            m_LineList[pLineData->beforeLine]->lineType  == LT_MONSTER_ROAD
                            )
                            return false;
						if( bIsMonster == true )
						{
							switch ( pLineData->lineType )
							{
							case LT_UNIT_ROAD:
							case LT_JUMP_UP_ABS:
							case LT_JUMP_UP_REL:
								return false;
								break;

							default:
								break;
							}
						}
						//{{ kimhc // 2010.8.3 // WALL 타입의 라인맵 처리 변경
						if ( LT_WALL == pLineData->lineType )
							return false;
						//}} kimhc // 2010.8.3 // WALL 타입의 라인맵 처리 변경

						if( pOutPos != NULL )
							*pOutPos = m_LineList[pLineData->beforeLine]->endPos;
						*pLastLineIndex = pLineData->beforeLine;
						return true;
					}
					else
					{
						//if( outPos != NULL )
						//	*outPos = lineData.startPos;
						return false;
					}
				}
				else
				{
                    if ( pLineData->nextLine >= 0
                        && pLineData->nextLine < iNumLineList
                        && true == m_LineList[pLineData->nextLine]->bEnable )
					{


                        if( bIsMonster == false &&
                            m_LineList[pLineData->nextLine]->lineType == LT_MONSTER_ROAD
                            )
                            return false;
						if( bIsMonster == true )
						{
							switch ( pLineData->lineType )
							{
							case LT_UNIT_ROAD:
							case LT_JUMP_UP_ABS:
							case LT_JUMP_UP_REL:
								return false;
								break;

							default:
								break;
							}
						}
						//{{ kimhc // 2010.8.3 // WALL 타입의 라인맵 처리 변경
						if ( LT_WALL == pLineData->lineType )
							return false;
						//}} kimhc // 2010.8.3 // WALL 타입의 라인맵 처리 변경

						if( pOutPos != NULL )
							*pOutPos = m_LineList[pLineData->nextLine]->startPos;
						*pLastLineIndex = pLineData->nextLine;
						return true;
					}
					else
					{
						//if( outPos != NULL )
						//	*outPos = lineData.endPos;
						return false;
					}
				}
			}
		}

		__forceinline bool IsOnLine( D3DXVECTOR3 pos, float fRadius, D3DXVECTOR3* pOutPos, int* pLineIndex, bool bIsMonster = false )
		{
			KTDXPROFILE();
			D3DXVECTOR3	temp;

			int			index = 0;
			if( pLineIndex != NULL )
				index = *pLineIndex;
			bool bFound = GetNearestWithBound( pos, fRadius, &temp, &index, bIsMonster );
            if ( bFound == false )
                return false;
			if( pOutPos != NULL )
				*pOutPos = temp;
			if( pLineIndex != NULL )
				*pLineIndex = index;
            return true;
		}

		__forceinline bool IsDownLine( D3DXVECTOR3 pos, float fRadius = 1.0f, D3DXVECTOR3* pOutPos = NULL, int* pLineIndex = NULL, bool /*bIsMonster*/ = false )
		{
			KTDXPROFILE();
			D3DXVECTOR3	temp;

			int			index = 0;
			if( pLineIndex != NULL )
				index = *pLineIndex;
			bool bFound = GetDownNearestWithBound( pos, fRadius, &temp, &index );
            if ( bFound == false )
                return false;
			if( pOutPos != NULL )
				*pOutPos = temp;
			if( pLineIndex != NULL )
				*pLineIndex = index;
			return true;
		}


		__forceinline D3DXVECTOR3 GetLandPosition_LUA( D3DXVECTOR3 pos )
		{
			return GetLandPosition( pos, 30.f, (int*) NULL );
		}


		__forceinline D3DXVECTOR3 GetLandPosition( D3DXVECTOR3 pos, float fRadius = 1.0f, unsigned char* pLineIndex = NULL ) const
		{
			if( pLineIndex != NULL )
			{
				int tempLineIndex = *pLineIndex;
				D3DXVECTOR3 bRetVal = GetLandPosition( pos, fRadius, &tempLineIndex );
				*pLineIndex = (unsigned char) tempLineIndex;
				return bRetVal;
			}
			else
			{
				int* pTemp = NULL;
				return GetLandPosition( pos, fRadius, pTemp );
			}
		}
		D3DXVECTOR3 GetLandPosition( D3DXVECTOR3 pos, float fRadius = 1.0f, int* pLineIndex = NULL ) const;

		bool CanPassUp( const D3DXVECTOR3& pos );

		__forceinline bool CanDown( const D3DXVECTOR3& pos, float fRadius = 1.0f, D3DXVECTOR3* pOutPos = NULL, unsigned char* pLineIndex = NULL )
		{
			if( pLineIndex != NULL )
			{
				int tempLineIndex = *pLineIndex;
				bool bRetVal = CanDown( pos, fRadius, pOutPos, &tempLineIndex );
				*pLineIndex = (unsigned char) tempLineIndex;
				return bRetVal;
			}
			else
			{
				int* pTemp = NULL;
				return CanDown( pos, fRadius, pOutPos, pTemp );
			}
		}

		// @ bOnLine : 주의!! input parameter입니다. 캐릭터가 라인위에 있는지 공중에 떠 있는지
		bool CanDown( D3DXVECTOR3 pos, float fRadius = 1.0f, D3DXVECTOR3* pOutPos = NULL, int* pLineIndex = NULL,
			bool bDownRadius = true, bool bIsMonster = false, IN bool bOnLine = true );



		bool CrossedWall( D3DXVECTOR3& oldPos, D3DXVECTOR3& newPos, bool &bCanWallJump );

        __forceinline const LineData* GetLineData( int index ) const
        {
	        KTDXPROFILE();
	        if ( index < 0 || index >= (int)m_LineList.size() )
	        {
		        return NULL;
	        }

	        return m_LineList[index];
        }
        __forceinline LineData* AccessLineData( int index )
        {
	        KTDXPROFILE();
	        if ( index < 0 || index >= (int)m_LineList.size() )
	        {
		        return NULL;
	        }

	        return m_LineList[index];
        }


		__forceinline int GetNumLineData() { return (int)m_LineList.size(); }
		__forceinline float GetLandHeight() { return m_fLandHeight; }
		__forceinline int GetNumCameraData() { return static_cast<int>(m_vecCameraData.size()); }
		__forceinline const CameraData * GetCameraDataList( int index )
		{
			KTDXPROFILE();
			if( 0 <= index && GetNumCameraData() > index )
				return &(m_vecCameraData[index]);

			return NULL;
		}

		int GetRedTeamStartPosNum(){ return (int) m_vecRedTeamStartInfo.size(); }
		int GetBlueTeamStartPosNum(){ return (int) m_vecBlueTeamStartInfo.size(); }
		int GetStartPosNum(){ return (int) m_mapStartInfo.size(); }
		const StartInfoMap& GetStartInfoMap() { return m_mapStartInfo; }
		D3DXVECTOR3 GetRedTeamStartPosition( int index )
		{
			if( index < 0 || index >= (int)m_vecRedTeamStartInfo.size() )
				return D3DXVECTOR3( 0, 0, 0 );
			return m_vecRedTeamStartInfo[index].m_vStartPos;
		}
		D3DXVECTOR3 GetBlueTeamStartPosition( int index )
		{
			if( index < 0 || index >= (int)m_vecBlueTeamStartInfo.size() )
				return D3DXVECTOR3( 0, 0, 0 );
			return m_vecBlueTeamStartInfo[index].m_vStartPos;
		}
		bool GetRedTeamStartRight( int index )
		{
			if( index < 0 || index >= (int)m_vecRedTeamStartInfo.size() )
				return true;
            return m_vecRedTeamStartInfo[index].m_bStartRight;
		}
		bool GetBlueTeamStartRight( int index )
		{
			if( index < 0 || index >= (int)m_vecBlueTeamStartInfo.size() )
				return true;
			return m_vecBlueTeamStartInfo[index].m_bStartRight;
		}
		int GetRedTeamStartLineIndex( int index )
		{
			if( index < 0 || index >= (int)m_vecRedTeamStartInfo.size() )
				return -1;
            return m_vecRedTeamStartInfo[index].m_iStartLineIndex;
		}
		int GetBlueTeamStartLineIndex( int index )
		{
			if( index < 0 || index >= (int)m_vecBlueTeamStartInfo.size() )
				return -1;
            return m_vecBlueTeamStartInfo[index].m_iStartLineIndex;
		}
		D3DXVECTOR3 GetStartPosition( int key )
		{
			StartInfoMap::const_iterator it = m_mapStartInfo.find( key );
			if( it != m_mapStartInfo.end() )
                return it->second.m_vStartPos;
			return D3DXVECTOR3( 0, 0, 0 );
		}
		bool GetStartRight( int key )
		{
			StartInfoMap::const_iterator it = m_mapStartInfo.find( key );
			if( it != m_mapStartInfo.end() )
                return  it->second.m_bStartRight;
			return true;
		}
		int GetStartLineIndex( int key )
		{
			StartInfoMap::const_iterator it = m_mapStartInfo.find( key );
			if( it != m_mapStartInfo.end() )
                return  it->second.m_iStartLineIndex;
			return -1;
		}




		int GetRandomStartPosIndex();

		D3DXVECTOR3 GetRandomPosition_LUA()
		{
			return GetRandomPosition( NULL, 0.f, false );
		}
		//{{oasis:김상윤////2009-10-7////
		D3DXVECTOR3 GetFixedDistancePosition( D3DXVECTOR3* pvPos, float fDist, bool bIsRight );
		//}}oasis:김상윤////2009-10-7////

		bool GetFixedDistancePositionForWeaponTaker( const D3DXVECTOR3* pvPos, float fDist, bool bIsRight, OUT D3DXVECTOR3& vResultPos );


		D3DXVECTOR3 GetRandomPosition( const D3DXVECTOR3* pvPos, float fRange, bool bOnThisLineGroup );
		D3DXVECTOR3 GetRandomPositionDir( const D3DXVECTOR3* pvPos, float fRange, bool bOnThisLineGroup, bool bIsRight );
        D3DXVECTOR3 GetNearest_LUA(D3DXVECTOR3 inPos);

		bool Step( int lineIndex, D3DXVECTOR3 pos );




		int GetLineSetIndex( int lineIndex );


		void ReBuildLineGroup();
		LineGroupID GetLineGroupIncludesLineData( int iLineIndex ) const;
        D3DXVECTOR3 GetLineGroupStartPos( const LineGroupID& id ) const
        {
            const LineData* pLineData = GetLineData( id.m_iStartLineIndex );
            if ( pLineData != NULL )
                return pLineData->startPos;
            return D3DXVECTOR3(0,0,0);
        }
        D3DXVECTOR3 GetLineGroupEndPos( const LineGroupID& id ) const
        {
            const LineData* pLineData = GetLineData( id.m_iEndLineIndex );
            if ( pLineData != NULL )
                return pLineData->endPos;
            return D3DXVECTOR3(0,0,0);
        }
        void DisableLines( const std::vector<int>& vecLineIndices );
		void EnableLineData( int iIndex, bool bEnable );


		void EnableLineSet( int iLineSetIndex, bool bEnable );
		void DisableAllLineData();

		//LUA
		void SetLandHeight( float fLandHeight ){ m_fLandHeight = fLandHeight; }
		void SetTeamStartPos( bool bRed, D3DXVECTOR3 pos, bool bRight, int iLineIndex )
		{
            StartInfo   kInfo;
            kInfo.m_vStartPos = pos;
            kInfo.m_bStartRight = bRight;
            kInfo.m_iStartLineIndex = iLineIndex;

			if( bRed == true )
			{
                m_vecRedTeamStartInfo.push_back( kInfo );
			}
			else
			{
                m_vecBlueTeamStartInfo.push_back( kInfo );
			}
		}
		void AddStartPos( int key, D3DXVECTOR3 pos, bool bRight, int iLineIndex )
		{
            StartInfo   kInfo( pos, bRight, iLineIndex );
            m_mapStartInfo[key] = kInfo;
		}
		bool AddLine_LUA();

		// 움직이는 라인맵
		bool AddLineAnim_LUA();
		void BuildLineAnimDataTree();
		bool AssignLineAnimDataToLine( CKTDGLineMap::LineData* pLineData, int iLineAnimDataIndex );
		CKTDGLineMap::LineAnimData* GetLineAnimData( int iLineAnimDataIndex );
		CKTDGLineMap::LineAnimDataMap& GetLineAnimDataMap() { return m_mapLineAnimData; }


		bool AddCameraData_LUA();



		const CameraData* GetCameraData( int iLineIndex ) const
		{
			if( iLineIndex < 0 ||
				iLineIndex >= (int) m_LineList.size() )
			{
				return NULL;
			}

			const LineData* pLineData = m_LineList[ iLineIndex ];

			if( NULL == pLineData )
				return NULL;

			if( pLineData->m_iCameraIndex < 0 ||
				pLineData->m_iCameraIndex >= (int) m_vecCameraData.size() )
			{
				return NULL;
			}

			return &m_vecCameraData[ pLineData->m_iCameraIndex ];
		}

		void AddCameraData( CameraData cameraData ) { m_vecCameraData.push_back(cameraData); }



        // 모든 라인들 중에서 pos와 가장 가까운 라인을 찾고 최단 거리 지점을 pOutPos로 출력한다.
        // 리턴값 : pos와 가장 가까운 라인의 index
        __forceinline int		GetNearestLine(	IN const D3DXVECTOR3& pos, OUT D3DXVECTOR3* pOutPos = NULL, IN bool bIsMonster = false )
        {
            int			touchLine		= -1;
            float		touchDistance	= 9999999.0f;
            D3DXVECTOR3	touchPos;

            //디딜 곳이 없다면 모든 선을 검색한다
            for( int i = 0; i < (int)m_LineList.size(); i++ )
            {
                //정상적인 발판만 확인한다
                if( VerifyLine( i, bIsMonster ) == false )
                    continue;

                //점과 선사이의 최단 거리를 구한다.
                D3DXVECTOR3 projectionPoint;
                float		fFinalLength;
                fFinalLength = GetLengthToLine( pos, i, &projectionPoint );

                if( fFinalLength <= touchDistance )
                {
                    touchLine		= i;
                    touchDistance	= fFinalLength;
                    touchPos		= projectionPoint;
                }
            }

            if( pOutPos != NULL )
                *pOutPos = touchPos;

            return touchLine;
        }
		bool	UpdateLineVB( bool bOnlyAnimLine = false );

		const LineData* GetAnyEnabledNormalLine();

		bool GetAnyEnabledLine();	// SERV_PET_SYSTEM


		float GetXMin() const { return m_fXMin; }
		float GetXMax() const { return m_fXMax; }
		float GetYMin() const { return m_fYMin; }
		float GetYMax() const { return m_fYMax; }


		float GetStopTime() const { return m_fStopTime; }
		void SetStopTime(float val) { m_fStopTime = val; }

        bool    UpdateLineData( int iIndex, const D3DXVECTOR3& vStart, const D3DXVECTOR3& vEnd );
		static void	MakeLine( LineData* pLineData );

//{{ kimhc // 2010.7.6 // 몬스터가 생성한 라인맵

		void EnableLineData_LUA( int iIndex, bool bEnable )
		{
			EnableLineData( iIndex, bEnable );
		}

		void SetStartEndPosAtThisLineMap_LUA( int iIndex, D3DXVECTOR3 vStartPos, D3DXVECTOR3 vEndPos );
		int GetNumofLineMapCreatedByMonster() const { return m_iNumofLineMapCreatedByMonster; }
		void SetNumofLineMapCreatedByMonster(int val) { m_iNumofLineMapCreatedByMonster = val; }
//}} kimhc // 2010.7.6 // 몬스터가 생성한 라인맵


		virtual HRESULT OnResetDevice();
		virtual HRESULT OnLostDevice();

	private:



		bool	MakeLineVB();

        // lineIndex의 라인이 정상적인 라인인지 검증한다. lineIndex의 값이 실제 라인들의 인덱스 범위 내에 있어야 한다.
        // MONSTER_ROAD 가 정의되어 있으면 bIsMonster 가 false 일 때 몬스터 로드 타입의 라인들은 정상적인 라인이 아니다.
        // UNIT_ROAD 가 정의되어 있으면 bIsMonster가 true 일 때 유닛 로드 타입의 라인들은 정상적인 라인이 아니다.
        // 월 타입의 라인은 정상적인 라인이 아니다.
        // 리턴값 : 정상적인 라인인지의 여부
		__forceinline bool	VerifyLine( IN int lineIndex, IN bool bIsMonster = false )
		{
			//라인이 정상이면 TRUE
			if( lineIndex < 0 || (int)m_LineList.size() <= lineIndex )
				return false;

			const LineData* pLineData = m_LineList[lineIndex];

            if( !pLineData->bEnable )
            {
                return false;
            }

            if( !bIsMonster && pLineData->lineType == LT_MONSTER_ROAD )
                return false;
			if( bIsMonster == true )
			{
				switch ( pLineData->lineType )
				{
				case LT_UNIT_ROAD:
				case LT_JUMP_UP_ABS:
				case LT_JUMP_UP_REL:
					return false;
					break;

				default:
					break;
				}
			}
			if( pLineData->lineType == LT_WALL )
			{
				return false;
			}

			return true;
		}

        // pos 부터 lineIndex의 라인까지의 최단 거리를 구하고 라인 상의 최단 거리 지점을 pOutPos 로 출력한다.
        // 리턴값 : lineIndex의 라인이 정상적인 라인이 아닐 경우 -1.0f를 리턴한다. 정상적인 라인이면 최단 거리를
        //          리턴한다.
		__forceinline float	GetLengthToLine( IN const D3DXVECTOR3& pos, IN int lineIndex, OUT D3DXVECTOR3* pOutPos = NULL )
		{
			const LineData* pLineData = m_LineList[lineIndex];

			//정상적인 발판만 확인한다
			if( VerifyLine( lineIndex ) == false )
				return -1.0f;

			//점과 선사이의 최단 거리를 구한다.
			float fLineLength				= GetDistance( pLineData->startPos, pLineData->endPos );

			D3DXVECTOR3 lineDir = pLineData->endPos - pLineData->startPos;
			D3DXVec3Normalize( &lineDir, &lineDir );
			D3DXVECTOR3 pointDir = pos - pLineData->startPos;

			D3DXVECTOR3 projectionPoint;
			float		fProjectionLength	= D3DXVec3Dot( &lineDir, &pointDir );

			//만약 최단거리가 선 내부가 아니라면 선 가장자리로 보정한다.
			if( fProjectionLength <= 0.0f )
				projectionPoint = pLineData->startPos;
			else if( fProjectionLength >= fLineLength )
				projectionPoint = pLineData->endPos;
			else
				projectionPoint		= lineDir * fProjectionLength + pLineData->startPos;

			float fFinalLength		= GetDistance( pos, projectionPoint );

			if( pOutPos != NULL )
				*pOutPos = projectionPoint;

			return fFinalLength;
		}

        // 모든 라인들 상의 점들 중에서 inPos에서 가장 가까운 점을 찾고 그것을 pOutPos로 출력한다.
        // MONSTER_ROAD 가 정의되어 있으면 bIsMonster 가 false 일 때 몬스터 로드 타입의 라인들을 배제하고,
        // UNIT_ROAD 가 정의되어 있으면 bIsMonster가 true 일 때 유닛 로드 타입의 라인들을 배제한다.
        // 리턴값 : inPos부터 pOutPos까지의 거리를 리턴한다.

        // inPos에서 가장 가까운 라인을 찾고 그 라인상의 점들 중 inPos와 가장 가까운 점을 pOutPos로 출력한다.
        // MONSTER_ROAD 가 정의되어 있으면 bIsMonster 가 false 일 때 몬스터 로드 타입의 라인들을 배제하고,
        // UNIT_ROAD 가 정의되어 있으면 bIsMonster가 true 일 때 유닛 로드 타입의 라인들을 배제한다.
        // 리턴값 : 만약 inPos가 라인의 범위에 있다면 inPos부터 pOutPos까지의 거리를 리턴한다.
        //          그렇지 않으면 999999를 리턴한다.
		__forceinline bool GetNearestWithBound( IN D3DXVECTOR3 inPos, IN float fRadius, OUT D3DXVECTOR3* pOutPos, IN OUT int* pLlineIndex, IN bool bIsMonster = false )
		{
			KTDXPROFILE();
			D3DXVECTOR3 target;
			float		fProjectionLength;
			D3DXVECTOR3 outPos;
			int			iOutIndex = 0;
			bool		bFound = false;
            float		outLength = fRadius;
			float		tempLength;
			D3DXVECTOR3	tempPos;

			//*lineIndex = -1;
			int iOffset = 0;
			int iSize = m_LineList.size();
			if( pLlineIndex != NULL && *pLlineIndex >= 0 && *pLlineIndex < iSize )
			{
				iOffset = *pLlineIndex;
			}
			for( int iBase = 0; iBase < iSize; iBase++ )
			{
                int i = iBase + iOffset;
                if ( i >= iSize )
                    i -= iSize;
				const LineData* pLineData = m_LineList[i];
				if( false == pLineData->bEnable )
					continue;
                if( bIsMonster == false && pLineData->lineType == LT_MONSTER_ROAD )
					 continue;
				if( bIsMonster == true )
				{
					switch ( pLineData->lineType )
					{
					case LT_UNIT_ROAD:
					case LT_JUMP_UP_ABS:
					case LT_JUMP_UP_REL:
						continue;
						break;

					default:
						break;
					}
				}

				//{{ kimhc // 2010.8.3 // WALL 타입의 라인맵 처리 변경
				if ( LT_WALL == pLineData->lineType )
					continue;
				//}} kimhc // 2010.8.3 // WALL 타입의 라인맵 처리 변경


				if( inPos.x < pLineData->m_vPosMin.x || inPos.x > pLineData->m_vPosMax.x )
				{
					if( inPos.y < pLineData->m_vPosMin.y || inPos.y > pLineData->m_vPosMax.y )
					{
						if( inPos.z < pLineData->m_vPosMin.z || inPos.z > pLineData->m_vPosMax.z )
						{
							continue;
						}
					}
				}

				if ( inPos.x + outLength <= pLineData->m_vPosMin.x || pLineData->m_vPosMax.x + outLength <= inPos.x )
					continue;
				if ( inPos.y + outLength <= pLineData->m_vPosMin.y || pLineData->m_vPosMax.y + outLength <= inPos.y )
					continue;
				if ( inPos.z + outLength <= pLineData->m_vPosMin.z || pLineData->m_vPosMax.z + outLength <= inPos.z )
					continue;


				KTDXPROFILE_BEGIN("MATH");
				target				= inPos - pLineData->startPos;
				fProjectionLength	= D3DXVec3Dot( &pLineData->dirVector, &target );
				if( fProjectionLength >= 0.0f && fProjectionLength < pLineData->fLength )
				{
					tempPos		= fProjectionLength * pLineData->dirVector + pLineData->startPos;
					tempLength	= GetDistance( tempPos, inPos );
                    if ( tempLength < outLength )
					{
						outLength = tempLength;
						outPos = tempPos;
						iOutIndex = i;
                        bFound = true;
					}//if

					if( tempLength == 0 )
						break;
				}

				KTDXPROFILE_END();
			}
			if( bFound == true )
			{
                if ( pOutPos != NULL )
				    *pOutPos = outPos;
				if ( pLlineIndex != NULL )
					*pLlineIndex = iOutIndex;
                return true;
			}
            return false;
		}

        // lineIndex의 라인상에서 inPos 와 가장 가까운 점을 찾아 pOutPos로 출력한다.
        // 리턴값 : 만약 inPos가 라인의 범위에 있다면 inPos부터 pOutPos까지의 거리를 리턴한다.
        //          그렇지 않으면 999999를 리턴한다.
		__forceinline float GetNearest( IN D3DXVECTOR3 inPos, OUT D3DXVECTOR3* pOutPos, IN int lineIndex )
		{
			KTDXPROFILE();

			D3DXVECTOR3 target;
			float		fProjectionLength;
			float		outLength = 999999;
			float		tempLength;
			D3DXVECTOR3	tempPos;

			target				= inPos - m_LineList[lineIndex]->startPos;
			fProjectionLength	= D3DXVec3Dot( &m_LineList[lineIndex]->dirVector, &target );
			if( fProjectionLength >= 0.0f && fProjectionLength < m_LineList[lineIndex]->fLength )
			{
				tempPos		= fProjectionLength * m_LineList[lineIndex]->dirVector + m_LineList[lineIndex]->startPos;
				tempLength	= GetDistance( tempPos, inPos );
				if( tempLength < outLength )
				{
					outLength	= tempLength;
					*pOutPos		= tempPos;
				}
			}

			return outLength;
		}

		//{{ kimhc // 2010.7.6 // 몬스터가 생성한 라인맵
	public:
		//}} kimhc // 2010.7.6 // 몬스터가 생성한 라인맵

        // inPos보다 아래에 있는 라인들 중에서 가장 가까운 라인을 찾고 그 라인상의 점들 중 inPos와 가장 가까운 점을 pOutPos로 출력한다.
        // 리턴값 : 만약 inPos가 라인의 범위에 있다면 inPos부터 pOutPos까지의 거리를 리턴한다.
        //          그렇지 않으면 999999를 리턴한다.

        __forceinline bool GetDownNearestWithBound( IN D3DXVECTOR3 inPos, IN float fBound_, OUT D3DXVECTOR3* pOutPos, OUT int* pLlineIndex )
		{
			KTDXPROFILE();
			D3DXVECTOR3 target;
			float		fProjectionLength;
			D3DXVECTOR3 outPos;
            int         iOutIndex = 0;
            float       outLength = fBound_;
            bool        bFound = false;
			float		tempLength;
			D3DXVECTOR3	tempPos;

			//*lineIndex = -1;

			int iOffset = 0;
			int iSize = m_LineList.size();
			if( pLlineIndex != NULL && *pLlineIndex >= 0 && *pLlineIndex < iSize )
			{
				iOffset = *pLlineIndex;
			}
			for( int iBase = 0; iBase < iSize; iBase++ )
			{
                int i = iBase + iOffset;
                if ( i >= iSize )
                    i -= iSize;
				const LineData* pLineData = m_LineList[i];
				if( false == pLineData->bEnable )
					continue;

				if( inPos.y < pLineData->m_vPosMin.y )
					continue;

				if( inPos.x < pLineData->m_vPosMin.x || inPos.x > pLineData->m_vPosMax.x )
				{
					if( inPos.y > pLineData->m_vPosMax.y )
					{
						if( inPos.z < pLineData->m_vPosMin.z || inPos.z > pLineData->m_vPosMax.z )
						{
							continue;
						}
					}
				}
                if ( inPos.x + outLength <= pLineData->m_vPosMin.x || pLineData->m_vPosMax.x + outLength <= inPos.x )
                    continue;
                if ( pLineData->m_vPosMax.y + outLength <= inPos.y )
                    continue;
                if ( inPos.z + outLength <= pLineData->m_vPosMin.z|| pLineData->m_vPosMax.z + outLength <= inPos.z )
                    continue;

				target				= inPos - pLineData->startPos;
				fProjectionLength	= D3DXVec3Dot( &pLineData->dirVector, &target );
				if( fProjectionLength >= 0.0f && fProjectionLength < pLineData->fLength )
				{
					tempPos		= fProjectionLength * pLineData->dirVector + pLineData->startPos;
					tempLength	= GetDistance( tempPos, inPos );
					if( tempLength < outLength )
					{
						outLength		= tempLength;
						outPos		= tempPos;
						iOutIndex	= i;
                        bFound = true;
					}
				}
			}
            if ( bFound == true )
            {
                if ( pOutPos != NULL )
                    *pOutPos = outPos;
                if ( pLlineIndex != NULL )
                    *pLlineIndex = iOutIndex;
                return true;
            }
            return false;
		}

		bool AddTrigger_LUA();
		CKTDGLineMap::LINEMAP_TRIGGER* GetLineMapTrigger() { return &m_LineMapTrigger; }


// 기가 프로미넌스를 위한 위치 잡는 기능
		bool GetFixedDistancePositionInAllLine( const D3DXVECTOR3& vDirVector, const D3DXVECTOR3& vPos, float fDist, bool bIsRight, OUT D3DXVECTOR3& vTargetPos, OUT int& iTargetLineIndex );





		// The original measured each line's distance for a near flag that it no longer sets (commented out); a no-op.
		void CheckNearLines( const D3DXVECTOR3& /*vPos_*/, bool /*bRight_*/ = true, float /*fDist_*/ = 1800.f )
		{
		}


	private:

        __forceinline LineData* _AccessLineData( int index )
        {
	        KTDXPROFILE();
	        if ( index < 0 || index >= (int)m_LineList.size() )
	        {
		        return NULL;
	        }
	        return m_LineList[index];
        }

	private:

		vector<LineData*>			m_LineList;
		vector<LineData*>			m_WallList;


		LPDIRECT3DVERTEXBUFFER9		m_pLineMapVB;

		float						m_fLandHeight;
        std::vector<StartInfo>      m_vecRedTeamStartInfo;
        std::vector<StartInfo>      m_vecBlueTeamStartInfo;
        StartInfoMap                m_mapStartInfo;


		vector<CameraData>			m_vecCameraData;

		float						m_fXMin;
		float						m_fXMax;
		float						m_fYMin;
		float						m_fYMax;

		KLuabinder*					m_pKLuabinder;

		float						m_fStopTime;


		// 움직이는 라인맵 관련
		LineAnimDataMap	m_mapLineAnimData;
		LineAnimData m_RootLineAnimData;

		//{{ kimhc // 2010.7.22 // 몬스터가 생성한 라인맵
		int							m_iNumofLineMapCreatedByMonster;	// 몬스터에 의해 생성된 라인맵의 개수
		//}} kimhc // 2010.7.22 // 몬스터가 생성한 라인맵
		CKTDGLineMap::LINEMAP_TRIGGER				m_LineMapTrigger;



};
