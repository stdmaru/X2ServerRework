[Início](../README.md) › Porte

# Porte

Uma página (ou subpasta) por módulo de `src/`: estado, o que mudou em relação ao original, decisões locais e pendências.
O plano geral está em [Projeto › Roadmap](../projeto/roadmap.md).

| Módulo | Estado | Página |
|---|---|---|
| `src/platform` | Linux e MinGW (Wine) ok com testes; MSVC não testado | [Plataforma](platform.md) |
| `src/kncsdk` | Linux e MinGW (Wine) ok com testes; MSVC não testado; validação de protocolo pendente | [KNCSDK](kncsdk.md) |
| `src/server/Common` | compila no Linux (GCC/Clang, sem warnings), com `Socket/` (TCP e UDP); falta o `ODBC/`; testes de pacotes, sessão e UDP | [Servidor: Common](server-common.md); [Banco de dados (ODBC)](server-common-db.md), desenho |
