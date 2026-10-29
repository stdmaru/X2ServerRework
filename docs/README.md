# X2ServerRework — Documentação

Porte do servidor e do cliente de Elsword (`_reference/X2Server`) de Windows/DirectX9/MSVC
para uma base multiplataforma (Windows + Linux), limpando e reorganizando o código no caminho.
Assets do cliente (`.kom`, `.dds`, `.x`, `.fx`, ...) **não** são alterados — só código.

## Core (leia isto primeiro)

| Documento | Conteúdo |
|---|---|
| [core/missao-e-escopo.md](core/missao-e-escopo.md) | O que entra, o que fica de fora, critério de "pronto" |
| [core/roadmap.md](core/roadmap.md) | Fases do porte, em ordem, com entregáveis |
| [core/decisoes.md](core/decisoes.md) | Decisões tomadas e **decisões pendentes** |

## Análise detalhada (consulta)

| Documento | Conteúdo |
|---|---|
| [analise/inventario.md](analise/inventario.md) | Módulos do `_reference`, tamanho, dependências entre eles |
| [analise/bibliotecas.md](analise/bibliotecas.md) | Cada lib/3rdparty: o que é, versão, ação (substituir/portar/remover) |
| [analise/servidor.md](analise/servidor.md) | Acoplamento Windows no servidor (IOCP, ODBC, MFC, threads) |
| [analise/cliente.md](analise/cliente.md) | Acoplamento Windows/D3D9 no cliente (KTDXLIB, X2Lib, X2) |
| [analise/protocolo.md](analise/protocolo.md) | Serialização e riscos de compatibilidade de rede (tipos, wchar_t) |
| [analise/feature-flags.md](analise/feature-flags.md) | `SERV_*` / países / `_IN_HOUSE_` — o maior volume de "lixo" |
| [analise/debug-e-dev.md](analise/debug-e-dev.md) | Ferramentas de debug/dev/admin a manter (`X2_DEV_TOOLS`) |
| [analise/binarios-proprietarios.md](analise/binarios-proprietarios.md) | `.lib`/`.dll` sem fonte (anti-cheat, billing, Nexon...) |
| [analise/codificacao.md](analise/codificacao.md) | Encoding CP949, CRLF misto, convenções do código original |

## Ruído (provavelmente irrelevante, guardado por precaução)

- [ruido/fora-de-escopo.md](ruido/fora-de-escopo.md) — ferramentas e projetos do `_reference` que não vamos portar agora.

## Convenções destes docs

- `core/` só contém o que muda o rumo do projeto. Se algo é só referência, vai para `analise/`.
- Números (linhas, arquivos, contagens de `grep`) são de **2026-10-29**, sobre `_reference/X2Server`
  (commit `b8fb1380`). São estimativas de ordem de grandeza, não métricas exatas.
