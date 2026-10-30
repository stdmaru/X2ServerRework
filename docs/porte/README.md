[Início](../README.md) › Porte

# Porte

Uma página (ou subpasta) por módulo de `src/`: estado, o que mudou em relação ao original, decisões locais e pendências.
O plano geral está em [Projeto › Roadmap](../projeto/roadmap.md).

| Módulo | Estado | Página |
|---|---|---|
| `src/platform` | Linux e MinGW (Wine) ok com testes; MSVC não testado | [Plataforma](platform.md) |
| `src/kncsdk` | Linux e MinGW (Wine) ok com testes; MSVC não testado; validação de protocolo pendente | [KNCSDK](kncsdk.md) |
| `src/server/Common` | compila no Linux (GCC/Clang) e no MinGW (Wine), sem warnings, com `Socket/` (TCP e UDP) e `ODBC/`; testes de pacotes, sessão, UDP e banco (SQL Server real, FreeTDS; só Linux) | [Servidor: Common](server-common.md); [Banco de dados (ODBC)](server-common-db.md) |
| `src/server/EPublisherServer` | reimplementação do AuthServer (DEB): compila sem warnings, testes no Linux e no Wine, roda contra o banco | [Servidor: EPublisherServer](server-epublisher.md) |
| `src/server/ChannelServer` | compila no Linux (GCC/Clang) e no MinGW sem warnings; roda no Linux até o estado pronto com o banco (`x2_run_channel`); cliente não testado | [Servidor: ChannelServer](server-channel.md) |
