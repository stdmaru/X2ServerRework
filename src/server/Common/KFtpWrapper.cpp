#include "KFtpWrapper.h"

#include <cstdio>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <vector>

namespace
{
	SOCKET ConnectTo( const std::string& strHost, int iPort )
	{
		addrinfo hints = {};
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		addrinfo* pResult = nullptr;
		if( ::getaddrinfo( strHost.c_str(), std::to_string( iPort ).c_str(), &hints, &pResult ) != 0 || !pResult )
			return INVALID_SOCKET;
		SOCKET s = ::socket( pResult->ai_family, pResult->ai_socktype, pResult->ai_protocol );
		if( s != INVALID_SOCKET && ::connect( s, pResult->ai_addr, static_cast<int>( pResult->ai_addrlen ) ) != 0 )
		{
			closesocket( s );
			s = INVALID_SOCKET;
		}
		::freeaddrinfo( pResult );
		return s;
	}

	bool SendAll( SOCKET s, const char* p, size_t n )
	{
		while( n > 0 )
		{
			const int sent = ::send( s, p, static_cast<int>( n ), 0 );
			if( sent <= 0 )
				return false;
			p += sent;
			n -= static_cast<size_t>( sent );
		}
		return true;
	}

	std::string Narrow( const std::wstring& s ) { return x2::platform::WideToUtf8( s ); }
}

KFtpWrapper::KFtpWrapper()
	: m_hServer( INVALID_SOCKET )
{
}

KFtpWrapper::~KFtpWrapper()
{
	Close();
}

BOOL KFtpWrapper::Connect( std::wstring strAddress, std::wstring strUserID, std::wstring strPassword, int iPort )
{
	Close();
	m_hServer = ConnectTo( Narrow( strAddress ), iPort );
	if( m_hServer == INVALID_SOCKET )
		return FALSE;

	if( Command( "" ) != 220 ) // greeting
	{
		Close();
		return FALSE;
	}
	int iReply = Command( "USER " + Narrow( strUserID ) );
	if( iReply == 331 )
		iReply = Command( "PASS " + Narrow( strPassword ) );
	if( iReply != 230 )
	{
		Close();
		return FALSE;
	}
	return TRUE;
}

void KFtpWrapper::Close()
{
	if( m_hServer != INVALID_SOCKET )
	{
		Command( "QUIT" );
		closesocket( m_hServer );
		m_hServer = INVALID_SOCKET;
	}
	m_strBuffer.clear();
}

BOOL KFtpWrapper::SetCurrentDirectory( std::wstring strPath )
{
	return Command( "CWD " + Narrow( strPath ) ) == 250 ? TRUE : FALSE;
}

BOOL KFtpWrapper::GetFile( std::wstring strLocalFile, std::wstring strRemoteFile, EFileType eFileType )
{
	if( Command( GetTransferType( eFileType ) == 'A' ? "TYPE A" : "TYPE I" ) != 200 )
		return FALSE;
	SOCKET data = OpenPassive();
	if( data == INVALID_SOCKET )
		return FALSE;
	const int iReply = Command( "RETR " + Narrow( strRemoteFile ) );
	if( iReply != 150 && iReply != 125 )
	{
		closesocket( data );
		return FALSE;
	}
	std::ofstream out( std::filesystem::path( strLocalFile ), std::ios::binary | std::ios::trunc );
	char buf[8192];
	int n;
	while( ( n = ::recv( data, buf, sizeof( buf ), 0 ) ) > 0 )
		out.write( buf, n );
	closesocket( data );
	return ( Command( "" ) == 226 && out ) ? TRUE : FALSE;
}

BOOL KFtpWrapper::PutFile( std::wstring strLocalFile, std::wstring strRemoteFile, EFileType eFileType )
{
	std::ifstream in( std::filesystem::path( strLocalFile ), std::ios::binary );
	if( !in )
		return FALSE;
	if( Command( GetTransferType( eFileType ) == 'A' ? "TYPE A" : "TYPE I" ) != 200 )
		return FALSE;
	SOCKET data = OpenPassive();
	if( data == INVALID_SOCKET )
		return FALSE;
	const int iReply = Command( "STOR " + Narrow( strRemoteFile ) );
	if( iReply != 150 && iReply != 125 )
	{
		closesocket( data );
		return FALSE;
	}
	char buf[8192];
	bool ok = true;
	while( ok )
	{
		in.read( buf, sizeof( buf ) );
		const std::streamsize n = in.gcount();
		if( n <= 0 )
			break;
		ok = SendAll( data, buf, static_cast<size_t>( n ) );
	}
	closesocket( data );
	return ( Command( "" ) == 226 && ok ) ? TRUE : FALSE;
}

// 'I' (binary) or 'A' (ascii); the original returned the WinInet FTP_TRANSFER_TYPE_* flag
DWORD KFtpWrapper::GetTransferType( EFileType eFileType )
{
	return eFileType == eFileTypeAscii ? 'A' : 'I';
}

int KFtpWrapper::Command( const std::string& strCommand )
{
	if( m_hServer == INVALID_SOCKET )
		return 0;
	if( !strCommand.empty() )
	{
		const std::string line = strCommand + "\r\n";
		if( !SendAll( m_hServer, line.data(), line.size() ) )
			return 0;
	}
	// read lines until the final one: "NNN text" (a "NNN-" line starts a multi-line reply)
	for( ;; )
	{
		const size_t eol = m_strBuffer.find( "\r\n" );
		if( eol != std::string::npos )
		{
			const std::string line = m_strBuffer.substr( 0, eol );
			m_strBuffer.erase( 0, eol + 2 );
			if( line.size() >= 4 && isdigit( static_cast<unsigned char>( line[0] ) ) && line[3] == ' ' )
				return std::atoi( line.substr( 0, 3 ).c_str() );
			continue;
		}
		char buf[1024];
		const int n = ::recv( m_hServer, buf, sizeof( buf ), 0 );
		if( n <= 0 )
			return 0;
		m_strBuffer.append( buf, static_cast<size_t>( n ) );
	}
}

SOCKET KFtpWrapper::OpenPassive()
{
	if( m_hServer == INVALID_SOCKET )
		return INVALID_SOCKET;
	const std::string line = "PASV\r\n";
	if( !SendAll( m_hServer, line.data(), line.size() ) )
		return INVALID_SOCKET;
	// reply: 227 Entering Passive Mode (h1,h2,h3,h4,p1,p2)
	for( ;; )
	{
		const size_t eol = m_strBuffer.find( "\r\n" );
		if( eol == std::string::npos )
		{
			char buf[1024];
			const int n = ::recv( m_hServer, buf, sizeof( buf ), 0 );
			if( n <= 0 )
				return INVALID_SOCKET;
			m_strBuffer.append( buf, static_cast<size_t>( n ) );
			continue;
		}
		const std::string reply = m_strBuffer.substr( 0, eol );
		m_strBuffer.erase( 0, eol + 2 );
		if( reply.compare( 0, 4, "227 " ) != 0 )
		{
			if( reply.size() >= 4 && reply[3] == '-' )
				continue;
			return INVALID_SOCKET;
		}
		int h1, h2, h3, h4, p1, p2;
		const size_t open = reply.find( '(' );
		if( open == std::string::npos ||
			std::sscanf( reply.c_str() + open + 1, "%d,%d,%d,%d,%d,%d", &h1, &h2, &h3, &h4, &p1, &p2 ) != 6 )
			return INVALID_SOCKET;
		std::ostringstream host;
		host << h1 << '.' << h2 << '.' << h3 << '.' << h4;
		return ConnectTo( host.str(), p1 * 256 + p2 );
	}
}
