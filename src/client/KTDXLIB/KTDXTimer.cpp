#include "stdafx.h"
#include "./KTDXTimer.h"






void    CKTDXCheckElapsedTime::OnFrameMove( float fElapsedTime )
{
	m_fSumOfElapsedTime += fElapsedTime;
}
