[Home](../README.md) › [Original code](README.md) › Network protocol and serialization

# Network protocol and serialization

## Locations

- Packet definitions: `KncWX2Server/Common/ClientPacket.{h,cpp}`, `CommonPacket.{h,cpp}`, `OnlyGlobal/Packet/*_Global.cpp`,
  IDs in `EventID_{Client,Server,System}.h` (+ `_Global`). **Shared** with the client (`X2Lib` includes `KncWX2Server/Common`).
- Serialization: `KNCSDK_*/Include/Serializer/` (`Serializer.h`, `SerBuffer`, `Ser{Vector,Map,Set,List,Deque,Pair}.h`, `KeyedSerializer.h`, `ByteOrder`).
- Transport: `Common/Socket/{Session,X2Packet,NetCommon}` and `X2ServerProtocol/Socket` (client). Encryption via `KncSecurity` (Crypto++).

## The problem

The serializer writes primitive types using `WriteBytes(&x, sizeof(T))` (see `Serializer.h:129`), so the on-wire format
**is the MSVC x86 memory layout**. When compiling with GCC/Clang on x64:

| C++ type | Win32 MSVC | Linux x64 | Effect |
|---|---|---|---|
| `long` / `unsigned long` | 4 B | **8 B** | corrupted packet (49 uses in `ClientPacket.h`/`CommonPacket.h` alone) |
| `wchar_t` / `std::wstring` | 2 B (UTF-16) | **4 B** (UTF-32) | all strings break (`Put(const std::wstring&)`) |
| `time_t` (`_USE_32BIT_TIME_T` in client) | 4 B | 8 B | dates |
| `bool`, `enum` | 1 B / 4 B | 1 B / 4 B | okay, but enums without explicit underlying types should get `: int32_t` |
| `DWORD`, `UINT`, `__int64` | 4 / 4 / 8 | need typedefs | define as `uint32_t`/`int64_t` |
| pointers/`size_t` in structs | 4 B | 8 B | audit whether any are serialized |

## Proposed strategy

1. Fixed typedefs in a platform header (`DWORD=uint32_t`, `LONG=int32_t`, ...).
2. Explicit overloads in `Serializer` converting `long` → `int32_t` and `std::wstring` → UTF-16LE (`char16_t`) **on the wire**.
   Internally, code can continue using `std::wstring` for now.
3. `static_assert(sizeof(...))` on serialized POD structs.
4. **Identical-byte test**: capture packets from the original client/server (or serialize fixtures in the `_reference` MSVC x86 build)
   and compare them with output from the ported code.
5. Endianness: all targets are little-endian; `ByteOrder.cpp` can be simplified.

The same concern applies to **ODBC bindings** (`SQLBindParameter` with `SQL_C_LONG` + 8-byte `long`) and reading client binary files
(`.kom`, mesh headers) into structs.

## Format confirmed in code (2026-10-29)

Read from the original `KNCSDK` during the port; tests in `src/kncsdk/tests/test_serializer.cpp` encode this.

| Item | Network format |
|---|---|
| `short`, `int`, `DWORD`/`UINT`, `INT64`/`UINT64`, `float`, `double` | **big-endian** (`ByteOrder.h`: `BYTEORDER_NET = BIG`), 2/4/8 bytes |
| `char`, `unsigned char`, `bool` | 1 byte (`bool` = 0/1) |
| `wchar_t` | 2 bytes **little-endian** (raw memory, no byte swap) |
| `std::string` | `DWORD` (BE) with size in bytes + bytes |
| `std::wstring` | `DWORD` (BE) with size **in bytes** + UTF-16 **LE** |
| STL containers | `DWORD` (BE) with item count + items |
| tags | 1 byte before each value, only if serializer is in `tagging` mode |
| compressed `KSerBuffer` | raw **LE** `DWORD` with original size + zlib stream (level 1) |
| TCP frame (`KSession`) | total size (raw **LE** `USHORT`, includes its own 2 bytes) + secure header; frames larger than `MAX_PACKET_SIZE` (32 KB) drop the connection |
| first packet (server → client) | `E_ACCEPT_CONNECTION_NOT` with new SPI and `KSecurityAssociation`, encrypted with the default key; after that, both sides use the new SPI |
| UDP datagram (`CKTDNUDP`) | 1-byte ID (`X2_PACKET_TYPE` or `SP_*`) + struct copied as memory (`#pragma pack(1)`, little-endian, no encryption), up to 2,048 bytes |
| secure header (`KSecureBuffer`) | SPI (`unsigned short`, raw LE) + sequence number (`unsigned int`, raw LE) + IV + encrypted data (DES-CBC) + ICV (HMAC-MD5) |

In other words, numbers go through `HTON`, while values copied as memory (`wchar_t`, secure header, compression size) remain little-endian.
The port reproduces this with fixed-width types. Still to confirm against captured packets from the original client
([KNCSDK › Pending](../porting/kncsdk.md#pending)).
