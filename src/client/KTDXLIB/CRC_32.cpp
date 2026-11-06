#include <stdafx.h>
#include <fstream>
#include <iomanip>
#include <iterator>
#include <sstream>
#include <vector>

CRC_32& CRC_32::GetInstance( void )
{
    static CRC_32 obj;
    return obj;
}

std::string CRC_32::GetCRCFromBuffer( const void* pvBuf, UINT uiSize )
{
    DWORD dwCRCCode = CRC.CalcCRC( pvBuf, uiSize );

    if ( 0 == dwCRCCode )
    {
        return std::string(); // failed
    }

    std::ostringstream ostrStream;
    ostrStream << std::resetiosflags( std::ios::uppercase )
               << std::hex << std::setw( 8 ) << std::setfill( '0' )
               << dwCRCCode;

    return ostrStream.str(); // succeeded
}

std::string CRC_32::GetCRCFromFile( const char* szFilePath )
{
    // Port: read with a file stream (the original mapped the file into memory). An empty file still fails, as
    // mapping one did.
    std::ifstream file( szFilePath, std::ios::binary );
    if ( !file )
        return std::string(); // failed
    const std::vector<char> vecData( ( std::istreambuf_iterator<char>( file ) ), std::istreambuf_iterator<char>() );
    if ( vecData.empty() )
        return std::string(); // failed

    DWORD dwCRCCode = CRC.CalcCRC( vecData.data(), static_cast<DWORD>( vecData.size() ) );
    std::ostringstream ostrStream;
    ostrStream << std::resetiosflags( std::ios::uppercase )
               << std::hex << std::setw( 8 ) << std::setfill( '0' )
               << dwCRCCode;

    return ostrStream.str(); // succeeded
}

std::string CRC_32::GetCRCFromString( const std::string& str )
{
    DWORD dwCRCCode = CRC.CalcCRC( str.c_str(), str.size() );

    if ( 0 == dwCRCCode )
    {
        return std::string(); // failed
    }

    std::ostringstream ostrStream;
    ostrStream << std::resetiosflags( std::ios::uppercase )
        << std::hex << std::setw( 8 ) << std::setfill( '0' )
        << dwCRCCode;

    return ostrStream.str(); // succeeded
}
