#pragma once



//{{ 허상형 : [2009/11/12] //	핵쉴드 국가별 라이선스 키
	#define HS_LICENSE_KEY L""
	#define HS_GAME_CODE	0
//}} 허상형 : [2009/11/12] //	핵쉴드 국가별 라이선스 키

//{{ 2010/10/14	조효진	핵쉴드 업데이트 기능
//}}

//{{ 2011.3.21	조효진	핵쉴드 모니터링 서버 연동
//}}

//{{ 허상형 : [2009/8/3] //	핵쉴드 관련 함수
//	핵쉴드 초기화
BOOL	HS_Init();

//	핵쉴드 서비스 시작
BOOL	HS_StartService();

//	핵쉴드 서비스 일시 정지
BOOL	HS_PauseService();

//	핵쉴드 서비스 재개
BOOL	HS_ResumeService();

//	핵쉴드 서비스 정지
BOOL	HS_StopService();

//	핵쉴드 종료
BOOL	HS_UnInit();

//	콜백함수
int	__stdcall	HS_CallbackProc( long lCode, long lParamSize, void* pParam );

//	에러메시지 출력
void ShowErrorMsg( int iErrorMsg );

//	비정상 종료시 처리 함수
LONG WINAPI GAME_UnhandledExceptionHandler(struct _EXCEPTION_POINTERS *lpTopLevelExceptionFilter);

//	에러 발견시 메일 전송 함수
void	SendHackShieldLog( int iError, char *wszMsg );
