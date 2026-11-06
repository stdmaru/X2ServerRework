#pragma once

// Replacement for boost::lexical_cast from a string to a number: the whole string must be the number, with no
// whitespace around it (as Boost), in the "C" locale. Returns false instead of throwing bad_lexical_cast.

#include <ios>
#include <locale>
#include <sstream>
#include <string>

template <class T>
bool KLexicalCast( const std::string& strIn_, T& Out_ )
{
    std::istringstream stream( strIn_ );
    stream.imbue( std::locale::classic() );
    T value{};
    stream >> std::noskipws >> value;
    if ( stream.fail() || stream.peek() != std::char_traits<char>::eof() )
        return false;
    Out_ = value;
    return true;
}
