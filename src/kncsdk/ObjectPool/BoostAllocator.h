#pragma once

// 2010-01-15 : 최육사
// Pooled operator new/delete for a class (originally a wrapper of boost::object_pool).
// [참고] BoostAllocator를 상속하는 class는 반드시 자식클래스중 가장 하위 클래스여야한다.
//
// Port: backed by KncPool::FixedBlockPool (same growth: first chunk of 1 block, doubling).
// BoostAllocator is not thread-safe, BoostAllocatorSync is (as in the original).

#include "FixedBlockPool.h"
#include "x2/platform/sync.h"

#define BOOST_ALOC_CLASS(className) \
	class className ## Sealer \
	{ \
	private: className ## Sealer(){}; \
		friend class className; \
	}; \
	class className : virtual private className ## Sealer, public BoostUtil::BoostAllocator<className> \

#define BOOST_ALOC_CLASS_INHERITANCE(className, inheritance) \
	class className ## Sealer \
	{ \
	private: className ## Sealer(){}; \
		friend class className; \
	}; \
	class className : virtual private className ## Sealer, public BoostUtil::BoostAllocator<className>, inheritance \

#define BOOST_ALOC_SYNC_CLASS(className) \
	class className ## Sealer \
	{ \
	private: className ## Sealer(){}; \
		friend class className; \
	}; \
	class className : virtual private className ## Sealer, public BoostUtil::BoostAllocatorSync<className> \

#define BOOST_ALOC_SYNC_CLASS_INHERITANCE(className, inheritance) \
	class className ## Sealer \
	{ \
	private: className ## Sealer(){}; \
		friend class className; \
	}; \
	class className : virtual private className ## Sealer, public BoostUtil::BoostAllocatorSync<className>, inheritance \

namespace BoostUtil {

struct AllocatorCriticalSection : public CRITICAL_SECTION
{
	AllocatorCriticalSection()  { InitializeCriticalSection( this ); }
	~AllocatorCriticalSection() { DeleteCriticalSection( this ); }
};

class AllocatorLocker
{
public:
	AllocatorLocker( CRITICAL_SECTION& cs ) : m_pCS( &cs ) { EnterCriticalSection( m_pCS ); }
	~AllocatorLocker() { LeaveCriticalSection( m_pCS ); }
	AllocatorLocker( const AllocatorLocker& ) = delete;
	AllocatorLocker& operator=( const AllocatorLocker& ) = delete;

private:
	CRITICAL_SECTION* m_pCS;
};

// The pool is created on first use: the allocated type is incomplete where BoostAllocator<T> is a base.
template< class AllocType >
using PoolOf = KncPool::FixedBlockPool< sizeof( AllocType ), alignof( AllocType ) >;

template< class AllocType >
class BoostAllocator
{
public:
	void* operator new( size_t /*size*/ )
	{
		return Pool().Allocate();
	}

	void operator delete( void* pPointer )
	{
		Pool().Free( pPointer );
	}

	unsigned int GetNextAllocSize()
	{
		return static_cast<unsigned int>( Pool().NextSize() );
	}

private:
	static auto& Pool()
	{
		static PoolOf<AllocType> s_Pool( 1, true );	// 생성자의 전달인자는 미리 생성해놓을 메모리 블럭 갯수
		return s_Pool;
	}
};

template< class AllocType >
class BoostAllocatorSync
{
public:
	void* operator new( size_t /*size*/ )
	{
		AllocatorLocker lock( Cs() );
		return Pool().Allocate();
	}

	void operator delete( void* pPointer )
	{
		AllocatorLocker lock( Cs() );
		Pool().Free( pPointer );
	}

	unsigned int GetNextAllocSize()
	{
		AllocatorLocker lock( Cs() );
		return static_cast<unsigned int>( Pool().NextSize() );
	}

private:
	static auto& Pool()
	{
		static PoolOf<AllocType> s_Pool( 1, true );
		return s_Pool;
	}

	static AllocatorCriticalSection& Cs()
	{
		static AllocatorCriticalSection s_cs;
		return s_cs;
	}
};

} // namespace BoostUtil
