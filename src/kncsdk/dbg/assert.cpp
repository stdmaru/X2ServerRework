//////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright Triple Buffer Software 2003 All rights reserved                                        //
//                                                                                                  //
// This is free sour code. You may check the terms and conditions for free source code usage at:    //
//                                                                                                  //
// http://triplebuffer.devmaster.net/license.php                                                    //
//////////////////////////////////////////////////////////////////////////////////////////////////////

#include "assert.hpp"

#include <cassert>
#include <cstdlib>

#undef m_custom_assert_a
#undef m_custom_assert_b
#undef cassert

namespace dbg
{
    custom_assert cassert;

    custom_assert::custom_assert()
        : m_custom_assert_a(*this), m_custom_assert_b(*this)
    {
    }

    custom_assert& custom_assert::operator () ( const wchar_t* expr, unsigned int line, const wchar_t* func, const wchar_t* file )
    {
        m_expression = expr;
        m_filename = file;
        m_line = line;

        cerr << L"Assert failure in ";
        cerr.line_file_func( line, file, func ) << L". Expr is \"" << expr << L"\"" << endl;

        return *this;
    }

    custom_assert& custom_assert::operator () ( const char* expr, unsigned int line, const wchar_t* func, const wchar_t* file )
    {
        const std::wstring wexpr = KncUtil::toWideString( expr );
        return ( *this )( wexpr.c_str(), line, func, file );
    }

    custom_assert& custom_assert::msg( const wchar_t* msg )
    {
        cerr << tab << L"[Msg] " << msg << endl;
        return *this;
    }

    custom_assert& custom_assert::msg( const char* msg )
    {
        cerr << tab << L"[Msg] " << msg << endl;
        return *this;
    }

    void custom_assert::fatal()
    {
        cerr << L"F A T A L" << endl;
#ifndef NDEBUG
        std::abort();
#endif
    }

} // dbg
