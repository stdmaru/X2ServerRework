[Início](../README.md) › Porte

# Porte

Uma página (ou subpasta) por módulo de `src/`: estado, o que mudou em relação ao original, decisões locais e pendências.
O plano geral está em [Projeto › Roadmap](../projeto/roadmap.md).

| Módulo | Estado | Página |
|---|---|---|
| `src/platform` | Linux (Clang, GCC) e Windows (clang-cl) ok com testes | [Plataforma](platform.md) |
| `src/kncsdk` | Linux (Clang, GCC) e Windows (clang-cl) ok com testes; cripto própria (T27); validação de protocolo pendente | [KNCSDK](kncsdk.md) |
| `src/server/Common` | compila no Linux (Clang, GCC) e no Windows (clang-cl), sem warnings, com `Socket/` (TCP e UDP) e `ODBC/`; testes de pacotes, sessão, UDP e banco (SQL Server real, FreeTDS; só Linux) | [Servidor: Common](server-common.md); [Banco de dados (ODBC)](server-common-db.md) |
| `src/server/EPublisherServer` | reimplementação do AuthServer (DEB): compila sem warnings, testes no Linux e no Windows, roda contra o banco | [Servidor: EPublisherServer](server-epublisher.md) |
| `src/server/GameServer` | compila sem warnings no Linux (Clang, GCC) e no Windows (clang-cl); roda no Linux (`x2_run_game`); cliente original até a vila (2026-10-31) | [Servidor: GameServer](server-game.md) |
| `src/server/ChannelServer` | compila no Linux (Clang, GCC) e no Windows (clang-cl) sem warnings; roda no Linux (`x2_run_channel`); login do cliente original no canal funcionando (2026-10-30) | [Servidor: ChannelServer](server-channel.md) |
| `src/server/LoginServer` | compila sem warnings no Linux (Clang, GCC) e no Windows (clang-cl); roda no Linux (`x2_run_login`); com ele o cliente original chega à vila (2026-10-31) | [Servidor: LoginServer](server-login.md) |

**Ícones no Windows** (2026-10-31): cada executável de servidor leva o ícone do original (`icon.ico` na pasta do módulo, copiado do
`_reference` pelo git; o do EPublisherServer vem do `AuthServer` do DEB). O `x2_windows_icon` (`cmake/X2Compiler.cmake`) gera o `.rc`
com o mesmo `IDI_ICON1 ICON "icon.ico"` dos `<Servidor>.rc` originais; os diálogos MFC desses `.rc` não vêm (T15). No Linux não faz nada.
