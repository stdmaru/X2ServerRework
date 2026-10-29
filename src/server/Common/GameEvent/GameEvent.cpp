#include "GameEvent.h"

KGameEvent::KGameEvent()
:
m_iUID( 0 ),
//{{ 2010. 11. 15	최육사	이벤트 스크립트 실시간 패치
m_iScriptID( 0 ),
//}}
m_eType( GET_NONE ),
m_bEnable( false )
, m_bOn( false )
{

}

KGameEvent::~KGameEvent()
{

}
