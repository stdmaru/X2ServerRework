#pragma once

#include <windows.h>
#include "lua_tinker.h"
#include <assert.h>


class KLuabinder
{

public:
	KLuabinder();
	KLuabinder( lua_State* pLuaState ){ m_pkLuaState = pLuaState; }
	virtual ~KLuabinder();

	//void		SetLuaTinker(){ lua_tinker::init(m_pkLuaState); }

	HRESULT		DoFile(const char* strFilename);

	//HRESULT
    bool    DoMemoryNotEncrypt(const char* pBuffer, long nSize)
	{
        if ( m_pkLuaState == NULL || pBuffer == NULL || nSize <= 0 )
            return false;

        bool bOK = false;

        if( lua_getencoding( GetLuaState() ) != LUA_ENCODING_LEGACY )
        {
            if( pBuffer[0] == ( char )033 )
            {
                bOK = lua_tinker::dobuffer( m_pkLuaState, pBuffer, nSize );
            }
            else
            {
                assert( nSize >= 3 );
                if( nSize < 3 )
                {
                    return false;
                }

                assert( ( pBuffer[0] == ( char )0xef ) && ( pBuffer[1] == ( char )0xbb ) && ( pBuffer[2] == ( char )0xbf ) );
                if( ( pBuffer[0] != ( char )0xef ) || ( pBuffer[1] != ( char )0xbb ) || ( pBuffer[2] != ( char )0xbf ) )
                {
                    return false;
                }

                bOK = lua_tinker::dobuffer( m_pkLuaState, pBuffer + 3, nSize - 3 );
            }
        }
        else
        {
            bOK = lua_tinker::dobuffer( m_pkLuaState, pBuffer, nSize );
        }
        //{{

        return  bOK;
	}

	lua_State*	GetLuaState(){ return m_pkLuaState; }

    void    FlushJitCompileMode();

protected:

	void Initialize();
	void Finalize();


private:
    lua_State* m_pkLuaState;
};

#define KDeclareLuaInvoke \
public: \
    void to_lua(lua_State* L) { lua_tinker::push_::invoke(L, this); }

#define KRegLua(x) x
