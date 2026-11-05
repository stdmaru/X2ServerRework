[Home](../README.md) › [Porting](README.md) › `.kom` archives

# `.kom` archives (`src/kom`, targets `x2::kom`, `x2::kom_write`)

The client's assets live in 173 `.kom` archives (2026-11-05: 2.2 GB, 101,826 entries, ~5.2 GB decoded). Decision T35 makes
`src/kom` the one implementation of the format, shared by the client, the launcher and the tools; `tools/kom/kom.py` stays as
its test oracle (and for the server package's `ClientScript`, [Server package](../tools/runtime.md)) until a C++ CLI replaces
it. The format itself is described in `x2/kom/archive.h` and at the top of `tools/kom/kom.py`; the format variants of other
client versions are in [Inventory](../original/inventory.md). What is inside the entries is not this library's business:
Lua bytecode and `Item.kim` are read and written by [`x2::lua`](lua.md), used by [client_data](../tools/client-data.md#lua).

| Target | Header | For | Contents |
|---|---|---|---|
| `x2::kom` | `x2/kom/codec.h` | everyone | `Decode`, `XorCrcDecrypt`/`Encrypt`, `Adler32`, `AsciiLower` |
| | `x2/kom/archive.h` | everyone | `Archive`: open, list, `Find`, `ReadStored`, `Read`, `RawHeader`, `Close` |
| | `x2/kom/mount.h` | client, launcher | `Mount` (a folder of archives by entry name), `ArchivesInLoadOrder` |
| `x2::kom_write` | `x2/kom/writer.h` | tools, launcher | `Encode`, `ArchiveBuilder` |

Dependencies: `x2::platform` (`ReadOnlyFile`, UTF-8/UTF-16) and zlib. No libxml2: the XML header is a flat list, read by a small
strict parser (`src/header.cpp`).

## Reading

- `Archive::Open` reads the 72-byte fixed header and the XML header only, never the whole archive. It rejects other magics and
  versions (V.0.1/V.0.2: "unsupported .kom version"), a compressed flag other than 1,
  an XML header whose adler32 does not match, an entry count different from the XML's, and entries past the end of the file.
- Entries have name, decoded size, stored size, checksum (adler32 of the stored blob), time and algorithm, all available without
  decompressing (the launcher's verification).
- `ReadStored`/`Read` use positional reads (`pread`, `ReadFile` with an offset), so any number of threads read one archive at
  once; the original kept one Win32 handle per registered thread (`MASS_FILE_MANAGER_MAX_NUM_THREADS` = 2). Every read checks the
  blob's adler32 against the header.
- Algorithms: 0 zlib, 2 XORCRC (the 2014 client's Lua), 3 XORCRC3 (the August 2014 and later files: raw deflate from byte 6);
  1 (zlib + Blowfish, unused) is kept opaque: listed, copied, not decoded.
- <a id="xml-header"></a>**XML header layouts** found in the client's files (2026-11-05, all 173 read and round-tripped):

  | Archives | Prolog | Attributes | End of `<File>` | `FileTime` |
  |---|---|---|---|---|
  | 115 (KOG's packer) | `<?xml version="1.0"?>` | `Name Size CompressedSize Checksum FileTime Algorithm` | ` />` | = `Checksum` |
  | 58 (another packer) | `<?xml version="1.0" ?>` | alphabetical | `/>` | `00000000` |

  The parser accepts both, single or double quotes, entity references, `<File></File>`, comments and NUL padding. Every
  archive's header `FileTime` is the sum of its entries' times, and no blob failed its checksum.
- Names are compared with ASCII case folding, and lower-cased with ASCII rules for the cipher, as the client (`MakeUpperCase`
  for the map, `std::tolower` in the "C" locale for XORCRC).

## Writing

`ArchiveBuilder` (in `x2::kom_write`, so the client does not link it) collects entries copied verbatim from open archives
(`From`, `AddCopy`) and new data (`Add` encodes with zlib level 9 as `kom.py`, or the level given to `SetLevel`; `AddStored`
takes a blob already encoded), in order; a name added again replaces its entry in place. **KOG's packer used zlib level 6**
(`kKogLevel`): with stock zlib 1.3.1, 90,103 of the client's 101,826 entries (88.5%) re-encode byte for byte at level 6. The
other 11,723 (763 MB stored, mostly `.tga` in the equipment archives `data015`–`020`, `107`, `131`, `143`) have a standard
zlib header but a stream that no zlib level, memLevel, window size or strategy reproduces, in zlib 1.3.1, 1.2.11 or 1.2.3,
nor zlib-ng: another compressor wrote them (2026-11-05). `Write`:

- writes `<destination>.tmp` and renames it over the destination, so a failure leaves the original untouched; blobs copied from
  archives are checked against their checksums while copying;
- **byte-identical** output when nothing changed (every entry of one archive, in its order): the original fixed and XML headers
  are copied as they were, whatever the packer that wrote them; so is a header given to `SetHeader` that still describes the
  entries exactly (`Describes`: count, order, names, sizes, checksums, times, algorithms), which is how
  [client_data](../tools/client-data.md) reproduces archives from re-encoded files;
- otherwise generates KOG's layout (first row above), deterministically: new entries get `FileTime` = checksum unless given;
- closes source archives whose file is the destination right before the rename (rewriting in place; reopen afterwards);
- fails clearly ("is it open in another program, such as a running client?") when the destination cannot be replaced: on Windows
  `ReadOnlyFile` and the original client open archives with `FILE_SHARE_READ` only. POSIX has no such lock (the running client
  keeps reading the old file);
- refuses archives over 2 GB (the client reads 32-bit offsets) and entry names that are empty, not UTF-8, or contain `/`, `\` or
  control characters (names in a `.kom` have no folders).

A file written by `x2::kom` from the same inputs is byte-identical to `kom.py`'s `write_kom` (tested).

## Mounting

<a id="mounting"></a>`Mount` looks entries up by name across archives, as the client's `KGCMassFileManager`:

- **Load order**: `ArchivesInLoadOrder` sorts a folder's `.kom` names case-insensitively, which gives the client's order:
  `data001`…`data150`, then `datai001`…`datai023` (`CX2Main`'s fixed list, `X2Main.cpp`).
- **A name in more than one archive: the first one read wins**, and the warning sink gets
  `<name> is in both <first> and <later>; using <first>` (a repeat inside one archive: `appears twice`). The default sink writes
  to stderr. The client's 173 archives have no repeated name (2026-11-05).
- Lookups are thread-safe once mounting is done.

## The original (`KTDXLIB/KGCMassFileManager.*`), surveyed 2026-11-05

What the client step will replace (roadmap Phase 3), with the Win32 it uses:

- **Registration**: `CX2Main` builds a fixed list of 173 names (`data%03d.kom` 1–150, `datai%03d.kom` 1–23), calls
  `AddMassFile("./<name>")` for each and then `LockMassFileMap()`. The release client crashes on purpose (`*x = 1`) when one is
  missing or rejected. At start it also **deletes every `.kom` (and `.kom_`) not in the list**, recursively under the game folder
  (`DeleteOtherFile`).
- `CMassFile::LoadMassFile` opens the archive twice (`CreateFileW`, `FILE_SHARE_READ`; release retries twice after 300 ms), reads
  the header with `SetFilePointer` + `ReadFile`, and parses the XML with **libxml2** (`xmlReadMemory`), keeping only `Name`, `Size`
  and `CompressedSize` (`sscanf %d`). Names are converted with `MultiByteToWideChar(CP_ACP)`, **truncated to 59 characters**
  (`WCHAR[60]`) and upper-cased. `data110.kom` has a 61-character name
  (`Trigger_Hamel_Frozen_Water_Hall_Stage3_Stone_Statue_Water_A.x`) that the original can therefore never find; `x2::kom` keeps
  full names. No checksum is checked, the header's or the entries'.
- **Duplicates**: `AddMassFile` rejects the whole later archive when one of its names is already mapped, with a message box; with the release client's crash on failure, a repeated name stopped the game. T35 changes this to first-wins with a
  warning.
- **Decoding**: `_GetMemberFile` inflates with zlib `uncompress`. **The `Algorithm` attribute is ignored**: the caller decides.
  `CKTDXApp::LoadAndDoMemory(..., bEncryption)` asks for the stored blob (`MFI_COMPRESSEDDATA`), decrypts it with
  `XORCRCDecrypt` (`KTDXCommonFunc.h`, keyed by the upper-cased name) and inflates it; algorithm 3 does not exist in this client.
  `x2::kom` decodes by the attribute instead.
- **Buffers**: `KGCMassFileBufferManager` hands out per-thread reusable buffers (`KGCMassFileBufferPtr`); data returned by
  `LoadDataFile` lives in them.
- **Loose files**: dev builds (`X2_DEV_TOOLS`, the original's `_DEBUG_`/not `_SERVICE_`) fall back to files on disk **only when the
  name is in no `.kom`** (`LoadRealFile`): `SetSubDirectory` registers the current folder and every subfolder, recursively
  (`FindFirstFileW`; except `.svn`, `Branches`, `epay`, `SupportFiles`), and the first folder holding the name wins
  (`CreateFileW`). Such files are flagged `MFI_LOCALFILE` and not decrypted. `*_LocalFile` variants read only loose files, in
  every build. T35 reverses the priority: dev builds prefer the loose files in `/data/data`.
- `ExternFileMapping`/`MassFileMapping` (`AddMappingList_LUA`, `AddMappingTime_LUA`) rename requested files by date ranges
  (event assets); `SaveMassFile`/`ModifyMassFile` are an old V.0.1-style writer that the game does not call. `KFileVerifier`
  (patcher checksum thread) is compiled but nothing creates it.

## Tests (`ctest -R kom`)

`x2_kom_tests`, headless, on archives the tests build (no client assets in the repository):

- codec: XORCRC/XORCRC3 against vectors from `kom.py` (including a non-ASCII name), round trips, every algorithm with sizes 0 to
  300 KB, wrong sizes and corrupted blobs rejected, algorithm 1 refused;
- archives: listing from the header, every algorithm and an opaque entry, byte-identical rewrite of both header layouts, rewrite
  with changes in place, corrupted or foreign files (magic, version, header checksum, count, truncation, XML errors, entry
  checksum), entity references, name validation, 8 threads reading one archive; on Windows, replacing an archive held open
  fails and leaves it untouched;
- mount: load order, first-wins with the warning text, repeats inside one archive;
- `kom.py` (when CMake finds Python 3): each reads what the other writes, and both write identical bytes;
- **opt-in**: `X2_KOM_CLIENT_DATA=<client data folder> x2_kom_tests -tc="*client*"` mounts, decodes and rewrites every archive
  there and compares the bytes. 2026-11-05, `_client/data`: 173 archives, 101,826 entries, 5.19 GB decoded, all identical, no
  repeated name; 4 min 46 s in `linux-debug`.

`x2::platform`'s `ReadOnlyFile` has its own test (`test_file.cpp`: positional and concurrent reads, short reads, errors).

## Next steps (roadmap Phase 3, one branch each)

Packing `client_data` for distribution (only the changed archives) is x2 studio's export to `.kom`
([roadmap](../project/roadmap.md#later--x2-studio)).


1. Import the client into `client_data` + a manifest that repacks to the original archives byte for byte: done,
   [client_data](../tools/client-data.md).
2. The client's `KGCMassFileManager` on `x2::kom`: mount `/data` in load order; with `X2_DEV_TOOLS`, index `/data/data` by name
   recursively and prefer it. Decide then what replaces `DeleteOtherFile` and the crash on a missing archive.
