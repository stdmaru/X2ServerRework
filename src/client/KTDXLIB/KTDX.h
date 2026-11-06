#ifndef _KTDX_H_
#define _KTDX_H_

#ifndef LINE1
#define LINE1(x_) #x_
#endif // #ifndef LINE1
#ifndef LINE
#define LINE(x_) LINE1(x_)
#endif // #ifndef LINE

#ifndef TODO
#define TODO(msg_) message ( __FILE__ "(" LINE(__LINE__)  "): [TODO] " msg_ )
#endif // #ifndef TODO
#ifndef NOTE
#define NOTE(msg_) message ( __FILE__ "(" LINE(__LINE__)  "): [NOTE] " msg_ )
#endif // #ifndef NOTE


//////////////////////////////////////////////////////////////////////////
// 2009-06-01, dmlee,
// 항상 포함되어야 하는 macro는 _ALWAYS_ 안쪽에 정의해서 사용
// 본섭 패치 버전은 _SERVICE_
// 오픈테섭 패치 버전은 _SERVICE_, _OPEN_TEST_
// 해외 패치 버전은 _FOREIGN_
// 테스트 코드를 포함한 사내 패치 버전은 _IN_HOUSE_
// 테스크 코드를 제외한 사내 패치 버전은 _IN_HOUSE_SERVICE_READY_QA_
//////////////////////////////////////////////////////////////////////////





//#define NVPerfHUD
//#define PROFILE_BUILD // Uncomment this line to enable profiling


//#define _SERVICE_MANUAL_LOGIN_ // 본섭에 VC++에서 디버깅으로 접속할 때
//#define _SERVICE_
//#define _OPEN_TEST_
//#define _OPEN_TEST_2_
//#define _NEXON_KR_
//#define _GAMANIA_TW_
//#define _IN_HOUSE_
//#define _IN_HOUSE_SERVICE_READY_QA_

//#ifdef  TARGET_UNDEFINED
//    #define _NEXON_KR_
//    #define _IN_HOUSE_
//    #define _TEST_UNPACK_
//#endif  TARGET_UNDEFINED


#ifdef _USE_BOOST_CONST_STRING_

#define BOOST_CONST_CSTRING             boost::const_cstring
#define BOOST_CONST_STRING_CREF( x )    std::cref( x )

#else

#define BOOST_CONST_CSTRING             std::string
#define BOOST_CONST_STRING_CREF( x )    ( x )

#endif

// 패키지 암호화는 무조건 !! - robobeg, 2014-01-21

#if !X2_DEV_TOOLS
		//#define _ENCRIPT_SCRIPT_

//	#define NO_GAMEGUARD	// 해외팀 제거
    // #define CHECK_NP_HACKUSER	// 핵유저에 한해서 게임가드 실행
	//#define BUG_TRAP				// X2Main.cpp 로 이관

// 허상형 : [2009/8/4] //	핵실드
//	#define	HACK_SHIELD		// 해외팀 제거
//{{ 허상형 : [2009/10/26] //	핵쉴드 LMP기능
//	#define	HS_LMP			// 해외팀 제거
//{{ 허상형 : [2010/5/24/] //	핵실드 어드민 계정 검사 안함
//	#define HS_NO_ADMIN_NEED

//	#define ADD_CHKOPT_ABNORMAL_FUNCTION_CALL_V2	// 핵실드 기능 추가	// 해외팀 제거
//	#define ADDITIONAL_HS							// 핵실드 기능 추가	// 해외팀 제거
//	#define SEND_URL_TO_NEXON				// 어느 매체를 통해 게임 내 유입이 많이 이루어졌는지, 신규가입을 많이 했는지 넥슨쪽에 URL전달 // 해외팀 제거
//	#define SEND_NEXON_WISE_LOG_BY_URL		// 넥슨 유저 접속 및 케릭터 생성 관련 와이즈 로그 전송 // 해외팀 제거
	//#define AVATAR_IMAGE_TEST				// kimhc // 홈페이지 개편에서 아바타 이미지 표현으로 사용될 캐릭터 이미지 캡쳐 기능 테스트



#elif X2_DEV_TOOLS
    //#define CHECK_NP_HACKUSER // 사내에서는 전사용자 대상으로 게임가드 실행
	//#define _USE_NEXON_MSG_INHOUSE // 사내에서 본섭 넥슨메신져 열결 (왠만해선 사용하지 말자.)

	//#define	HACK_SHIELD

    //#define MASS_FILE_FIRST


#else
	NOT GOOD.

#endif // _SERVICE_

//#if defined( NDEBUG )	// kimhc // 2010-06-28 // 사내 또는 오픈테섭 릴리즈에서만 적용
//#define	IGA_TEST	//kimhc // 2010-03-03 // 광고 노출도 적용
//#endif	// #if defined( NDEBUG )

// 더미다 적용
//#define IN_TESTING_THEMIDA_BY_TOOL_TEAM


// Always ----------------------------------------------------------------------
#include "Always.h"
#include "AlwaysButConditionally.h"
// -----------------------------------------------------------------------------





// New_Village -----------------------------------------------------------------

#if X2_DEV_TOOLS
//#define CHECK_OPENED_DEVICE
//#define MEMORY_LEAK_DETECTION_TEST
#endif


// 새 마을 렌더링 테스트

//#define SQUARE_CAMERA_TRACKING_TEST		// 광장에서 카메라가 캐릭터 느리게 따라가게 테스트



// 캐릭터 백그라운드 로딩 테스트
/** skinned mesh의 background 로딩 관련 작업을 테스트하기 시작하다.
- jintaeks on 2008-12-12, 11:12
광장에 진입할 때 X2WorldObjectSkinMesh.cpp 객체의 로딩 작업을 완료하다.
- jintaeks on 2008-12-13, 15:07*/

/** 광장에 진입하고 나서 광장으로 들어오는 다른 유저들의 리소스 로딩 작업을 검토하기 시작하다.
- jintaeks on 2008-12-13, 15:07*/


// 새마을 버전에 광장 유닛 백그라운드 로딩 적용시키기


// 새마을 버전에 fieldnpc 백그라운드 로딩
//#define NEW_VILLAGE_FIELD_NPC_BACKGROUND_LOADING_TEST


// uitextureData에서 SetTex호출 할때 텍스쳐가 바뀐 경우에만 settex호출하도록 수정






// -----------------------------------------------------------------------------
// 기술지원팀 최적화 작업 : robobeg, 2013-11-08


//#define X2OPTIMIZE_MASS_FILE_MANAGER_PER_THREAD_SYNC_IO   // 기본으로 적용토록 함
//#define X2OPTIMIZE_KFILESERIALIZER_MODIFIED   // robobeg : 항상 적용되게 하고 매크로는 주석처리함, 2013-11-08
//#define X2OPTIMIZE_NPC_LUASPACE_SHARING       // robobeg : 항상 적용되게 하고 매크로는 주석처리함, 2013-11-08

//#define SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK    // robobeg : 항상 적용되게 하고 매크로는 주석처리함, 2013-11-08
//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK

//#define X2OPTIMIZE_PARTICLE_SEQUENCE_DVB 	    // robobeg : 항상 적용되게 하고 매크로는 주석처리함, 2013-11-08
//#define  X2OPTIMIZE_PREVENT_ERRANEOUS_GAMEUNIT_PUSH // robobeg : 항상 적용되게 하고 매크로는 주석처리함, 2013-11-08
//#ifdef  X2OPTIMIZE_NPC_NONHOST_SIMULATION
//#define X2OPTIMIZE_NPC_NONHOST_SIMULATION_ANALYSIS
//#endif  X2OPTIMIZE_NPC_NONHOST_SIMULATION
// 비방장 시뮬레이션 오버헤드를 최소화한다.
//#define X2OPTIMIZE_PUSH_PASS_BUG_FIX					// robobeg : 항상 적용되게 하고 매크로는 주석처리함, 2013-11-08
//#define X2OPTIMIZE_USER_USER_PUSH_PASS_SYNC           // robobeg : 항상 적용되게 하고 매크로는 주석처리함, 2013-11-08
//#define X2OPTIMIZE_USER_USER_PUSH_PASS_SYNC_BUG_FIX   // robobeg : 항상 적용되게 하고 매크로는 주석처리함, 2013-11-08
// 통신 오류 등으로 인해 npc state 가 범위를 초과했을 때 크래쉬가 발생하는 것을 막는다
//#define X2OPTIMIZE_NPC_STATE_INDEXING_CRASH_BUG_FIX   // robobeg : 항상 적용되게 하고 매크로는 주석처리함, 2013-11-08
// 서로 다른 필드나 마을에 있는 파티원끼리 불필요하게 udp peer 연결하는 문제 방지
//#define X2OPTIMIZE_PARTY_UDP_CONNECTION_BUG_FIX       // robobeg : 항상 적용되게 하고 매크로는 주석처리함, 2013-11-08
// 비방장 유저가 NPC 투과 스킬 사용시 방장 컴퓨터에서 NPC 가 밀리는 버그 수정
//#define X2OPTIMIZE_USER_PASS_PUSH_UNIT_TIME_BUG_FIX   // robobeg : 항상 적용되게 하고 매크로는 주석처리함, 2013-11-08

// shader 상수 업데이트 최적화 : hcsung, 2013-11-08
// NPC 그림자 컬링 : hcsung, 2013-11-08
// 월드 오브젝트 mesh subset 부분 컬링 : hcsung, 2013-11-08
// NPC headbone frame 찾는 부하 개선 : hcsung, 2013-11-08
// 월드 오브젝트 mesh subset, bounding sphere 가 view frustum 안에 완전히 들어오면 부분컬링 안 함 : robobeg, 2013-12-03

// 게임 옵션 : 캐릭터,NPC 그림자 표시 : hcsung, 2013-11-08
// 게임 옵션 : NPC 죽을때 나는 사운드 : hcsung, 2013-11-08
// 게임 옵션 : damage effect 표시 : hcsung, 2013-11-08
// 게임 옵션 : 게임옵션 자동 설정 : hcsung, 2013-11-08
// damage effect 충돌검사 최적화 : hcsung, 2013-11-08
// 파티클 컬링 : hcsung, 2013-11-08
// dynamic buffer 의 수를 늘여서 lock 오버헤드를 줄인다 : hcsung : 2013-11-20


// 파티클, 이펙트 handle 화 : robobeg, 2013-11-08
// 파티클, 이펙트 시뮬레이션을 렌더링 직전에 1번 하기 : robobeg, 2013-11-08
// 컴파일 및 런타임에 각종 이펙트 handle 의 validity 를 체크할 수 있도록 함 : robobeg : 2013-11-14
// 마을 PET 리소스 백그라운드 로드 : robobeg : 2013-11-08
// 효율적인 stl container 로 대체 : robobeg, 2013-11-08
// 불필요한 pointer 사용 자제 : robobeg, 2013-11-08
// shared ptr -> intrusive ptr 로 대체 : robobeg, 2013-11-08
// NPC heuristic update 에 의한 최적화 : robobeg, 2013-11-08
// 호환성 유지를 위해 기존 NPC 스크립트 포맷도 지원

// 게임 옵션 저장한 것을 읽지 못하는 버그 : hcsung, 2013-11-08
// linemap 관련 버그 수정 및 최적화 : robobeg, 2013-11-08
// ara, chung 에서 particle handle 을 mesh 에서 지우려 하는 버그 수정 : robobeg, 2013-11-08
// 렌더링 게임 옵션 바꾼 뒤 크래쉬발생할 수 있는 버그 수정 : robobeg, 2013-11-08
// attacker 가 user/npc 일 때 구분 처리 제대로 안 되는 버그 수정 : robobeg, 2013-11-08
// X2ServerProtocol 쪽에서 multithread safety 문제로 크래쉬 발생하는 오류 수정 : robobeg, 2013-11-08
#define X2OPTIMIZE_X2SERVERPROTOCOL_MULTITHREAD_CRASH_BUG_FIX
// 이펙트를 육탄공격으로 캔슬하는 것 동작하지 않는 오류 수정, 최적화 버그 아님 : robobeg : 2013-11-13
// 필드 이펙트를 유저가 달고 다니는 버그 수정, 최적화 버그 아님 : robobeg : 2013-11-13
// 이펙트셋에서의 잠재적인 NULL 체크 버그 fix  : robobeg : 2013-11-20
// 비방장으로 필드진입시 start state 실행안되는 버그 수정  : robobeg : 2013-11-20
// 탈것을 타고 벽에 돌진한 뒤 빠져나오지 못하는 버그 수정(기존오류) : robobeg, 2013-11-21
// 락온 이펙트 충돌검사시 크래쉬 버그 수정 : robobeg, 2013-11-21
// dynamic buffer 렌더링할 때 데이타를 2개로 나눠 지정할 수 있게 한다. ( circular array 지원 ) : robobeg 2013-11-21
// 파티클 시스템 loop 를 외부로 노출시키면 발생할 수 있는 버그 : robobeg, 2013-12-03
// 파티클 시스템 등에서 랙이 이벤트 타이밍보다 커질 때 발생하는 velocity 누적 오류 수정 : robobeg, 2013-12-09

// 스킨메쉬 크래쉬 버그 수정, 기존 버그. 왜 이런 일이 발생하는가? : robobeg, 2013-12-04
// CX2Eqip 크래쉬 버그 수정  : robobeg, 2013-12-04
// CX2SlotItem 크래쉬 버그 수정  : robobeg, 2013-12-04
// CX2KTDGXSkinAnim 크래쉬 버그 수정 : robobeg, 2013-12-04
//

// TET, XET 전처리, 기본으로 적용
// Dialog, Linemap 등 LUA 에서 인터프리터 사용하지 않도록 포맷 수정, 기본으로 적용

// 언팩방지 적용... 패키지 옵션임 : robobeg, 2014-01-17


    #if !X2_DEV_TOOLS
    #endif // defined(_SERVICE_) || defined(_IN_HOUSE_SERVICE_READY_QA_)
    #if !defined(DEBUG) && !defined(_DEBUG)
    #endif
    // 주요 파일 암호화 강화
    // 주요 파일, 패키징시 압축 후 암호화하는 방식으로 변경



// 2개의 인접한 mass file 읽기 지원, 텍스쳐와 TET 파일 로딩 속도 개선

// 해킹툴 리스트 체크시 멀티쓰레드 접근에 의한 크래쉬 방지한다.
// 프리타입 폰트를 위한 메모리 절약
// 백그라운드 로딩 쓰레드가 좀 더 협조적으로...
// 일반적인 thread 안정성 개선
// COLLECTHACKINFO thread 의 thread 안정성 개선
// CKTDXLog 클래스 개체 ( g_CKTDXLog ) 의 thread safety 보장
// KTDGLineMap buffer overrun 버그 수정
// SERVER TIME OUT 로그 출력 무시, 컴파일 제대로 되었나 확인
//#define X2OPTIMIZE_IGNORE_SERVER_TIME_OUT_LOG
// 패키징시 uncompress 에러 체크한다. 불필요한 크래쉬를 줄이며, Kom 오류감지나 핵 디텍션으로 활용될 수 있다.
// 반디 에서 KTDGDynamicVBManager 사용하도록 수정 : XP 머신에서 크래쉬 방지, robobeg - 2014-01-23
// 게임 종료상황에서 죽는 버그 수정
// 핵체크 프로세스 셋 업데이트하면서 멀티쓰레드 이슈로 발생하는 크래쉬 수정
// 베스마 돌 떨어지는 연출신에서 크래쉬 버그 수정


#if X2_DEV_TOOLS

// 로컬 파일에 대해서는 xor encrypt 적용되지 않도록 수정
//#define X2OPTIMIZE_VERIFY_NPC_SCRIPT_CONSISTENCY

//#define X2OPTIMIZE_TCP_RELAY_TEST//TCP Relay 테스트용
//#define X2OPTIMIZE_UDP_PACKET_SEND_ME_VIA_NETWORK
//#define X2OPTIMIZE_ONE_SIMUL_ONE_RENDER_TEST
//#define X2OPTIMIZE_RENDER_BOUNDING_SPHERE_TEST
//#define X2OPTIMIZE_DAMAGE_EFFECT_TEST
//#define X2OPTIMIZE_VIEWDISTANCE_TEST
//#define X2OPTIMIZE_DYNAMICVB_DISCARDCOUNT_LOG




//vtune 전용
//#ifdef _VTUNE_
//#define MASS_FILE_FIRST
//#define X2OPTIMIZE_REFERENCE_RESOURCE_NEW_FOLDER_FOR_VTUNE
//#endif//_VTUNE_

#endif //_SERVICE_


//
// 기술지원팀 최적화 작업
// -----------------------------------------------------------------------------

//
// -----------------------------------------------------------------------------
// OpenTest 2번 서버 ---------------------------------------------------------------------
//




// -----------------------------------------------------------------------------

// InHouse ---------------------------------------------------------------------
#if X2_DEV_TOOLS

 	#include "InHouseEtc.h"	// 사내 툴 및 기타 기능들....
 	#include "InHouse1.h"	// 김종훈
 	#include "InHouse2.h"	// 김현철
 	#include "InHouse3.h"	// 김태환
 	#include "InHouse4.h"	// 강정훈
 	#include "InHouse5.h"	// 오현빈
	#include "InHouse6.h"	// 김창한



#endif // _IN_HOUSE_
// -----------------------------------------------------------------------------



// 해외팀 추가
#define XOR_KEY0 0xc6f8aa02
#define XOR_KEY1 0x2647abdc
#define XOR_KEY2 0x9800bbef


// System headers: Win32 through x2::platform, D3D9 through KTDXLIB's own device (decision T38,
// docs/porting/client-d3d9.md), DirectInput through SDL3. DirectSound, DirectShow, DirectDraw and DirectPlay are gone.
#include "x2/platform/net.h"
#include "x2/platform/win32.h"

#include <assert.h>
#include <wchar.h>
#include <math.h>
#include <limits.h>
#include <stdio.h>
#include <time.h>

#include "KSuperAssert.h"


	#define KTDXPROFILE()
	#define KTDXPROFILE2(name)
	#define KTDXPROFILE_BEGIN(name)
	#define KTDXPROFILE_END()

#include "../X2/KOGGamePerformanceCheckDefine.h"

// Direct3D: KTDXLIB's device and the D3D9 vocabulary, the D3DX math, and the D3DX interfaces the engine still names
#include "D3D9Types.h"
#include "Device.h"
#include "x2/platform/d3dx_math.h"
#include "D3DXTypes.h"
#include "DirectInputKeys.h"	// DirectInput is replaced by SDL3 (docs/porting/client-input.md)

#include <zlib.h>

#include "DXUT.h"  // the DXUT replacement (src/client/DXUT): window, loop, timer
#include "SdlInput.h"  // DXUTGetInputState(): the cursor
#include "x2/platform/process.h"



#include <vector>
using std::vector;
#include <stack>
using std::stack;
#include <queue>
using std::queue;
#include <list>
using std::list;
#include <map>
using std::map;
#include <unordered_map>
using std::unordered_map;
#include <string>
using std::wstring;
using std::string;
using std::wstringstream;
using std::stringstream;
#include <fstream>
#include <math.h>
#include <iostream>
using std::wcout;
using std::cout;
#include <set>
using std::set;
//{{ robobeg : 2008-10-10
#include <bitset>
#include    <deque>
#include <algorithm>
#include <type_traits>
//}} robobeg : 2008-10-10

#include <sys/stat.h>
#include <array>
#include <memory>
#include "x2/platform/noncopyable.h"
#include "x2/platform/intrusive_ptr.h"
#include "KCircularBuffer.h"
#include "KSingletonPool.h"

#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftbitmap.h>


// The CRT's debug assertion (crtdbg.h)
#ifndef _ASSERTE
#define _ASSERTE(expr) assert(expr)
#endif

#if defined(DEBUG) || defined(_DEBUG)

	// ASSERTwnx
	//
	#if !defined(ASSERT)
		#define ASSERT(exp)         _ASSERTE(exp)
	#endif // #if !defined(ASSERT)

	#ifndef V
		#define V(x)           { hr = x; if( FAILED(hr) ) { DXUTTrace( __FILE__, (DWORD)__LINE__, hr, L ## #x, true ); } }
	#endif
	#ifndef V_RETURN
		#define V_RETURN(x)    { hr = x; if( FAILED(hr) ) { return DXUTTrace( __FILE__, (DWORD)__LINE__, hr, L ## #x, true ); } }
	#endif


    #if !defined(TRACE)
        // To stderr (OutputDebugString in the original). Wide formats follow Windows (%s is wide).
        inline void _tracew(const WCHAR* fmt_, ...)
        {
            WCHAR   out[4096];
            va_list argList;

            va_start( argList, fmt_ );
            const std::wstring format = x2::platform::TranslateWideFormat( fmt_ );
            std::vswprintf( out, std::size( out ), format.c_str(), argList );
            va_end( argList );
            out[std::size( out ) - 1] = L'\0';
            std::fputs( x2::platform::WideToUtf8( out ).c_str(), stderr );
        }//_tracew()

        inline void _tracea(const char* pszFormat_, ...)
        {
            char    out[4096];
            va_list argList;

            va_start( argList, pszFormat_ );
            std::vsnprintf( out, sizeof( out ), pszFormat_, argList );
            va_end( argList );
            std::fputs( out, stderr );
        }//_tracea()
        #if !defined(TRACEA)
        #define TRACEA              _tracea
        #endif // #if !defined(TRACEA)
        #define TRACEW              _tracew

    #ifdef UNICODE // 2005-5-2, 11:40:58
    #define TRACE               _tracew
    #else
    #define TRACE               _tracea
    #endif // #ifdef UNICODE // 2005-5-2, 11:40:58
    #endif // #if !defined(TRACE)

#else

	#if !defined(ASSERT)
		#define ASSERT(exp)
	#endif // #if !defined(ASSERT)

	#ifndef V
		#define V(x)           { hr = x; }
	#endif
	#ifndef V_RETURN
		#define V_RETURN(x)    { hr = x; if( FAILED(hr) ) { return hr; } }
	#endif

    #if !defined(TRACE)
        //#define TRACE               1 ? (void)0 :
        /** The __noop intrinsic specifies that a function should be ignored and the argument list
            be parsed but no code be generated for the arguments. It is intended for use in global
            debug functions that take a variable number of arguments.
            - jintaeks on 2008-03-04, 10:00 */
        #define TRACE               __noop
        #if !defined(TRACEA)
        //#define TRACEA              1 ? (void)0 :
        #define TRACEA              __noop
        #endif // #if !defined(TRACEA)
        //#define TRACEW              1 ? (void)0 :
        #define TRACEW              __noop
    #endif // #if !defined(TRACE)

#endif


// Reference counting for x2::intrusive_ptr<CLASS>, found by argument-dependent lookup (namespace boost in the original)
#define IMPLEMENT_INTRUSIVE_PTR(CLASS) \
    inline void    intrusive_ptr_add_ref( CLASS* p ) { if ( p )    p->AddRef(); } \
    inline void    intrusive_ptr_release( CLASS* p ) { if ( p )    p->Release(); }



template <class Container>
inline typename Container::iterator EraseUnorderlyUsingSwap( Container& container, typename Container::iterator iter )
{
	typename Container::iterator iterLast = container.end();
	iterLast--;
	if ( iterLast == iter )
	{
		return	container.erase( iter );
	}//if
	swap( *iter, *iterLast );
	typename Container::size_type dist = iter - container.begin();
	container.erase( iterLast );
	return	container.begin() + dist;
}


//{{ seojt // 2008-10-15, 10:27
//------------------------------------------------------------------------------
// CRT debug macros
//
#ifndef INIT_CRTDEBUG
/** 메모리 할당과 해제 요청이 있을 때마다 모든 메모리를 검사해서
    overwrite나 underwrite를 발견하고 싶으면 flag_에

        INIT_CRTDEBUG(_CRTDBG_CHECK_ALWAYS_DF);

    를 명시한다.
    일반적으로

        INIT_CRTDEBUG(0)

    처럼 사용한다.
    - jintaeks on 2008-03-20, 10:36 */
#define INIT_CRTDEBUG(flag_)    ::_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | flag_)
#endif // #ifndef INIT_CRTDEBUG


/** @example    ASSERT_MEMORY()
	@code

    #include "KJohnGen.h"
    #include <iostream>

    void main()
    {
        INIT_CRTDEBUG(0);

        int*    p = new int;
        delete p;

        *p = 3;             // using after deletion
        ASSERT_MEMORY();    // see and check call stack window
        delete p;
    }//main()

	@endcode
*/
// ASSERT_MEMORY()가 제대로 동작하기 위해서는 먼저 INIT_CRTDEBUG()가 호출된 상태여야 한다.
#ifndef ASSERT_MEMORY
#define ASSERT_MEMORY()     ASSERT( ::_CrtCheckMemory() )
#endif // #ifndef ASSERT_MEMORY


/** @example    BREAK_ALLOC()
	@code

  //------------------------------------------------------------------------------
  //
  1) memory leak example:

    #include "KJohnGen.h"
    #include <iostream>

    void main()
    {
        INIT_CRTDEBUG(0);

        int*    p = new int;

        *p = 3;
    }//main()

    // debug message:
    Detected memory leaks!
    Dumping objects ->
    {44} normal block at 0x00372DA8, 4 bytes long.
    Data: <    > 03 00 00 00
    Object dump complete.

    // @note    allocation number is {44}, so you can set break point at 44th alloc
    //          see below example


  //------------------------------------------------------------------------------
  //
  2) break on mal-alloc {44}:

    #ifdef PROJECT_PCH
    #define _TOSTR1(x)    #x
    #define _TOSTR(x)     _TOSTR1(x)
    #include _TOSTR(PROJECT_PCH)
    #endif // PROJECT_PCH
    #include "KJohnGen.h"
    #include <iostream>

    void main()
    {
        BREAK_ALLOC( 44 ); // install break CRT point condition
        INIT_CRTDEBUG(0);

        int*    p = new int;

        *p = 3;
    }//main()

	@endcode
*/
// INIT_CRTDEBUG()가 보고한 메모리 블록에 break point를 건다.
#ifndef BREAK_ALLOC
#define BREAK_ALLOC(line_)   ::_CrtSetBreakAlloc( line_ )
#endif // #ifndef BREAK_ALLOC
//}} seojt // 2008-10-15, 10:27






#ifndef DMLEE_NOTE
		#define DMLEE_NOTE(msg_) message( "" )
#endif // DMLEE_NOTE



#define ARRAY_SIZE(a)       (sizeof(a)/sizeof((a)[0]))

#ifndef SAFE_CALL
#define SAFE_CALL(p)			if(p) p
#endif
#ifndef SAFE_DELETE
#define SAFE_DELETE(p)			{ if(p) { delete (p);     (p)=NULL; } }
#endif
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p)	{ if(p) { delete[] (p);   (p)=NULL; } }
#endif
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)			{ if(p) { (p)->Release(); (p)=NULL; } }
#endif
#ifndef SAFE_CLOSE
#define SAFE_CLOSE(p)			{ if(p) { g_pKTDXApp->GetDeviceManager()->CloseDevice( (p)->GetDeviceID() );     (p)=NULL; } }
#endif

//{{ robobeg : 2008-01-05

#ifndef SAFE_FREE
#define SAFE_FREE(p)        { if(p) { ::free(p); (p)=NULL; } }
#endif // #ifndef SAFE_FREE

#ifndef SAFE_REALLOC
#define SAFE_REALLOC(p,s)   \
{   if((s)==0){ SAFE_FREE(p); }   \
else if (!(p))    { (p) = ::malloc(s); ASSERT(p); } \
else {  (p) = ::realloc((p),(s)); ASSERT(p); } }
#endif // #ifndef SAFE_REALLOC

#ifndef SAFE_TYPEDREALLOC
#define SAFE_TYPEDREALLOC(type,p,s)   \
{   if((s)==0){ SAFE_FREE(p); }   \
else if (!(p))    { (p) = (type*) ::malloc((s) * sizeof(type)); ASSERT(p); } \
else {  (p) = (type*) ::realloc((p), (s) * sizeof(type)); ASSERT(p); } }
#endif // #ifndef SAFE_TYPEDREALLOC

//}} robobeg : 2008-01-05

#ifndef SAFE_DELETE_DIALOG
//{{ robobeg : 2009-01-07
//#define SAFE_DELETE_DIALOG(p)	{ DialogLog("SAFE_DELETE_DIALOG"); if(p) { g_pKTDXApp->GetDGManager()->GetDialogManager()->DeleteDlg( (p) );		 (p)=NULL; } }
//#define SAFE_DELETE_DIALOG(p)   { DialogLog("SAFE_DELETE_DIALOG"); if ( p ) { bool bRet = g_pKTDXApp->GetDGManager()->GetDialogManager()->DeleteDlg(p); ASSERT( bRet ); (p)=NULL; } }
#define SAFE_DELETE_DIALOG(p)   { DialogLog("SAFE_DELETE_DIALOG"); if ( p != NULL ) { bool bRet = g_pKTDXApp->GetDGManager()->GetDialogManager()->DeleteDlg(p); ASSERT( bRet ); (p)=NULL; } }
//}} robobeg : 2009-01-07
#endif

//{{ seojt // 2009-8-19, 17:21
#ifndef SAFE_DELETE_DIALOG_HANDLE
#define SAFE_DELETE_DIALOG_HANDLE(p)    { CKTDGUIDialog::Destroy(p.GetHandle()); p.Invalidate(); }
#endif // SAFE_DELETE_DIALOG_HANDLE
//}} seojt // 2009-8-19, 17:21

//{{ seojt // 2009-1-13, 15:44
#ifndef SAFE_DELETE_KTDGOBJECT
#define SAFE_DELETE_KTDGOBJECT(p)   { if(p) { CKTDGObject::DeleteKTDGObject(p); (p)=NULL;} }
#endif // SAFE_DELETE_KTDGOBJECT
//}} seojt // 2009-1-13, 15:44


//{{ seojt // 2009-1-13, 15:44
#ifndef SHARED_POINTER_CAST
#define SHARED_POINTER_CAST(TARGET_TYPE,sharedPointer)     std::static_pointer_cast<TARGET_TYPE>( sharedPointer )
#endif // SHARED_POINTER_CAST()
//}} seojt // 2009-1-13, 15:44


//#ifndef SAFE_DELETE_XMESH_PLAYER_INSTANCE
//#define SAFE_DELETE_XMESH_PLAYER_INSTANCE(xmesh,p)	{ if(p) { (xmesh)->DestroyInstance( (p) );					 (p)=NULL; } }
//#endif
#ifndef CREATE_VECTOR3
#define CREATE_VECTOR3(v,a,b,c)	{ D3DXVECTOR3 (v); (v).x = (a); (v).y = (b); (v).z = (c); }
#endif
#ifndef INIT_VECTOR3
#define INIT_VECTOR3(v,a,b,c)	{ (v).x = (a); (v).y = (b); (v).z = (c); }
#endif
#ifndef CREATE_VECTOR2
#define CREATE_VECTOR2(v,a,b)	{ D3DXVECTOR2 (v); (v).x = (a); (v).y = (b); }
#endif
#ifndef INIT_VECTOR2
#define INIT_VECTOR2(v,a,b)		{ (v).x = (a); (v).y = (b); }
#endif
#ifndef INIT_COLOR
#define INIT_COLOR(v,red,green,blue,alpha)		{ (v).r = (red); (v).g = (green); (v).b = (blue); (v).a = (alpha); }
#endif

#define SETVECTOR4(vector,_x,_y,_z,_w) {vector.x = _x; vector.y = _y; vector.z = _z; vector.w = _w;}
#define SETVECTOR3(vector,_x,_y,_z) {vector.x = _x; vector.y = _y; vector.z = _z;}
#define SETVECTOR2(vector,_x,_y) {vector.x = _x; vector.y = _y;}





static const D3DXVECTOR3 Y_AXIS( 0.f, 1.f, 0.f);





struct TimeLimited
{
	float m_fTimeLeft;

	virtual void OnTimeOutCallBack() {};

	virtual void OnFrameMove( double /*fTime*/, float fElapsedTime )
	{
		if( m_fTimeLeft > 0.f )
		{
			m_fTimeLeft -= fElapsedTime;

			if( m_fTimeLeft <= 0.f )
			{
				m_fTimeLeft = 0.f;
				OnTimeOutCallBack();
			}
		}
	}


};

struct KTDGSphere
{
	D3DXVECTOR3	center;
	float		fRadius;
};

extern double g_NowTime;




#ifdef APPLY_THEMIDA
#	include "../X2Lib/ThemidaSDK.h"
#endif


#include "EnumToString.h"
#include <KNCSingleton.h>
#include "KTDXRandomNumbers.h"
#include "KTDXStringTable.h"
#include "KVersionControl.h"

#include "KSystemTimer.h"

#include "KTDXSimpleHandleInterface.h"
#include "KTDXLog.h"
#include "debug.h"


#include "KTDXErrorDefine.h"
#include "KGCMassFileBufferManager.h"
#include "KGCMassFileManager.h"
#include "CRC_32.h"
#include "KTDXCommonFunc.h"
#include "KTDXRecyclingArray.h"
//{{ robobeg : 2013-12-12
//#include "../luaLib/KLuaManager.h"
//#include "../luaLib/KLuaBinder.h"
#include "../LuaCommon/lua_tinker.h"
#include "../LuaCommon/KLuaManager.h"
#include "../LuaCommon/KLuabinder.h"
#include "KLuaManagerProxy.h"
//}} robobeg : 2013-12-12



#include "delegate.hpp"

#include "KProtectRefTable.h"
#include "KProtectedType.h"
#include "KBrowserWrapper.h"	// the system browser, in place of the embedded KPCHWebBrowser
#include "KTDXTimer.h"
#include "KTDXThread.h"


#include "KTDXDeviceHolder.h"
#include "KTDXStage.h"

//#ifdef DYNAMIC_VERTEX_BUFFER_OPT
#include "KTDGDynamicVBManager.h"
//#endif

//{{ dmlee 2008.05.14 checksum 검사를 별도 스레드에서 하도록 하는 코드 테스트 중
//#include "KStringUtil.h"
#include "CSLock.h"
#include "KJohnThread.h"

//}} dmlee 2008.05.14 checksum 검사를 별도 스레드에서

#if X2_DEV_TOOLS // 2008-10-23
#include "KHelpSystem.h"
#endif // KHELP_SYSTEM_TEST // 2008-10-23

//{{ robobeg : 2008-01-05
#include "KFileSerializer.h"
//}} robobeg : 2008-01-05

//{{ seojt // 2009-4-3, 11:53
/** CKTDGUIDialog에서 crash 발생으로, handle을 사용하는 방식으로 바꾸는 작업을 시작하다.
    : jintaeks on 2009-04-03, 11:54
*/
#include "KJohnObjMan.h"
#include "KHandle.h"
//}} seojt // 2009-4-3, 11:53

#include "KHangulPPWSelector.h"
#include "MinMax.h"

#ifdef CONVERSION_VS
#if defined(DEBUG) || defined(_DEBUG)
#else
#endif
#endif // CONVERSION_VS



#include "KTDXCollision.h"


class CKTDGObject;


//#include "KTDXXET.h"

#include "KSmartNull.h"
#include "KObserverPtr.h"

// #ifdef MOVIE_TEST	 	// 13-11-11, 진입 구조 개편, kimjh, MOVIE_TEST 중 사용에 필요한 Define 을 MOVIE_TEST_BASE 로 변경

#include "MoviePlayer.h"	// x2::client_movie, in place of BandiVideo (docs/porting/client-movie.md)

// #endif //  MOVIE_TEST	// 13-11-11, 진입 구조 개편, kimjh, MOVIE_TEST 중 사용에 필요한 Define 을 MOVIE_TEST_BASE 로 변경

#include "KTDXDevice.h"
#include "KTDXDeviceBaseTexture.h"
#include "KTDXDeviceTET_Preprocessing.h"
#include "KTDXDeviceXET_Preprocessing.h"
#include "KTDXDeviceTexture.h"
#include "KTDXDeviceXET.h"
#include "KTDXDeviceXMesh.h"
#include "KTDXDeviceXSkinMesh.h"

#include "KTDXDeviceRenderTargetTexture.h"
#include "KTDXDeviceSound.h"
#include "KTDXDeviceManager.h"

#include "KTDXEnumToLua.h"


#include "KTDGManager.h"
#include "KTDIManager.h"
#include "KTDSManager.h"
#ifdef CONVERSION_VS
#include "KTDNPacket.h"
#include "KTDNUDPStatisticData.h"
#include "KTDNUDP.h"

#else
#include "KTDNManager.h"
#endif // CONVERSION_VS

//{{ robobeg : 2008-01-05
#include "KTDXAnimationController.h"
//}} robobeg : 2008-01-05

#include "KExceptionSafeUtil.h"



#include "KTDXApp.h"

extern CKTDXApp* g_pKTDXApp;

#endif
