# Ready-to-run server folders (decision T22, docs/ferramentas/runtime.md).
#
#   build/<preset>/run/ServerResource/   runtime/ServerResource (server scripts, versioned)
#   build/<preset>/run/ClientScript/     generated from the client data (tools/runtime/gen_clientscript.py)
#   build/<preset>/run/<Server>/         the executable, runtime/<Server>/* and the filled *.in templates
#
# x2_run_<name> builds the server and assembles its folder; it is not part of ALL.

set(X2_RUN_DIR "${CMAKE_BINARY_DIR}/run" CACHE PATH "Folder where the x2_run_* targets assemble the servers")
set(X2_CLIENT_DATA "${CMAKE_SOURCE_DIR}/_client/data" CACHE PATH
    "Client data folder (data*.kom) the ClientScript folder is generated from")

# Values of the runtime/*/*.in templates. The defaults match docker/docker-compose.yml.
if(DEFINED ENV{X2_DB_PASSWORD})
    set(_x2_db_password "$ENV{X2_DB_PASSWORD}")
else()
    set(_x2_db_password "X2dev!Passw0rd")
endif()
if(EXISTS /usr/lib/libtdsodbc.so)
    set(_x2_odbc_driver "/usr/lib/libtdsodbc.so")
    set(_x2_odbc_options "TDS_Version=7.4;ClientCharset=UTF-8")
else()
    set(_x2_odbc_driver "ODBC Driver 18 for SQL Server")
    set(_x2_odbc_options "Encrypt=yes;TrustServerCertificate=yes")
endif()
set(X2_RUN_DB_SERVER "127.0.0.1,1433" CACHE STRING "SQL Server (host,port) of the .dsn templates")
set(X2_RUN_DB_USER "sa" CACHE STRING "SQL Server user of the .dsn templates")
set(X2_RUN_DB_PASSWORD "${_x2_db_password}" CACHE STRING "SQL Server password of the .dsn templates (not versioned)")
set(X2_RUN_ODBC_DRIVER "${_x2_odbc_driver}" CACHE STRING "ODBC driver (DBLayer:SetODBCDriver)")
set(X2_RUN_ODBC_OPTIONS "${_x2_odbc_options}" CACHE STRING "ODBC connection options (DBLayer:SetODBCOptions)")
set(X2_RUN_SERVER_IP "127.0.0.1" CACHE STRING "Address the servers announce (SetLocalIP); docker/mssql/restore.sh writes it to LServerList")
set(X2_RUN_AUTH_URL "http://127.0.0.1:8000/" CACHE STRING "DEB auth service (DEBAuthManager:AddAuthServiceAddress)")

find_package(Python3 COMPONENTS Interpreter)
find_program(X2_LUAJIT_EXECUTABLE luajit DOC "LuaJIT, used to generate the ClientScript folder")

set(_x2_runtime_src "${CMAKE_SOURCE_DIR}/runtime")

# Shared folders, built once for every server
if(NOT TARGET x2_run_resources)
    set(_x2_commands
        COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different "${_x2_runtime_src}/ServerResource" "${X2_RUN_DIR}/ServerResource")
    if(Python3_Interpreter_FOUND AND X2_LUAJIT_EXECUTABLE AND IS_DIRECTORY "${X2_CLIENT_DATA}")
        list(APPEND _x2_commands
            COMMAND Python3::Interpreter -I "${CMAKE_SOURCE_DIR}/tools/runtime/gen_clientscript.py"
                "${X2_CLIENT_DATA}" "${X2_RUN_DIR}/ClientScript" --luajit "${X2_LUAJIT_EXECUTABLE}")
    else()
        message(STATUS "x2_run_*: ClientScript is not generated (needs Python 3, luajit and X2_CLIENT_DATA=${X2_CLIENT_DATA})")
    endif()
    add_custom_target(x2_run_resources ${_x2_commands}
        COMMENT "Assembling ${X2_RUN_DIR} (ServerResource, ClientScript)" VERBATIM)
endif()

# x2_runtime_server(<name> <executable target> <runtime/ subfolder>)
function(x2_runtime_server name target folder)
    set(src "${_x2_runtime_src}/${folder}")
    set(dst "${X2_RUN_DIR}/${folder}")
    file(GLOB templates CONFIGURE_DEPENDS "${src}/*.in")
    file(GLOB plain CONFIGURE_DEPENDS "${src}/*")
    list(FILTER plain EXCLUDE REGEX "\\.in$")
    foreach(t IN LISTS templates)
        get_filename_component(n "${t}" NAME)
        string(REGEX REPLACE "\\.in$" "" n "${n}")
        configure_file("${t}" "${CMAKE_CURRENT_BINARY_DIR}/runtime/${n}" @ONLY)
        list(APPEND plain "${CMAKE_CURRENT_BINARY_DIR}/runtime/${n}")
    endforeach()
    add_custom_target(x2_run_${name}
        COMMAND ${CMAKE_COMMAND} -E make_directory "${dst}/Statistics"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${plain} "$<TARGET_FILE:${target}>" "${dst}"
        DEPENDS ${target} x2_run_resources
        COMMENT "Assembling ${dst}" VERBATIM)
endfunction()
