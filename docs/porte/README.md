[Início](../README.md) › Porte

# Porte

Uma página (ou subpasta) por módulo de `src/`: estado, o que mudou em relação ao original, decisões locais e pendências.
O plano geral está em [Projeto › Roadmap](../projeto/roadmap.md).

| Módulo | Estado | Página |
|---|---|---|
| `src/platform` | Linux e MinGW (Wine) ok com testes; MSVC não testado | [Plataforma](platform.md) |
| `src/kncsdk` | Linux e MinGW (Wine) ok com testes; MSVC não testado; validação de protocolo pendente | [KNCSDK](kncsdk.md) |
| `src/server/Common` | compila no Linux (GCC/Clang, sem warnings), com o TCP de `Socket/`; falta o UDP e o `ODBC/`; testes de pacotes e de sessão | [Servidor: Common](server-common.md) |
