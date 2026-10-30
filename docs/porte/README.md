[Início](../README.md) › Porte

# Porte

Uma página (ou subpasta) por módulo de `src/`: estado, o que mudou em relação ao original, decisões locais e pendências.
O plano geral está em [Projeto › Roadmap](../projeto/roadmap.md).

| Módulo | Estado | Página |
|---|---|---|
| `src/platform` | Linux e MinGW (Wine) ok com testes; MSVC não testado | [Plataforma](platform.md) |
| `src/kncsdk` | Linux e MinGW (Wine) ok com testes; MSVC não testado; validação de protocolo pendente | [KNCSDK](kncsdk.md) |
| `src/server/Common` | compila no Linux (GCC/Clang) e no MinGW (Wine), sem warnings, com `Socket/` (TCP e UDP) e `ODBC/`; testes de pacotes, sessão, UDP e banco (SQL Server real, FreeTDS; só Linux) | [Servidor: Common](server-common.md); [Banco de dados (ODBC)](server-common-db.md) |
| `src/server/ChannelServer` | importado (2026-10-30), ainda não compila | [Servidor: ChannelServer](server-channel.md) |
