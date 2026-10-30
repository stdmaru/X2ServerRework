// What each server defines for the Common, so the tests link without a server:
// - globals and factories of the server's main module (g_szConfigFile, CreateDBThread);
// - KStatisticsLocalLog, compiled with the GameServer (it includes GameServer/CenterServer headers).
// Nothing here runs in the tests.
#include "KStatisticsLocalLog.h"
#include <Thread/Thread.h>

const wchar_t* g_szConfigFile = L"";
KThread* CreateDBThread( int, const wchar_t*, bool ) { return nullptr; }

KStatisticsLocalLog::KStatisticsLocalLog() {}
KStatisticsLocalLog::~KStatisticsLocalLog() {}
namespace { std::wfstream s_wfsNull; }
std::wfstream& KStatisticsLocalLog::LocalLogStm( KLocalLog ) { return s_wfsNull; }
std::wfstream& KStatisticsLocalLog::LocalLogStm_Henir_Ranking( int ) { return s_wfsNull; }
void KStatisticsLocalLog::FlushLocalLogStm() {}
void KStatisticsLocalLog::CloseLocalLogData() {}
void KStatisticsLocalLog::ClearLocalLog() {}
void KStatisticsLocalLog::CloseHenirRankingLogData( int ) {}
void KStatisticsLocalLog::DeleteHenirRankingLog() {}
