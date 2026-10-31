# Ready-to-run server folders (decision T22, docs/ferramentas/runtime.md).
#
#   build/<preset>/run/ServerResource/   runtime/ServerResource (server scripts, versioned)
#   build/<preset>/run/ClientScript/     generated from the client data (tools/runtime/gen_clientscript.py)
#   build/<preset>/run/<Server>/         the executable, runtime/<Server>/* and the filled *.in templates
#
# x2_run_<name> builds the server and assembles its folder; with X2_RUN_FOLDERS (default) every folder is part of ALL.
# These folders are a development setup (local database, this machine's ODBC driver and client), not a release package.

option(X2_RUN_FOLDERS "Assemble the ready-to-run server folders in every build (x2_run_* in ALL)" ON)

# x2_env_cache(<var> <type> <default> <doc> [<env>]): cache variable that the environment variable <env> (default:
# the same name; from the shell or docker/.env, see docker/.env.example) overrides at every configure when not empty.
# Without it the cache keeps its value (-D or the default). A relative PATH is taken from the repository root.
function(x2_env_cache var type default doc)
    set(env "${var}")
    if(ARGC GREATER 4)
        set(env "${ARGV4}")
    endif()
    set(${var} "${default}" CACHE ${type} "${doc}")
    set(value "$ENV{${env}}")
    if(value STREQUAL "")
        return()
    endif()
    if(type STREQUAL "PATH")
        cmake_path(ABSOLUTE_PATH value BASE_DIRECTORY "${CMAKE_SOURCE_DIR}" NORMALIZE)
    endif()
    if(NOT value STREQUAL "${${var}}")
        set(${var} "${value}" CACHE ${type} "${doc}" FORCE)
    endif()
endfunction()

x2_env_cache(X2_RUN_DIR PATH "${CMAKE_BINARY_DIR}/run" "Folder where the x2_run_* targets assemble the servers")
x2_env_cache(X2_CLIENT_DATA PATH "${CMAKE_SOURCE_DIR}/_client/data"
    "Client data folder (data*.kom) the ClientScript folder is generated from")

# Values of the runtime/*/*.in templates. The defaults match docker/docker-compose.yml; X2_DB_PASSWORD, X2_DB_PORT and
# X2_SERVER_IP are the variables docker compose and docker/mssql/restore.sh also read.
# FreeTDS: /usr/lib on Arch, /usr/lib/<triplet>/odbc on Ubuntu (the build image, T24)
find_library(X2_FREETDS_ODBC tdsodbc PATH_SUFFIXES odbc DOC "FreeTDS ODBC driver, preferred by the .dsn templates")
if(X2_FREETDS_ODBC)
    set(_x2_odbc_driver "${X2_FREETDS_ODBC}")
    set(_x2_odbc_options "TDS_Version=7.4;ClientCharset=UTF-8")
else()
    set(_x2_odbc_driver "ODBC Driver 18 for SQL Server")
    set(_x2_odbc_options "Encrypt=yes;TrustServerCertificate=yes")
endif()
if(DEFINED ENV{X2_DB_PORT} AND NOT DEFINED ENV{X2_RUN_DB_SERVER})
    set(ENV{X2_RUN_DB_SERVER} "127.0.0.1,$ENV{X2_DB_PORT}")
endif()
x2_env_cache(X2_RUN_DB_SERVER STRING "127.0.0.1,1433" "SQL Server (host,port) of the .dsn templates")
x2_env_cache(X2_RUN_DB_USER STRING "sa" "SQL Server user of the .dsn templates")
x2_env_cache(X2_RUN_DB_PASSWORD STRING "X2dev!Passw0rd" "SQL Server password of the .dsn templates (not versioned)" X2_DB_PASSWORD)
x2_env_cache(X2_RUN_DB_NAME_PREFIX STRING "" "Prefix of the database names in the .dsn templates (a server with Elsword-Account...: Elsword-)")
x2_env_cache(X2_RUN_ODBC_DRIVER STRING "${_x2_odbc_driver}" "ODBC driver (DBLayer:SetODBCDriver)")
x2_env_cache(X2_RUN_ODBC_OPTIONS STRING "${_x2_odbc_options}" "ODBC connection options (DBLayer:SetODBCOptions)")
x2_env_cache(X2_RUN_SERVER_IP STRING "127.0.0.1"
    "Address the servers announce (SetLocalIP); docker/mssql/restore.sh writes it to LServerList" X2_SERVER_IP)
x2_env_cache(X2_RUN_EPUBLISHER_PORT STRING "8000" "Port of the EPublisherServer")
x2_env_cache(X2_RUN_EPUBLISHER_URL STRING "http://127.0.0.1:${X2_RUN_EPUBLISHER_PORT}/"
    "EPublisher service the servers call (DEBAuthManager:AddAuthServiceAddress)")

find_package(Python3 COMPONENTS Interpreter)
find_program(X2_LUAJIT_EXECUTABLE luajit DOC "LuaJIT, used to generate the ClientScript folder")
# Without a system luajit (the build image has none, T21), build the interpreter of the pinned LuaJIT commit.
# It runs after the library build: both use the same source tree.
set(_x2_luajit_depends)
if(NOT X2_LUAJIT_EXECUTABLE AND TARGET x2_luajit_build AND NOT WIN32 AND IS_DIRECTORY "${X2_CLIENT_DATA}")
    set(_x2_luajit_exe "${luajit_SOURCE_DIR}/src/luajit")
    add_custom_command(OUTPUT "${_x2_luajit_exe}"
        COMMAND ${X2_MAKE} -C "${luajit_SOURCE_DIR}/src" BUILDMODE=static CC=${CMAKE_C_COMPILER} luajit
        DEPENDS x2_luajit_build
        COMMENT "Building the LuaJIT ${X2_LUAJIT_COMMIT} interpreter (ClientScript generation)"
        VERBATIM)
    add_custom_target(x2_luajit_exe DEPENDS "${_x2_luajit_exe}")
    set(X2_LUAJIT_EXECUTABLE "${_x2_luajit_exe}")
    set(_x2_luajit_depends x2_luajit_exe)
endif()

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
        DEPENDS ${_x2_luajit_depends}
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
    set(all)
    if(X2_RUN_FOLDERS)
        set(all ALL)
    endif()
    add_custom_target(x2_run_${name} ${all}
        COMMAND ${CMAKE_COMMAND} -E make_directory "${dst}/Statistics"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${plain} "$<TARGET_FILE:${target}>" "${dst}"
        DEPENDS ${target} x2_run_resources
        COMMENT "Assembling ${dst}" VERBATIM)
endfunction()
