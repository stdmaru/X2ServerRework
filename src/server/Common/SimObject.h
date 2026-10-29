#pragma once

#include "x2/platform/noncopyable.h"

#include <RTTI.h>
#include <ToString.h>
#include <vector>
#include <string>
#include <memory>

#include "KncUidType.h"

#define IN
#define OUT

#pragma pack( push, 1 )

/// ToString, RTTI, RefCount, Name
class KSimObject : public std::enable_shared_from_this<KSimObject>, public x2::NonCopyable
{
    NiDeclareRootRTTI( KSimObject );
    DeclToStringW;

public:
    KSimObject(void);
    virtual ~KSimObject(void);

    const std::wstring& GetName() const         { return m_strName; }
    void SetName(const std::wstring& strName_)  { m_strName = strName_; }
    void SetName(const wchar_t* szName_)        { m_strName = szName_; }

    UidType GetUID() const                      { return m_nUID; }
    void SetUID( UidType nUID )                 { m_nUID = nUID; }

    int GetRefCount() const;

    template <class T> void GetThisPtr( OUT std::shared_ptr<T>& spPtr ) // output parameter로 포인터 받음
    {
        spPtr = std::static_pointer_cast<T>( shared_from_this() );
    }

    template <class T> std::shared_ptr<T> GetThisPtr()  // function return으로 포인터 받음
    {
        return std::static_pointer_cast<T>( shared_from_this() );
    }

protected:
    std::wstring        m_strName;
    UidType             m_nUID;
};

#pragma pack( pop )
