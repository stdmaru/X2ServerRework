#include "stdafx.h"
#include "KTDGUIAnchor.h"

#include <cctype>
#include <cstdio>
#include <cwctype>

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
