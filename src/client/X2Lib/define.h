



#if !X2_DEV_TOOLS
#	define PATCH_ORG_SRC		"http://gamepatch.elswordonline.com/"	// 본섭
#	define PATCH_PATH_FILE		"PatchPath.dat"     // 본섭
#else
#	define PATCH_TEST_SRC		"http://192.168.71.240/"                  // 국내사내버전
#	define PATCH_PATH_FILE		"PatchPath_.dat"     // 테스트
#endif



#define MAX_RETRY           64                          // 최대 재시도 횟수
#define MAX_DELAY           128                         // 재시도 지연 간격


#define LOAD_CUR_FILE_BAR_X			72
#define LOAD_CUR_FILE_BAR_Y			285
#define LOAD_CUR_FILE_BAR_WIDTH		373 - 5
#define LOAD_CUR_FILE_BAR_HEIGHT	473 - 462

#define LOAD_CUR_FILE_PER_X			449
#define LOAD_CUR_FILE_PER_Y			285
#define LOAD_CUR_FILE_PER_WIDTH		60
#define LOAD_CUR_FILE_PER_HEIGHT	30

#define LOAD_ALL_FILE_BAR_X			72
#define LOAD_ALL_FILE_BAR_Y			305
#define LOAD_ALL_FILE_BAR_WIDTH		373 - 5
#define LOAD_ALL_FILE_BAR_HEIGHT	473 - 462

#define LOAD_ALL_FILE_PER_X			449
#define LOAD_ALL_FILE_PER_Y			305
#define LOAD_ALL_FILE_PER_WIDTH		60
#define LOAD_ALL_FILE_PER_HEIGHT	30
