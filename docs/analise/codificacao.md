# Codificação de arquivos e convenções do código original

## Encoding

Amostra de ~400 `.cpp` (servidor + cliente) via `file`:
- maioria aparece como "ISO-8859" / "Non-ISO extended-ASCII" → na prática **CP949 (EUC-KR)**: comentários e literais em coreano.
- uma minoria já está em UTF-8 (alguns com BOM, ex. `luajitLib/ReadMe.txt`), e parte é ASCII puro.
- fim de linha misto: CRLF, LF e CRLF+LF no mesmo arquivo; um caso com `NEL`.

Conversão sugerida (Fase 0):
1. Detectar por arquivo (`uchardet`/`charset-normalizer`): ASCII/UTF-8 ficam; resto `iconv -f CP949 -t UTF-8`.
2. Revisar diffs de **literais de string** (`L"..."`, `"..."`) — o MSVC compilava com code page 949; ao mudar para UTF-8 o binário muda.
   Strings visíveis ao jogador normalmente vêm de tabelas (`StringID_def*.h`, `.lua`, `.kom`), não do código, mas confirmar.
3. Normalizar para LF + `.gitattributes`; adicionar `.editorconfig`.
4. Compilar com `/utf-8` no MSVC.

Ler arquivos originais no terminal: `iconv -f cp949 -t utf-8 -c <arquivo>`.

## Convenções observadas

- Prefixo `K` (KNC = empresa KOG / "KNC"): `KActor`, `KGSUser`, `KTDX*` (engine), `CX2*` (jogo, cliente).
- Blocos de mudança marcados com comentários `//{{ autor : data` ... `//}}` e cabeçalho por feature em `ServerDefine.h`
  (`이름/아이디/작업시작날짜/패치날짜/작업내용`). São histórico de SVN embutido no código → candidatos a remoção junto com as flags.
- Código comentado em grande volume (ex. várias chamadas `uncompress` comentadas em `KGCMassFileManager.cpp`).
- Precompiled headers `stdafx.h` em todos os projetos.
- Projetos antigos coexistindo: `.vcproj` (VS2003/2005), `*_2003.vcxproj`, `.dsp/.dsw` (VC6), `.vspscc/.vssscc` (SourceSafe) → lixo.
