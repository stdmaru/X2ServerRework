# Decisões

## Tomadas

| # | Decisão | Motivo |
|---|---|---|
| T1 | Assets do cliente não mudam; o código se adapta aos formatos atuais | Pedido do projeto |
| T2 | Ferramentas (`X2Tool`, patcher, `*DBManager`...) ficam fora do porte inicial | Foco em servidor + cliente + libs |
| T3 | Bibliotecas de terceiros entram pela versão **upstream**, não a cópia do `_reference` | As cópias são antigas, modificadas e só-Windows |
| T4 | Compatibilidade de rede com o `x2.exe`/`elsword.exe` original (antiga D1) | Validar o servidor sem ter o cliente portado |
| T5 | Variante **US / `_SERVICE_`** (antiga D2) | Config já usada no porte VS2026 do `_reference` |
| T6 | Alvo **x64** (antiga D5) | — |
| T7 | Manter **MSSQL via ODBC** por enquanto (antiga D6) | Migrar SGBD é projeto à parte |
| T8 | Ordem: **bibliotecas → servidor → cliente** | Servidor é menor e testável com o cliente original |
| T9 | `_reference` fica **intocado**; código novo do zero em `src/`, alimentado por um **importador automático** (seleção por vcxproj + CP949→UTF-8 + `coan` + exclusões) | Limpar o original exigiria compilar no Windows/MSVC x86 a cada passo; copiar à mão 2 M linhas é inviável e perderia a limpeza das flags. O importador dá o melhor dos dois: repo limpo desde o 1º commit e transformação reproduzível |

| T10 | Base `_SERVICE_`, **mantendo ferramentas de debug/dev/admin** sob uma flag nova `X2_DEV_TOOLS` (build `Development`). Descartar o resto: flags não usadas, legado, outros países, anti-cheat | `_IN_HOUSE_` mistura debug com gameplay não lançado; whitelist + relatório em [../analise/debug-e-dev.md](../analise/debug-e-dev.md) |

## Pendentes

### D3 — Gerenciamento de dependências (layout já definido)
Layout em uso: `src/kncsdk`, depois `src/server/{common,login,center,channel,game,global}` e `src/client/{engine,game,app}`.
Na Fase 0, cada módulo espelha a estrutura original (sem reescrever includes); a reorganização interna fica para a Fase 5.

Dependências — proposta: `find_package` primeiro (Arch tem `luajit`, `zlib`, `libxml2`, `freetype2`, `sdl3`, `fmt`) e, se não achar,
fallback para `FetchContent` (`FIND_PACKAGE_ARGS`). No Windows o fallback cobre tudo, e o vcpkg fica opcional.
Casos especiais: Crypto++ (sem CMake upstream → `cryptopp-cmake`) e LuaJIT (Makefile → build custom). Decidir no início da Fase 1.

### D4 — Backend gráfico
- **SDL3 + SDL_GPU** — mais simples, Vulkan/D3D12/Metal de graça; shaders em SPIR-V/HLSL via SDL_shadercross.
- **Vulkan direto** — mais controle, muito mais código.
- **Ponte temporária** (DXVK-native d3d9 no Linux) — o cliente já roda em DXVK (`x2.dxvk-cache` em `_client/data`), mas não resolve D3DX nem é o objetivo final.

Recomendação: SDL3 para janela/input/áudio e **SDL_GPU** para render; decidir de vez na Fase 3.
