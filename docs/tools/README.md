[Início](../README.md) › Ferramentas

# Ferramentas do projeto

| Página | Conteúdo |
|---|---|
| [Importador](importer.md) | `tools/import`: copia módulos do `_reference` para `src/` já limpos (encoding, flags, includes) |
| [Porte mecânico](mechanical-porting.md) | `tools/port`: passada mecânica e correção de erros/warnings guiada pelo log do build (`typename`, `switch`, `reorder`...) |
| [Banco de desenvolvimento](development-database.md) | `docker/`: SQL Server 2022 em Docker com os bancos de `_database/`, connection strings e os testes do `KODBC` contra ele |
| [Pastas de execução](runtime.md) | `runtime/` e os alvos `x2_run_*`: pasta pronta para rodar cada servidor, `ClientScript` gerado do cliente, `tools/kom` |
| [Ambiente de build](build-environment.md) | `docker/build.sh` e `docker/build/Dockerfile`: imagem Ubuntu onde rodam todos os builds e testes, local e no CI (T24) |
| [Integração contínua](ci.md) | `.github/workflows/ci.yml`: build sem warnings e testes em Clang, GCC e clang-cl (Windows) a cada push |
| [Flags e verificação](flags.md) | `gen_flags.py` (quais flags a build original define) e `check_equivalence.py` (prova que o resolvedor não muda o código) |
