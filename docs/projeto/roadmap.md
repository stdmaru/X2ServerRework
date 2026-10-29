[Início](../README.md) › [Projeto](README.md) › Roadmap

# Roadmap

Ordem pensada para ter algo **testável** o mais cedo possível. A peça-chave: o cliente original
(`_client/elsword.exe`, PE32 x86) já existe e funciona — então o servidor portado pode ser validado
contra ele **antes** de tocarmos no cliente. Isso só funciona se mantivermos o protocolo de rede
idêntico ([Protocolo de rede e serialização](../original/protocolo.md)).

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
2. **Encoding**: CP949 → UTF-8, fim de linha LF. Ver [Codificação de arquivos e convenções do código original](../original/codificacao.md).
3. **Feature flags congeladas** com `coan` usando o conjunto final de macros da variante US/`_SERVICE_`
   (~1.175 flags `SERV_*`, ~58 mil linhas de `#if*`). Ver [Feature flags e variantes](../original/feature-flags.md).
   **Exceção:** macros de debug/dev da whitelist e as checagens diretas de ambiente (`#ifndef _SERVICE_`, `_IN_HOUSE_`)
   viram `#if X2_DEV_TOOLS` e entram no relatório para triagem. Ver [Ferramentas de debug / desenvolvimento / administração](../original/debug-e-dev.md).
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
- [x] Importador ([../../tools/import/README.md](../ferramentas/importador.md)): seleção via vcxproj, encoding, resolvedor de `#if` próprio
      (no lugar do `coan`, por causa da regra `X2_DEV_TOOLS`), exclusões, mapa de destino, correção de includes, relatório, trava contra sobrescrita.
- [x] Módulo `kncsdk` importado: 62 arquivos de 742 ([relatório](../../tools/import/reports/kncsdk/REPORT.md)).
- [ ] Gerar o conjunto final de `SERV_*` a partir de `ServerDefine.h` (servidor) e `KTDX.h`/`Always.h` (cliente), e detectar conflitos entre os dois no código compartilhado (`Common`).
- [ ] Validar o resolvedor em escala: amostra de arquivos pré-processados (`-E`, com headers Windows como stub) antes/depois devem dar o mesmo código.
- [ ] Importar `server-common`, depois cada servidor.

## Fase 1 — Bibliotecas

A maior parte não é "portar", é **substituir pela versão upstream** via CMake. Só o KNCSDK é código próprio.
Tabela completa: [Bibliotecas](../original/bibliotecas.md).

Andamento em 2026-10-29: plataforma e KNCSDK compilando no Linux (GCC e Clang), com testes. Detalhes em [Porte](../porte/README.md).

- [x] Camada de plataforma `x2::platform` (tipos, sync, tempo, `CTime`, `CString`, CRT, unicode), com testes. Ver [Plataforma](../porte/platform.md).
- [x] Dependências via CMake (T13): Crypto++ 8.9, zlib, LuaJIT 2.1 (pkg-config), doctest.
- [x] KNCSDK portado: log, `Serializer` (formato de rede fixo), `KncSecurity`, `Thread`, `ObjectPool`, `KncUtil`, unido ao `KNCSDK_Client`. Ver [KNCSDK](../porte/kncsdk.md).
- [x] `lua_tinker` (versão do servidor) compilando em GCC/Clang sobre LuaJIT 2.1; luabind removido.
- [ ] **Validar o protocolo com bytes reais** do cliente/servidor originais (hoje os testes usam bytes deduzidos do código).
- [ ] Compilar e testar no **Windows/MSVC** (nada foi validado lá ainda) e LuaJIT no Windows (D5).
- [ ] Boost: o KNCSDK não usa mais; o servidor ainda usa em ~230 arquivos. Reduzir ao portar o servidor.
- [ ] Intel TBB 2.2 → `std` (2 usos, no GameServer).
- [ ] libxml2 e FreeType upstream: só o cliente usa (Fase 3).

## Fase 2 — Servidor

Ver [Servidor — acoplamento com Windows](../original/servidor.md).

- [ ] Rede: IOCP → Asio (standalone) ou equivalente; preservar framing/cripto de pacotes.
- [ ] DB: ODBC → unixODBC + driver MS ODBC 18 no Linux (código de ODBC quase não muda).
- [ ] UI MFC (`Common/ui/ServerUIDlg`) → servidor headless (console + log + comandos).
- [ ] Remover SOAP/gSOAP, billing/auth de publishers, Mail(MIME/SMTP), CrashRpt; stub onde a lógica de jogo depende.
- [ ] Marco: login + vila + dungeon com o `elsword.exe` original contra servidor rodando em Linux.

## Fase 3 — Cliente: plataforma

Ver [Cliente — acoplamento com Windows / Direct3D 9](../original/cliente.md).

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
