# Roadmap

Ordem pensada para ter algo **testável** o mais cedo possível. A peça-chave: o cliente original
(`_client/elsword.exe`, PE32 x86) já existe e funciona — então o servidor portado pode ser validado
contra ele **antes** de tocarmos no cliente. Isso só funciona se mantivermos o protocolo de rede
idêntico ([../analise/protocolo.md](../analise/protocolo.md)).

```
Fase 0  Repo novo + importador automático       (copia do _reference já limpo; sem mudar lógica)
Fase 1  Bibliotecas: substituir 3rdparty, portar KNCSDK
Fase 2  Servidor  → Linux/Windows, testado com o cliente original
Fase 3  Cliente   → camada de plataforma (SDL) + abstração gráfica
Fase 4  Cliente   → backend gráfico novo (Vulkan / SDL_GPU)
Fase 5  Limpeza profunda / reorganização
```

## Fase 0 — Repo novo + importador

O `_reference` **não é modificado** (fica só leitura). O código novo nasce do zero em `src/`, mas os arquivos
que mantemos **não são copiados à mão**: um script (`tools/import/`) lê o `_reference` e grava em `src/`
a versão já limpa. Motivo em [decisoes.md](decisoes.md) (T9).

O importador, por etapa:

1. **Seleção de arquivos**: lê os `.vcxproj` na config `Release_US|Win32` e pega só os fontes que realmente compilam
   (respeitando `ExcludedFromBuild`) + os headers que eles incluem. Arquivo que não entra no build não é importado.
2. **Encoding**: CP949 → UTF-8, fim de linha LF. Ver [../analise/codificacao.md](../analise/codificacao.md).
3. **Feature flags congeladas** com `coan` usando o conjunto final de macros da variante US/`_SERVICE_`
   (~1.175 flags `SERV_*`, ~58 mil linhas de `#if*`). Ver [../analise/feature-flags.md](../analise/feature-flags.md).
   **Exceção:** macros de debug/dev da whitelist e as checagens diretas de ambiente (`#ifndef _SERVICE_`, `_IN_HOUSE_`)
   viram `#if X2_DEV_TOOLS` e entram no relatório para triagem. Ver [../analise/debug-e-dev.md](../analise/debug-e-dev.md).
4. **Exclusões**: lista explícita de pastas/arquivos a não trazer (outros países, anti-cheat, SOAP, binários proprietários, DXUT...).
5. **Mapa de destino**: caminho no `_reference` → caminho em `src/` (onde a reorganização de pastas acontece).
6. Relatório: arquivos importados, descartados e flags resolvidas.

Regras:
- Correções que só um humano faz (portar IOCP, trocar tipos, etc.) acontecem **depois** da importação, em commits normais em `src/`.
- O importador roda **por módulo** (primeiro KNCSDK, depois `Common`, depois cada servidor), conforme cada fase precisa — não importamos tudo de uma vez.
- Re-rodar o importador sobre um módulo já editado não é permitido; se descobrirmos um erro de flag depois, corrigimos à mão em `src/`.
  (Antes da primeira edição manual de um módulo, re-rodar é livre.)
- A unificação de duplicatas (`KNCSDK_Server`×`KNCSDK_Client`, as duas pastas `Socket`, as 3 cópias de `lua_tinker`) é feita no mapa
  de destino: escolhe-se uma fonte e as diferenças da outra viram revisão manual.

Tarefas:
- [x] Layout do repo (`src/`, `cmake/`, `tools/import/`), CMake raiz, presets Linux/Windows x64, `.gitattributes`, `.editorconfig`.
- [x] Importador ([../../tools/import/README.md](../../tools/import/README.md)): seleção via vcxproj, encoding, resolvedor de `#if` próprio
      (no lugar do `coan`, por causa da regra `X2_DEV_TOOLS`), exclusões, mapa de destino, correção de includes, relatório, trava contra sobrescrita.
- [x] Módulo `kncsdk` importado: 62 arquivos de 742 ([relatório](../../tools/import/reports/kncsdk/REPORT.md)).
- [ ] Gerar o conjunto final de `SERV_*` a partir de `ServerDefine.h` (servidor) e `KTDX.h`/`Always.h` (cliente), e detectar conflitos entre os dois no código compartilhado (`Common`).
- [ ] Validar o resolvedor em escala: amostra de arquivos pré-processados (`-E`, com headers Windows como stub) antes/depois devem dar o mesmo código.
- [ ] Importar `server-common`, depois cada servidor.

Pendências levantadas pelo import do KNCSDK (vão para a Fase 1):
- `KNCSDK_Client` tem conteúdo que o Server não tem. O caso principal é `KSecureBuffer::*_Local` (SADatabase por sessão), e `lua_tinker.h` diverge bastante.
  O KNCSDK unificado precisa ser a **união** dos dois. Diffs em `tools/import/reports/kncsdk/alt-diff/`.
- `lua_tinker` existe em 3 lugares (`KNCSDK`, `LuaCommon`, `luaLib`): escolher uma base ao importar o cliente.

## Fase 1 — Bibliotecas

A maior parte não é "portar", é **substituir pela versão upstream** via CMake. Só o KNCSDK é código próprio.
Tabela completa: [../analise/bibliotecas.md](../analise/bibliotecas.md).

- [ ] Lua: uma única implementação com API 5.1 → **LuaJIT 2.1** (x64/ARM64); eliminar Lua 5.0.2, `luaLib` e Lua 5.1.4 embutido no KNCSDK.
- [ ] `lua_tinker`: uma cópia, compilando em GCC/Clang. Remover luabind (só 2 arquivos o usam).
- [ ] Crypto++ 8.9 upstream, zlib upstream, libxml2 upstream, FreeType upstream.
- [ ] Boost 1.91: reduzir ao mínimo; trocar por `std` onde trivial (thread, mutex, shared_ptr, filesystem, chrono).
- [ ] Intel TBB 2.2 → `std` ou oneTBB (só 2 usos).
- [ ] KNCSDK: `dbg/logfile` (log), `Serializer`, `KncSecurity` (wrapper Crypto++), `Thread`, `ObjectPool` → portáveis, com testes unitários.
      O `Serializer` é crítico para o protocolo — testes de "bytes idênticos" contra o original.
- [ ] Camada de plataforma mínima (`platform/`): tempo (`GetTickCount`/`timeGetTime`), threads, locks, paths, strings largas.

## Fase 2 — Servidor

Ver [../analise/servidor.md](../analise/servidor.md).

- [ ] Rede: IOCP → Asio (standalone) ou equivalente; preservar framing/cripto de pacotes.
- [ ] DB: ODBC → unixODBC + driver MS ODBC 18 no Linux (código de ODBC quase não muda).
- [ ] UI MFC (`Common/ui/ServerUIDlg`) → servidor headless (console + log + comandos).
- [ ] Remover SOAP/gSOAP, billing/auth de publishers, Mail(MIME/SMTP), CrashRpt; stub onde a lógica de jogo depende.
- [ ] Marco: login + vila + dungeon com o `elsword.exe` original contra servidor rodando em Linux.

## Fase 3 — Cliente: plataforma

Ver [../analise/cliente.md](../analise/cliente.md).

- [ ] Janela/input/IME: Win32 + DirectInput → SDL3. Manter, por enquanto, o modelo `MsgProc(HWND, UINT, WPARAM, LPARAM)`
      traduzindo eventos SDL em mensagens no estilo Win32 (X2Lib usa `HWND` em ~272 arquivos — quase todo pela assinatura de `MsgProc`).
- [ ] Matemática D3DX (`D3DXVECTOR3` ~8.000 usos) → shim header-compatível sobre GLM ou implementação própria.
- [ ] Som: FMOD Ex / DirectSound → miniaudio/SDL3 audio (ou FMOD Core atual, que é multiplataforma).
- [ ] Vídeo (Bandi), browser embutido, CxImage, Steam, anti-cheat → remover/substituir.
- [ ] `.kom` (zlib + header "KOG GC TEAM MASSFILE V.0.3") — loader portável.

## Fase 4 — Cliente: gráfico

- [ ] Introduzir abstração gráfica dentro de `KTDXLIB` (D3D9 está concentrado lá: ~56 arquivos usam o device; X2Lib quase só usa matemática).
- [ ] Reimplementar o que hoje vem de D3DX: loader `.x` (mesh + skin + animação), efeitos `.fx` (`x2.fx`), texturas `.dds`, fontes, sprites.
- [ ] Backend novo (Vulkan direto ou SDL_GPU — [decisoes.md](decisoes.md)).

## Fase 5 — Limpeza profunda

Refatoração de verdade (remoção de sistemas mortos, organização de pastas, nomes), guiada por testes/smoke tests
das fases anteriores. Só depois que tudo roda.
