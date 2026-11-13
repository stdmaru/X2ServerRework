#include "stdafx.h"
#include "KTDGUIAnchor.h"

#include <algorithm>
#include <cctype>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cwctype>
#include <fstream>
#include <map>
#include <set>
#include <sstream>

namespace
{
	// Dialog scripts are named without a folder, in any case.
	std::wstring ScriptKey( const wchar_t* pwszName )
	{
		std::wstring wstrKey = pwszName != NULL ? pwszName : L"";
		const size_t uSlash = wstrKey.find_last_of( L"/\\" );
		if ( uSlash != std::wstring::npos )
			wstrKey.erase( 0, uSlash + 1 );
		for ( wchar_t& c : wstrKey )
			c = static_cast<wchar_t>( std::towlower( static_cast<wint_t>( c ) ) );
		return wstrKey;
	}

	std::wstring ToWide( const char* pszUtf8 )
	{
		std::wstring wstr;
		ConvertUtf8ToWCHAR( wstr, pszUtf8 );
		return wstr;
	}
}

bool ParseUIAnchor( const char* pszName, UIAnchor& eAnchor )
{
	if ( pszName == NULL )
		return false;
	static const struct { const char* pszName; UIAnchor eAnchor; } s_akNames[] =
	{
		{ "CENTER", UIAnchor::Center }, { "LEFT", UIAnchor::Left }, { "RIGHT", UIAnchor::Right }, { "STRETCH", UIAnchor::Stretch },
	};
	for ( const auto& kName : s_akNames )
	{
		const char* a = pszName;
		const char* b = kName.pszName;
		while ( *a != 0 && std::toupper( static_cast<unsigned char>( *a ) ) == *b )
		{
			++a;
			++b;
		}
		if ( *a == 0 && *b == 0 )
		{
			eAnchor = kName.eAnchor;
			return true;
		}
	}
	return false;
}

const std::unordered_map<std::wstring, CKTDGUIAnchors::Entry>& CKTDGUIAnchors::Get()
{
	static const std::unordered_map<std::wstring, Entry> s_mapEntries = Load();
	return s_mapEntries;
}

std::unordered_map<std::wstring, CKTDGUIAnchors::Entry> CKTDGUIAnchors::Load()
{
	std::unordered_map<std::wstring, Entry> mapEntries;
	lua_State* L = g_pKTDXApp->GetLuaBinder()->GetLuaState();

	if ( g_pKTDXApp->LoadLuaTinker( L"UIAnchors.lua" ) == false )
	{
		std::fprintf( stderr, "UIAnchors.lua not found: every dialog centered\n" );
		return mapEntries;
	}

	lua_getglobal( L, "UI_ANCHORS" );
	if ( lua_istable( L, -1 ) )
	{
		lua_pushnil( L );
		while ( lua_next( L, -2 ) != 0 )	// script name -> { DIALOG = ..., CONTROLS = { name = ... } }
		{
			if ( lua_type( L, -2 ) == LUA_TSTRING && lua_istable( L, -1 ) )
			{
				const char* pszScript = lua_tostring( L, -2 );
				const std::wstring wstrScript = ScriptKey( ToWide( pszScript ).c_str() );
				Entry& kEntry = mapEntries[wstrScript];

				lua_getfield( L, -1, "DIALOG" );
				if ( lua_type( L, -1 ) == LUA_TSTRING )
				{
					kEntry.bHasDialog = ParseUIAnchor( lua_tostring( L, -1 ), kEntry.eDialog );
					if ( kEntry.bHasDialog == false )
						std::fprintf( stderr, "UIAnchors.lua: unknown anchor for %s\n", pszScript );
				}
				lua_pop( L, 1 );

				lua_getfield( L, -1, "CONTROLS" );
				if ( lua_istable( L, -1 ) )
				{
					lua_pushnil( L );
					while ( lua_next( L, -2 ) != 0 )
					{
						UIAnchor eAnchor = UIAnchor::Center;
						if ( lua_type( L, -2 ) == LUA_TSTRING && lua_type( L, -1 ) == LUA_TSTRING
							&& ParseUIAnchor( lua_tostring( L, -1 ), eAnchor ) )
							kEntry.mapControls[ToWide( lua_tostring( L, -2 ) )] = eAnchor;
						else
							std::fprintf( stderr, "UIAnchors.lua: bad control anchor in %s\n", pszScript );
						lua_pop( L, 1 );
					}
				}
				lua_pop( L, 1 );
			}
			lua_pop( L, 1 );
		}
	}
	lua_pop( L, 1 );

	lua_pushnil( L );
	lua_setglobal( L, "UI_ANCHORS" );
	return mapEntries;
}

void CKTDGUIAnchors::Apply( CKTDGUIDialog& kDialog, const wchar_t* pwszScriptName )
{
	const auto& mapEntries = Get();
	const auto it = mapEntries.find( ScriptKey( pwszScriptName ) );
	if ( it == mapEntries.end() )
		return;

	const Entry& kEntry = it->second;
	if ( kEntry.bHasDialog && kDialog.IsAnchorDeclared() == false )
		kDialog.SetAnchor( kEntry.eDialog );

	for ( const auto& [wstrControl, eAnchor] : kEntry.mapControls )
	{
		CKTDGUIControl* pControl = kDialog.GetControl( wstrControl.c_str() );
		if ( pControl == NULL )
		{
			std::string strControl, strScript;
			ConvertWCHARToChar( strControl, wstrControl );
			ConvertWCHARToChar( strScript, it->first );
			std::fprintf( stderr, "UIAnchors.lua: no control %s in %s\n", strControl.c_str(), strScript.c_str() );
		}
		else if ( pControl->HasAnchor() == false )
			pControl->SetAnchor( eAnchor );
	}
}

#if X2_DEV_TOOLS
namespace
{
	std::string ToUtf8( const std::wstring& wstr )
	{
		if ( wstr.empty() )
			return std::string();
		const int iSize = ::WideCharToMultiByte( CP_UTF8, 0, wstr.c_str(), static_cast<int>( wstr.size() ), NULL, 0, NULL, NULL );
		std::string str( std::max( iSize, 0 ), '\0' );
		if ( iSize > 0 )
			::WideCharToMultiByte( CP_UTF8, 0, wstr.c_str(), static_cast<int>( wstr.size() ), str.data(), iSize, NULL, NULL );
		return str;
	}

	// One control of one dialog script: what it drew, relative to the dialog, and where the dialog was.
	struct AnchorRecord
	{
		int iType = 0;
		float fMinX = 0.f, fMinY = 0.f, fMaxX = 0.f, fMaxY = 0.f;
		std::set<std::pair<int, int>> setDialogPos;
		bool bPlacedByCode = false;
	};

	class AnchorRecordFile
	{
	public:
		static constexpr const char* kFileName = "UIAnchorsRecord.tsv";
		static constexpr size_t kMaxPositions = 16;

		AnchorRecordFile() { Read(); }

		void Note( const std::string& strScript, const std::string& strControl, int iType, const D3DXVECTOR2& vMin,
			const D3DXVECTOR2& vMax, const D3DXVECTOR2& vDialogPos, bool bPlacedByCode )
		{
			const std::string strKey = strScript + '\t' + strControl;
			auto [it, bNew] = m_mapRecords.try_emplace( strKey );
			AnchorRecord& kRecord = it->second;
			const std::pair<int, int> kPos( static_cast<int>( std::lround( vDialogPos.x ) ), static_cast<int>( std::lround( vDialogPos.y ) ) );
			if ( bNew )
			{
				kRecord.iType = iType;
				kRecord.fMinX = vMin.x;
				kRecord.fMinY = vMin.y;
				kRecord.fMaxX = vMax.x;
				kRecord.fMaxY = vMax.y;
				m_bDirty = true;
			}
			else if ( vMin.x < kRecord.fMinX || vMin.y < kRecord.fMinY || vMax.x > kRecord.fMaxX || vMax.y > kRecord.fMaxY )
			{
				kRecord.fMinX = std::min( kRecord.fMinX, vMin.x );
				kRecord.fMinY = std::min( kRecord.fMinY, vMin.y );
				kRecord.fMaxX = std::max( kRecord.fMaxX, vMax.x );
				kRecord.fMaxY = std::max( kRecord.fMaxY, vMax.y );
				m_bDirty = true;
			}
			if ( bPlacedByCode && kRecord.bPlacedByCode == false )
			{
				kRecord.bPlacedByCode = true;
				m_bDirty = true;
			}
			if ( kRecord.setDialogPos.size() < kMaxPositions && kRecord.setDialogPos.insert( kPos ).second )
				m_bDirty = true;
		}

		void FlushIfDue()
		{
			const auto tNow = std::chrono::steady_clock::now();
			if ( m_bDirty == false || tNow - m_tLastWrite < std::chrono::seconds( 2 ) )
				return;
			m_tLastWrite = tNow;
			m_bDirty = false;

			std::ofstream kFile( kFileName, std::ios::trunc );
			kFile << "# script\tcontrol\ttype\tmin_x\tmin_y\tmax_x\tmax_y\tdialog_positions\tplaced_by_code (layout units; control box relative to the dialog)\n";
			for ( const auto& [strKey, kRecord] : m_mapRecords )
			{
				kFile << strKey << '\t' << kRecord.iType << '\t' << kRecord.fMinX << '\t' << kRecord.fMinY << '\t'
					<< kRecord.fMaxX << '\t' << kRecord.fMaxY << '\t';
				bool bFirst = true;
				for ( const auto& [x, y] : kRecord.setDialogPos )
				{
					kFile << ( bFirst ? "" : ";" ) << x << ',' << y;
					bFirst = false;
				}
				kFile << '\t' << ( kRecord.bPlacedByCode ? 1 : 0 ) << '\n';
			}
		}

	private:
		void Read()
		{
			std::ifstream kFile( kFileName );
			std::string strLine;
			while ( std::getline( kFile, strLine ) )
			{
				if ( strLine.empty() || strLine[0] == '#' )
					continue;
				std::istringstream kLine( strLine );
				std::string strScript, strControl, strPositions;
				AnchorRecord kRecord;
				if ( !std::getline( kLine, strScript, '\t' ) || !std::getline( kLine, strControl, '\t' )
					|| !( kLine >> kRecord.iType >> kRecord.fMinX >> kRecord.fMinY >> kRecord.fMaxX >> kRecord.fMaxY ) )
					continue;
				kLine >> std::ws;
				std::getline( kLine, strPositions, '\t' );
				int iPlacedByCode = 0;
				if ( kLine >> iPlacedByCode )
					kRecord.bPlacedByCode = iPlacedByCode != 0;
				std::istringstream kPositions( strPositions );
				std::string strPos;
				while ( std::getline( kPositions, strPos, ';' ) )
				{
					int x = 0, y = 0;
					if ( std::sscanf( strPos.c_str(), "%d,%d", &x, &y ) == 2 )
						kRecord.setDialogPos.emplace( x, y );
				}
				m_mapRecords[strScript + '\t' + strControl] = std::move( kRecord );
			}
		}

		std::map<std::string, AnchorRecord> m_mapRecords;
		bool m_bDirty = false;
		std::chrono::steady_clock::time_point m_tLastWrite{};
	};

	AnchorRecordFile& RecordFile()
	{
		static AnchorRecordFile s_kFile;
		return s_kFile;
	}
}

bool CKTDGUIAnchorRecorder::IsOn()
{
	static const bool s_bOn = []
	{
		const char* pszValue = std::getenv( "X2_RECORD_UI_ANCHORS" );
		return pszValue != NULL && pszValue[0] != '\0' && pszValue[0] != '0';
	}();
	return s_bOn;
}

void CKTDGUIAnchorRecorder::Note( CKTDGUIDialog& kDialog )
{
	const std::wstring wstrScript = kDialog.GetScriptFileName();
	if ( wstrScript.empty() )
		return;	// built in code: nothing to declare anchors for
	const std::string strScript = ToUtf8( ScriptKey( wstrScript.c_str() ) );

	AnchorRecordFile& kFile = RecordFile();
	const std::vector<CKTDGUIControl*>& vecControls = kDialog.GetControlList();
	for ( size_t i = 0; i < vecControls.size(); ++i )
	{
		CKTDGUIControl* pControl = vecControls[i];
		D3DXVECTOR2 vMin, vMax;
		if ( pControl == NULL || pControl->GetShow() == false || pControl->GetDrawnBounds( vMin, vMax ) == false )
			continue;
		const std::wstring wstrName = pControl->GetName() != NULL ? pControl->GetName() : L"";
		const std::string strControl = wstrName.empty() ? "#" + std::to_string( i ) : ToUtf8( wstrName );
		kFile.Note( strScript, strControl, static_cast<int>( pControl->GetType() ), vMin, vMax, kDialog.GetPos(), kDialog.GetPlacedByCode() );
	}
	kFile.FlushIfDue();
}
#endif
