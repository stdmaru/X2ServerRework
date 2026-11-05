# Network core shared by the servers and the client (decision T34): the event/actor model (KEvent, KPerformer,
# KSimObject, KThreadManager) and the TCP layer of Socket/ (KSocketObject, KSession, KIocp, KIOThread). Included from
# src/CMakeLists.txt when the servers or the client are built; x2_server_common leaves these files out, and the
# client's X2ServerProtocol links it instead of its own copy. Nothing here may include server-only headers.
set(X2_COMMON_NET_SOURCES
    Event.cpp
    Performer.cpp
    SimObject.cpp
    ThreadManager.cpp
    Socket/CircularQueue.cpp
    Socket/IOCP.cpp
    Socket/IOThread.cpp
    Socket/NetCommon.cpp
    Socket/Session.cpp
    Socket/SocketObject.cpp)
list(TRANSFORM X2_COMMON_NET_SOURCES PREPEND ${CMAKE_CURRENT_LIST_DIR}/)

add_library(x2_common_net STATIC ${X2_COMMON_NET_SOURCES})
add_library(x2::common_net ALIAS x2_common_net)
target_include_directories(x2_common_net PUBLIC ${CMAKE_CURRENT_LIST_DIR})
# Event.h includes ServerDefine.h itself; forced here too, like x2_server_common, so these files see the same flags
target_compile_options(x2_common_net PRIVATE
    "$<IF:$<BOOL:${MSVC}>,/FI${CMAKE_CURRENT_LIST_DIR}/ServerDefine.h,SHELL:-include ${CMAKE_CURRENT_LIST_DIR}/ServerDefine.h>")
target_link_libraries(x2_common_net PUBLIC x2::kncsdk x2::platform)
