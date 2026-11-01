[Início](../README.md) › Código original

# Código original (`_reference/X2Server`)

Análise do código de referência, feita em 2026-10-29 sobre o commit `b8fb1380`.
É material de consulta: as decisões que saem daqui ficam em [Projeto › Decisões](../project/decisions.md).

## Visão geral

| Página | Conteúdo |
|---|---|
| [Inventário](inventory.md) | Módulos, tamanhos, grafo de dependências, dados |
| [Bibliotecas](libraries.md) | Cada lib/3rdparty: versão e ação (substituir/portar/remover) |

## Acoplamento com a plataforma

| Página | Conteúdo |
|---|---|
| [Servidor](server.md) | IOCP, ODBC, MFC, threads, tempo |
| [Cliente](client.md) | D3D9/D3DX, input, som, IME, vídeo |
| [Protocolo](protocol.md) | Serialização e compatibilidade de rede (`long`, `wchar_t`) |
| [Binários proprietários](proprietary-binaries.md) | `.lib`/`.dll` sem fonte: anti-cheat, billing, publishers |

## Limpeza

| Página | Conteúdo |
|---|---|
| [Feature flags](feature-flags.md) | `SERV_*`, países, ambientes: o maior volume de código morto |
| [Debug e dev](debug-and-development.md) | Ferramentas de debug/admin a manter (`X2_DEV_TOOLS`) |
| [Codificação](encoding.md) | CP949, fim de linha, convenções do código original |
| [Bugs do original](bugs.md) | Comportamento errado do original mantido no porte: pendências da Fase 5 |

## Ruído

| Página | Conteúdo |
|---|---|
| [Fora de escopo](out-of-scope.md) | Ferramentas e projetos que não serão portados agora |
