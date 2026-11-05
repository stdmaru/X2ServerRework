#pragma once

// The client's network library (docs/porting/client-serverprotocol.md): everything X2Lib needs to talk to the servers,
// in one header, as in the original. The event/actor model and the TCP layer are the servers' (src/server/Common,
// x2::common_net, decision T34); this folder keeps only the client's proxies and the CX2ServerProtocol facade.

// Defined by the original here, and still tested by X2Lib and KTDXLIB, which reach them through this header.
// Inside X2ServerProtocol they are resolved by hand (all three were on).
#define ADD_COLLECT_CLIENT_INFO_PROTOCOL
#define X2OPTIMIZE_X2SERVERPROTOCOL_MULTITHREAD_CRASH_BUG_FIX
#define X2OPTIMIZE_SESSION_THREAD_SAFETY

#include "x2/platform/win32.h"
#include "x2/platform/window_messages.h"    // WPARAM, LPARAM
#include "x2/platform/net.h"

#include <atomic>
#include <cassert>
#include <memory>
#include <queue>
#include <string>
#include <vector>

#include <KncSecurity/KncSecurity.h>
#include <Serializer/SerBuffer.h>
#include <Serializer/Serializer.h>
#include <RTTI.h>
#include <ToString.h>
#include <Thread/Thread.h>
#include <Thread/Locker.h>
#include <KNCSingleton.h>
#include <dbg/dbg.hpp>
#include <KncUtil.h>    // toWideString

extern bool g_bIsSERVICE;

// Delivers a received packet to the game: uMsg = the server message ID, wParam = the event ID, lParam = a new
// KSerBuffer* with the packet's data, which the receiver deletes.
typedef bool (*SEND_MSG_FUNC)( UINT uMsg, WPARAM wParam, LPARAM lParam, bool bDirectProcess );

// shared with the servers (src/server/Common)
#include "KncSmartPtr.h"
#include "KncUidType.h"
#include "Event.h"
#include "Performer.h"
#include "KncSend.h"
#include "Socket/NetCommon.h"
#include "Socket/Session.h"
#include "Socket/IOCP.h"
#include "SystemPacket.h"

// the client's own
#include "ActorProxy.h"
#include "UserProxy.h"
#include "TRUser.h"
#include "X2ServerProtocol.h"
