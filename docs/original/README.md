[Início](../README.md) › Código original

# Código original (`_reference/X2Server`)

Análise do código de referência, feita em 2026-10-29 sobre o commit `b8fb1380`.
É material de consulta: as decisões que saem daqui ficam em [Projeto › Decisões](../projeto/decisoes.md).

## Visão geral

| Página | Conteúdo |
|---|---|
| [Inventário](inventario.md) | Módulos, tamanhos, grafo de dependências, dados |
| [Bibliotecas](bibliotecas.md) | Cada lib/3rdparty: versão e ação (substituir/portar/remover) |

## Acoplamento com a plataforma

| Página | Conteúdo |
|---|---|
| [Servidor](servidor.md) | IOCP, ODBC, MFC, threads, tempo |
| [Cliente](cliente.md) | D3D9/D3DX, input, som, IME, vídeo |
| [Protocolo](protocolo.md) | Serialização e compatibilidade de rede (`long`, `wchar_t`) |
| [Binários proprietários](binarios-proprietarios.md) | `.lib`/`.dll` sem fonte: anti-cheat, billing, publishers |

## Limpeza

| Página | Conteúdo |
|---|---|
| [Feature flags](feature-flags.md) | `SERV_*`, países, ambientes: o maior volume de código morto |
| [Debug e dev](debug-e-dev.md) | Ferramentas de debug/admin a manter (`X2_DEV_TOOLS`) |
| [Codificação](codificacao.md) | CP949, fim de linha, convenções do código original |
| [Bugs do original](bugs.md) | Comportamento errado do original mantido no porte: pendências da Fase 5 |

## Ruído

| Página | Conteúdo |
|---|---|
| [Fora de escopo](fora-de-escopo.md) | Ferramentas e projetos que não serão portados agora |
