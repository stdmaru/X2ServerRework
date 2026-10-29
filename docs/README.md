# X2ServerRework — Wiki

Porte do servidor e do cliente de Elsword (`_reference/X2Server`), que hoje é só Windows/DirectX 9/MSVC,
para uma base multiplataforma (Windows + Linux), limpando e reorganizando o código no caminho.
Os assets do cliente (`.kom`, `.dds`, `.x`, `.fx`, ...) **não** são alterados: o porte é só de código.

## Comece por aqui

1. [Missão e escopo](projeto/missao-e-escopo.md): o que entra e o que fica de fora.
2. [Roadmap](projeto/roadmap.md): fases e o que está em andamento.
3. [Decisões](projeto/decisoes.md): tomadas e pendentes.

## Áreas

| Área | Assunto |
|---|---|
| [Projeto](projeto/README.md) | Missão, roadmap, decisões. **Essencial.** |
| [Código original](original/README.md) | Análise do `_reference`: módulos, libs, acoplamento com Windows, protocolo, flags, debug |
| [Porte](porte/README.md) | Notas de trabalho por módulo portado (estado, mudanças, pendências) |
| [Ferramentas](ferramentas/README.md) | Ferramentas do próprio projeto (importador) |

## Convenções do wiki

- Cada área é uma pasta com um `README.md`, que é o índice da área. Assunto grande vira subpasta com índice próprio.
- Toda página começa com um caminho de navegação (`Início › Área › Página`).
- Os índices marcam o que é **essencial** e o que é **referência/ruído**.
- Números (linhas, arquivos, contagens) são estimativas de ordem de grandeza, com a data indicada na página.
- Regras completas para quem edita o wiki: [AGENTS.md](../AGENTS.md#documentação).
