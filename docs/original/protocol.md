[Início](../README.md) › [Código original](README.md) › Protocolo de rede e serialização

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

## Formato confirmado no código (2026-10-29)

Lido no `KNCSDK` original durante o porte; os testes de `src/kncsdk/tests/test_serializer.cpp` codificam isto.

| Item | Formato na rede |
|---|---|
| `short`, `int`, `DWORD`/`UINT`, `INT64`/`UINT64`, `float`, `double` | **big-endian** (`ByteOrder.h`: `BYTEORDER_NET = BIG`), 2/4/8 bytes |
| `char`, `unsigned char`, `bool` | 1 byte (`bool` = 0/1) |
| `wchar_t` | 2 bytes **little-endian** (memória crua, sem troca de bytes) |
| `std::string` | `DWORD` (BE) com o tamanho em bytes + bytes |
| `std::wstring` | `DWORD` (BE) com o tamanho **em bytes** + UTF-16 **LE** |
| contêineres STL | `DWORD` (BE) com o número de itens + itens |
| tags | 1 byte antes de cada valor, só se o serializer estiver em modo `tagging` |
| `KSerBuffer` comprimido | `DWORD` **LE** (cru) com o tamanho original + stream zlib (nível 1) |
| quadro TCP (`KSession`) | tamanho total (`USHORT`, **LE** cru, inclui os 2 bytes do próprio tamanho) + cabeçalho seguro; quadros maiores que `MAX_PACKET_SIZE` (32 KB) derrubam a conexão |
| primeiro pacote (servidor → cliente) | `E_ACCEPT_CONNECTION_NOT` com o SPI novo e a `KSecurityAssociation`, cifrado com a chave padrão; depois disso os dois lados usam o SPI novo |
| datagrama UDP (`CKTDNUDP`) | 1 byte com o ID (`X2_PACKET_TYPE` ou `SP_*`) + a struct copiada como memória (`#pragma pack(1)`, little-endian, sem criptografia), até 2.048 bytes |
| cabeçalho seguro (`KSecureBuffer`) | SPI (`unsigned short`, LE cru) + número de sequência (`unsigned int`, LE cru) + IV + dados cifrados (DES-CBC) + ICV (HMAC-MD5) |

Ou seja: números passam por `HTON`, e o que é copiado como memória (`wchar_t`, cabeçalho seguro, tamanho de compressão) fica little-endian.
O porte reproduz isso com tipos de tamanho fixo. Ainda falta confirmar contra pacotes capturados do cliente original ([KNCSDK › Pendências](../porting/kncsdk.md#pendências)).
