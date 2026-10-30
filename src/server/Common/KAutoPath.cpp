#include "./KAutoPath.h"

#include "x2/platform/unicode.h"

#include <algorithm>
#include <filesystem>

namespace fs = std::filesystem;

//{{ 2009-07-13 removed hard-coded paths
static std::vector< std::string > g_vecAddPath; // file scope: no outside access

void KAutoPath::AddPath( const std::string& strPath )
{
#if defined(_WIN32)
	g_vecAddPath.push_back( strPath );
#else
	// the configs were written for Windows (C:\Elsword\ServerResource): outside it, '\' would be part of a name
	std::string strPortable = strPath;
	std::replace( strPortable.begin(), strPortable.end(), '\\', '/' );
	g_vecAddPath.push_back( strPortable );
#endif
}
//}}

KAutoPath::KAutoPath()
{
	// the working folder is always searched first
	std::error_code ec;
	const std::string strCurDir = x2::platform::WideToUtf8( fs::current_path( ec ).wstring() );
	m_vecDirectory.push_back( strCurDir );
	InitDirectory( strCurDir );

	// then the extra folders and their subfolders
	for( const std::string& strAddDir : g_vecAddPath )
	{
		m_vecDirectory.push_back( strAddDir );
		InitDirectory( strAddDir );
	}
}

KAutoPath::~KAutoPath(void)
{
	m_vecDirectory.clear();
}

bool KAutoPath::GetPullPath( std::string& strFileName )
{
	// first folder (in search order) that has the file
	for( const std::string& strDir : m_vecDirectory )
	{
		const fs::path path = fs::path( x2::platform::Utf8ToWide( strDir ) ) / x2::platform::Utf8ToWide( strFileName );
		std::error_code ec;
		if( fs::is_regular_file( path, ec ) )
		{
			strFileName = x2::platform::WideToUtf8( path.wstring() );
			return true;
		}
	}
	return false;
}

bool KAutoPath::GetPullPath( std::wstring& wstrFileName )
{
	std::string strFileName = x2::platform::WideToUtf8( wstrFileName );
	if( !GetPullPath( strFileName ) )
		return false;

	wstrFileName = x2::platform::Utf8ToWide( strFileName );
	return true;
}

// Adds every subfolder of strCurDir, depth first, in name order (the order FindFirstFile gives on NTFS).
void KAutoPath::InitDirectory( std::string strCurDir )
{
	std::error_code ec;
	std::vector< fs::path > vecSubDirs;
	for( const auto& entry : fs::directory_iterator( fs::path( x2::platform::Utf8ToWide( strCurDir ) ), ec ) )
	{
		if( entry.is_directory( ec ) )
			vecSubDirs.push_back( entry.path() );
	}
	std::sort( vecSubDirs.begin(), vecSubDirs.end() );

	for( const fs::path& subDir : vecSubDirs )
	{
		const std::string strDir = x2::platform::WideToUtf8( subDir.wstring() );
		m_vecDirectory.push_back( strDir );
		InitDirectory( strDir );
	}
}
