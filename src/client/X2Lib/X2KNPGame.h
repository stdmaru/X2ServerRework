#pragma once

//note : 한국 nProtect 모듈

#define CHECK_KNP_TIME	5000

extern CNPGameLib*	pNpgl;


bool	CheckNPHackUser();

bool	KNP_Init( HINSTANCE hInst );

void	KNP_Close();

void	KNP_SetHwnd( HWND hWnd );

bool	KNP_GetFindGameMon();

DWORD	KNP_Send( LPCWSTR wstrMSG );

void	KNP_AUTH2( GG_AUTH_DATA* pAuthData );


BOOL CALLBACK NPGameMonCallback( DWORD dwMsg, DWORD dwArg );
