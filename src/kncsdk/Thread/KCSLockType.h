/* @file   KCSLockType.h
    @brief  syntactic safeguard for data in a critical section.
    @author robobeg@kogstudios.com
    @since  2009-1-12, 09:00

    Port: Boost type traits replaced by <type_traits>; the in-class explicit specializations (MSVC only)
    replaced by two classes selected through an alias. Macros and semantics unchanged.

    Usage:
    @code
        class KMainThread
        {
            KCSLOCK_DECLARE( std::vector<int>, m_vecInt );          // KCSLockType< std::vector<int> > m_vecInt
            KCSLOCK_DECLARE( std::vector<int>&, m_refVecInt );      // KCSLockType< std::vector<int> >& m_refVecInt
            static KCSLOCK_DECLARE( bool, ms_bInitialized );        // + KCSLOCK_IMPLEMENT( KMainThread, ms_bInitialized ) in the .cpp
        };

        void KMainThread::Access()
        {
            KCSLOCK_BEGIN( m_vecInt )           // read/write access inside the block; return/break/continue are fine
                m_vecInt.push_back( 1 );
            KCSLOCK_END()
        }

        void KMainThread::Read() const
        {
            KCSLOCK_CBEGIN( m_vecInt )          // read-only access (const member functions)
                ...
            KCSLOCK_END()
        }
    @endcode
    Pass a KCSLockType to another object by reference (KCSLOCK_TYPE( m_refVecInt )), outside BEGIN/END blocks.
    Types with ',' need KCOMMA() or, better, a typedef.
*/
#ifndef _KCSLOCKTYPE_H
#define _KCSLOCKTYPE_H

#include "x2/platform/sync.h"

#include <type_traits>

template<typename T>
class KCSLockType
{
    static_assert( !std::is_const_v<T>, "KCSLockType<const T> is not supported" );
    static_assert( !std::is_reference_v<T>, "use KCSLOCK_DECLARE( T&, x ) for references" );

public:
    typedef T TYPE;

    class KCSLockedAccessRW
    {
    public:
        explicit KCSLockedAccessRW( KCSLockType& kIn_ ) : m_cs( kIn_.m_cs ), m_variable( kIn_.m_variable )
        {
            ::EnterCriticalSection( &m_cs );
        }
        ~KCSLockedAccessRW() { ::LeaveCriticalSection( &m_cs ); }
        KCSLockedAccessRW( const KCSLockedAccessRW& ) = delete;
        KCSLockedAccessRW& operator=( const KCSLockedAccessRW& ) = delete;

        T& AccessVariable() { return m_variable; }

    private:
        CRITICAL_SECTION&   m_cs;
        T&                  m_variable;
    };

    class KCSLockedAccessRO
    {
    public:
        explicit KCSLockedAccessRO( const KCSLockType& kIn_ ) : m_cs( kIn_.m_cs ), m_variable( kIn_.m_variable )
        {
            ::EnterCriticalSection( &m_cs );
        }
        ~KCSLockedAccessRO() { ::LeaveCriticalSection( &m_cs ); }
        KCSLockedAccessRO( const KCSLockedAccessRO& ) = delete;
        KCSLockedAccessRO& operator=( const KCSLockedAccessRO& ) = delete;

        const T& AccessVariable() const { return m_variable; }

    private:
        CRITICAL_SECTION&   m_cs;
        const T&            m_variable;
    };

    template<bool bIsConst>
    using KCSLockedAccess = std::conditional_t<bIsConst, KCSLockedAccessRO, KCSLockedAccessRW>;

    KCSLockType() { ::InitializeCriticalSection( &m_cs ); }
    KCSLockType( const T& kIn_ ) : m_variable( kIn_ ) { ::InitializeCriticalSection( &m_cs ); }
    ~KCSLockType() { ::DeleteCriticalSection( &m_cs ); }
    KCSLockType( const KCSLockType& ) = delete;
    KCSLockType& operator=( const KCSLockType& ) = delete;

private:
    mutable CRITICAL_SECTION    m_cs;
    T                           m_variable;
};

// Helpers for the macros (aliases do not need `typename` inside templates).
template<typename D>
using KCSLockTypeOf_ = std::conditional_t< std::is_reference_v<D>,
    KCSLockType< std::remove_reference_t<D> >&,
    KCSLockType< std::remove_reference_t<D> > >;

template<typename L, bool bIsConst>
using KCSLockedAccessOf_ = typename std::remove_reference_t<L>::template KCSLockedAccess<bIsConst>;

template<typename L>
using KCSLockValueOf_ = typename std::remove_reference_t<L>::TYPE;

#define _KCSLOCK_TYPEDEF_(_type_)           KCSLockTypeOf_< _type_ >

/* @def   KCSLOCK_TYPE(_variable_)       @brief \a _variable_ 의 \c KCSLockType */
#define KCSLOCK_TYPE(_variable_)            __KCSLockType__##_variable_

/* @def   KCSLOCK_VARIABLE_TYPE(_variable_)  @brief \a _variable_ 의 type */
#define KCSLOCK_VARIABLE_TYPE(_variable_)   KCSLockValueOf_< KCSLOCK_TYPE(_variable_) >

/* @def   KCSLOCK_DECLARE(_type_,_variable_)  @brief type \a _type_ 의 \a _variable_ 을 \c KCSLockType 으로 선언한다. */
#define KCSLOCK_DECLARE(_type_,_variable_)  _KCSLOCK_TYPEDEF_(_type_) _variable_;  typedef _KCSLOCK_TYPEDEF_(_type_) KCSLOCK_TYPE(_variable_);

/* @def   KCSLOCK_IMPLEMENT(_classname_,_variable_)  @brief static KCSLOCK_DECLARE( ... ) 의 implementation */
#define KCSLOCK_IMPLEMENT(_classname_,_variable_)   _classname_::KCSLOCK_TYPE(_variable_) _classname_::_variable_

/* @def KCSLOCK_BEGIN(_variable_)  @brief non-const 멤버 함수에서 read/write 접근 */
#define KCSLOCK_BEGIN(_variable_)  { KCSLockedAccessOf_< KCSLOCK_TYPE(_variable_), false > _kcsLockedAccess_(_variable_); \
    KCSLOCK_VARIABLE_TYPE(_variable_)& (_variable_)( _kcsLockedAccess_.AccessVariable() );

/* @def KCSLOCK_CBEGIN(_variable_)  @brief const 멤버 함수 내에서나 read만 할 때 */
#define KCSLOCK_CBEGIN(_variable_)  { const KCSLockedAccessOf_< KCSLOCK_TYPE(_variable_), true > _kcsLockedAccess_(_variable_); \
    const KCSLOCK_VARIABLE_TYPE(_variable_)& (_variable_)( _kcsLockedAccess_.AccessVariable() );

/* @def KCSLOCK_END()  @brief \c KCSLOCK_BEGIN 또는 \c KCSLOCK_CBEGIN 블럭을 해제 */
#define KCSLOCK_END()     }

/* @define KCOMMA()  @brief 매크로 파라미터 안에서 ',' 대신 사용 */
#define KCOMMA()   ,

/* @def KCSLOCK_SET_VALUE(_variable_)  @brief 멤버 변수에 값을 간단하게 저장 */
#define KCSLOCK_SET_VALUE(_variable_, value)   KCSLOCK_BEGIN(_variable_) _variable_ = value; KCSLOCK_END()

#endif // #ifndef _KCSLOCKTYPE_H
