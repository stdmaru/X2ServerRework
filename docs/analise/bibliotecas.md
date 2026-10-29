# Bibliotecas — o que fazer com cada uma

Legenda de ação: **UP** = trocar pela versão upstream via CMake · **PORT** = código próprio, portar ·
**STD** = substituir pela biblioteca padrão C++ · **RM** = remover · **NOVO** = substituir por outra lib.

## KNCSDK (código próprio — o que realmente precisa ser portado)

`KNCSDK_Server` e `KNCSDK_Client` são o mesmo SDK com divergências (36 arquivos: `dbg/*`, `SecureBuffer`,
`Ser{Map,Set,Vector}.h`, `KCSLockType.h`, `lua_tinker.h`, `ToString.h`, `KncUtil.h`...). Unificar num só.

| Sub-lib (vcxproj) | Conteúdo | Ação |
|---|---|---|
| `KncDebug71` | `Include/dbg/{assert,logfile,profiler}` — log usado em todo lugar (`START_LOG`, `dbg::cerr`...) | **PORT** (provavelmente reescrever sobre `fmt`/`spdlog` mantendo as macros) |
| `KncSerialize71` | `Include/Serializer/*` + zlib embutida (`ZLIB_WINAPI`) | **PORT** Serializer (crítico p/ protocolo, ver [protocolo.md](protocolo.md)); zlib **UP** |
| `KncSecurity71` | Crypto++ **8.9.0** inteiro + `Include/KncSecurity/*` (`SecureBuffer`) | Crypto++ **UP** (mesma versão serve); `KncSecurity` **PORT** |
| `KncLua71` | Lua 5.0.2 **e** 5.1.4 (612 `ExcludedFromBuild`), luabind, `lua_tinker.cpp` | **RM** as Luas embutidas → LuaJIT único; luabind **RM** (2 arquivos no servidor) |
| headers soltos | `Thread/`, `ObjectPool/`, `KNCSingleton.h`, `RTTI.h`, `ToString.h`, `KncUtil*.h`, `VariableSizeBuffer.h`, `vld.h` | **PORT**/**STD**; `vld.h` (Visual Leak Detector) **RM** |
| `UnitTest/` | testes antigos de crypto/lua/serializer | aproveitar como base de testes |

## Lua

Todas as cópias expõem **API Lua 5.1**:

| Cópia | Usada por | Ação |
|---|---|---|
| `KNCSDK_*/Src/lua5.1.4`, `lua5.0.2` | servidor (via KncLua) | **RM** |
| `luaLib/` (5.1.4 + `KLuabinder`, `KLuaManager`, `lua_tinker`) | cliente (debug) / ferramentas | **RM** (manter só wrappers úteis) |
| `luajitLib/LuaJIT-2.0.2` | cliente | **UP** → LuaJIT 2.1 (x64, ARM64) para cliente **e** servidor |
| `LuaCommon/` | cliente | fonte única de `lua_tinker` + `KLuaManager` → **PORT** |

`lua_tinker` é a ponte C++↔Lua: ~815 `class_def`, ~241 `decl`, ~148 `class_add` só no servidor; 73 arquivos no cliente.
Ele depende de templates antigos do MSVC — esperar trabalho para compilar em GCC/Clang.

**A verificar**: se os `.kom` contêm Lua em **bytecode** LuaJIT 2.0 (incompatível com 2.1) ou fonte. Se for bytecode, ou mantemos 2.0.x
ou precisamos recompilar os scripts (o que mexe em assets).

## 3rdparty

| Lib | Versão no ref | Usada por | Ação |
|---|---|---|---|
| Boost | 1.91 (headers + `serialization` pré-compilado) | servidor (229 arquivos), cliente (~90) | **UP**, minimizando; muito é `boost::shared_ptr`, `bind`, `thread`, `timer`, `format` → **STD** |
| Intel TBB | 2.2 (2009) | GameServer, 2 arquivos | **STD** (ou oneTBB) |
| DXSDK (jun/2010) | d3d9, d3dx9, dinput8, dsound, dxerr9 | cliente; servidor só matemática | **RM** → ver [cliente.md](cliente.md) |
| FreeType | 2.4.11 (`freetype2411.lib`) | KTDXLIB fontes | **UP** |
| libxml (`libxml/`) | libxml2 antigo | KTDXLIB | **UP** libxml2 — ou avaliar se tinyxml (já presente em `KTDXLIB/tinyxml` e `Common/TinyXml`) basta |
| CxImage + jpeg + png | antigo | KTDXLIB (21 arquivos) | **NOVO** → `stb_image`/`libpng` |
| zlib | embutida em vários lugares (`KNCSDK`, `KTDXLIB/zlibstat.lib`) | `.kom`, serializer | **UP** |
| tinyxml | 2 cópias | servidor/cliente | **UP** tinyxml2 ou unificar |
| gSOAP (`Common/SOAP`) | gerado | servidor, auth de publishers | **RM** (ver binários proprietários) |
| json (`json_vc71_libmt.lib`) | jsoncpp antigo binário | cliente/servidor | **NOVO** → nlohmann/json ou jsoncpp upstream |
| BasicExcel (`X2Lib`) | — | export de debug | **RM** provável |

## Próprias, menores

| Lib | Ação |
|---|---|
| `X2ServerProtocol/` | Fundir com `KncWX2Server/Common/Socket` + `Actor*`/`Event*` numa lib de rede comum |
| `Profiler/` | **RM** ou trocar por Tracy |
| `KTDXLIB/tinyxml`, `DXUT/` | DXUT **RM** (framework de exemplo da Microsoft) |
