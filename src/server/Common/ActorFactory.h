#pragma once

#include "x2/platform/noncopyable.h"
#include <dbg/dbg.hpp>

#include "FSM/FSMclass.h"
#include "Actor.h" // CreateActor() uses KActor

SmartPointer( IActorFactory );

// 단순히 KActorFactory의 인터페이스 역할만을 한다. 동적생성을 위한 포인터를 선언할 때 쓰인다.
class KActor;
class IActorFactory : public x2::NonCopyable
{
public:
    virtual std::shared_ptr<KActor> CreateActor() = 0;
};

template < class ActorType, class FSMType, int nSendQueueSize = 0 >
class KActorFactory : public IActorFactory
{
public:
    KActorFactory() : m_spFSM( new FSMType ) {}
    virtual ~KActorFactory(void) {}

    virtual std::shared_ptr<KActor> CreateActor()
    {
        std::shared_ptr<KActor> spActor( new ActorType );
        if( spActor )
        {
            spActor->Init( true );
            spActor->SetFSM( m_spFSM );
            spActor->SetSendQueueSize( nSendQueueSize );
        }
        else
        {
            START_LOG( cout, L" new KActor Fail..!!!! ");
        }

        return spActor;
    }

protected:
    FSMclassPtr m_spFSM;
};
