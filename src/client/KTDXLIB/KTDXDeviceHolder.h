#pragma once

#include "x2/platform/win32.h"

#include <list>

class	CKTDXDevice;
struct	CKTDXDeviceData;
typedef std::list<CKTDXDeviceData>   CKTDXDeviceDataList;

class CKTDXDeviceHolder
{
public:
	CKTDXDeviceHolder(void);
	virtual ~CKTDXDeviceHolder(void);

	bool	TextureReady( const WCHAR* pFileName );
	bool	XSkinMeshReady( const WCHAR* pFileName );
	bool	XMeshReady( const WCHAR* pFileName );
	bool	SoundReady( const WCHAR* pFileName );

	// 2 is CKTDXDeviceManager::PRIORITY_LOW
	bool	TextureReadyInBackground( const WCHAR* pFileName, int iPriority = 2 );
	bool	XSkinMeshReadyInBackground( const WCHAR* pFileName, int iPriority = 2 );
	bool	XMeshReadyInBackground( const WCHAR* pFileName, int iPriority = 2 );
	bool	SoundReadyInBackground( const WCHAR* pFileName
		, int iPriority = 2
		);

	bool 	DeviceReady( CKTDXDevice* pDevice );
	bool 	DeviceUnready( CKTDXDevice* pDevice );

	void	ClearAllDevice();
	CKTDXDeviceDataList&	AccessReadyDeviceList()	{ return m_listReadyDevice; }

protected:

	void            CustomInitialize_();
	void            CustomFinalize_();

private:

	CKTDXDeviceDataList		m_listReadyDevice;

};
