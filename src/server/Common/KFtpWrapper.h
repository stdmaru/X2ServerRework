#pragma once

// Minimal FTP client used by KLogReporter to upload statistics logs.
// Port: the original wrapped WinInet; this one speaks FTP directly (passive mode) over x2/platform/net.h.
// Same interface and BOOL results.

#include "x2/platform/win32.h"
#include "x2/platform/net.h"

#include <string>

#define INTERNET_DEFAULT_FTP_PORT 21

class KFtpWrapper
{
public:
	enum EFileType
	{
		eFileTypeBinary,
		eFileTypeAscii,
		eFileTypeMax
	};

public:
	KFtpWrapper();
	~KFtpWrapper();

	BOOL Connect(std::wstring strAddress, std::wstring strUserID,
		std::wstring strPassword, int iPort = INTERNET_DEFAULT_FTP_PORT);
	void Close();
	BOOL SetCurrentDirectory(std::wstring strPath);
	BOOL GetFile(std::wstring strLocalFile, std::wstring strRemoteFile, EFileType eFileType);
	BOOL PutFile(std::wstring strLocalFile, std::wstring strRemoteFile, EFileType eFileType);
	DWORD GetTransferType(EFileType eFileType);

protected:
	// sends a command (if not empty) and returns the final reply code, 0 on connection error
	int Command(const std::string& strCommand);
	SOCKET OpenPassive();

	SOCKET		m_hServer;
	std::string	m_strBuffer;	// unread bytes of the control connection
};
