# Protocolo de rede e serialização

## Onde está

- Definição de pacotes: `KncWX2Server/Common/ClientPacket.{h,cpp}`, `CommonPacket.{h,cpp}`, `OnlyGlobal/Packet/*_Global.cpp`,
  IDs em `EventID_{Client,Server,System}.h` (+ `_Global`). **Compartilhados** pelo cliente (`X2Lib` inclui `KncWX2Server/Common`).
- Serialização: `KNCSDK_*/Include/Serializer/` (`Serializer.h`, `SerBuffer`, `Ser{Vector,Map,Set,List,Deque,Pair}.h`, `KeyedSerializer.h`, `ByteOrder`).
- Transporte: `Common/Socket/{Session,X2Packet,NetCommon}` e `X2ServerProtocol/Socket` (cliente). Cripto via `KncSecurity` (Crypto++).

## O problema

O serializer escreve tipos primitivos com `WriteBytes(&x, sizeof(T))` (ver `Serializer.h:129`), ou seja, o formato no fio
**é o layout de memória do MSVC x86**. Ao compilar com GCC/Clang em x64:

| Tipo C++ | Win32 MSVC | Linux x64 | Efeito |
|---|---|---|---|
| `long` / `unsigned long` | 4 B | **8 B** | pacote corrompido (49 usos só em `ClientPacket.h`/`CommonPacket.h`) |
| `wchar_t` / `std::wstring` | 2 B (UTF-16) | **4 B** (UTF-32) | todas as strings quebram (`Put(const std::wstring&)`) |
| `time_t` (`_USE_32BIT_TIME_T` no cliente) | 4 B | 8 B | datas |
| `bool`, `enum` | 1 B / 4 B | 1 B / 4 B | ok, mas enums sem tipo explícito devem ganhar `: int32_t` |
| `DWORD`, `UINT`, `__int64` | 4 / 4 / 8 | precisam typedef | definir como `uint32_t`/`int64_t` |
| ponteiros/`size_t` em structs | 4 B | 8 B | auditar se algum é serializado |

## Estratégia proposta

1. Typedefs fixos numa header de plataforma (`DWORD=uint32_t`, `LONG=int32_t`, ...).
2. No `Serializer`, sobrecargas explícitas que convertem `long` → `int32_t` e `std::wstring` → UTF-16LE (`char16_t`) **no fio**.
   Internamente o código pode continuar com `std::wstring` por enquanto.
3. `static_assert(sizeof(...))` nos structs serializados como POD.
4. **Teste de bytes idênticos**: capturar pacotes do servidor/cliente originais (ou serializar fixtures no build MSVC x86 do `_reference`)
   e comparar com a saída do código portado.
5. Endianness: todos os alvos são little-endian; `ByteOrder.cpp` pode ser simplificado.

A mesma preocupação vale para **binding ODBC** (`SQLBindParameter` com `SQL_C_LONG` + `long` de 8 bytes) e para leitura de arquivos binários do cliente (`.kom`, headers de mesh) com structs.
