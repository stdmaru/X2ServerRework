// What each server defines for the Common, so the tests link without a server:
// - globals and factories of the server's main module (g_szConfigFile, CreateDBThread);
// - KStatisticsLocalLog, compiled with the GameServer (it includes GameServer/CenterServer headers);
// - KODBC, until ODBC/ is ported (server/common-db).
// Nothing here runs in the tests.
#include "KStatisticsLocalLog.h"
#include "ODBC/Odbc.h"
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

KODBC::KODBC() {}
KODBC::~KODBC() {}
bool KODBC::DriverConnect( const wchar_t*, bool ) { return false; }
bool KODBC::Query( const std::wstring& ) { return false; }
bool KODBC::Fetch() { return false; }
KODBC& KODBC::operator >>( int& ) { return *this; }
KODBC& KODBC::operator >>( std::wstring& ) { return *this; }
void KODBC::DumpMaxTime( std::wostream& ) {}
void KODBC::ResetMaxTime() {}
ImplToStringW( KODBC ) { return stm_; }
