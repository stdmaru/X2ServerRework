#pragma once

#include "KStringVarArg.h"

class CKTDXStringLoader
{
	public:
		CKTDXStringLoader( void );
		~CKTDXStringLoader( void );

	public:
		bool Load( const WCHAR* pFileName );
		const std::wstring& GetString( int ID );
		template <class... Args>
		std::wstring GetReplacedString( int ID, const char* szParamList, const Args&... args )
		{
			return GetReplacedStringV( ID, szParamList, KStringVarArg::Arg( args )... );
		}
		std::wstring GetReplacedStringV( int ID, const char* szParamList, ... );

	private:
		std::vector<std::wstring> m_vecString;
};
