Implemente o passo `server/common-db` (ODBC portável, decisão T17). O branch `server/common-db` já existe e tem só o commit do desenho.

Antes de tudo:
1. Leia o AGENTS.md e as páginas do wiki que ele manda ler (Roadmap e Decisões).
2. Leia o desenho em `docs/porte/server-common-db.md`. É o plano deste passo: siga a ordem dos commits dela.
3. Leia `docs/porte/server-common.md` (Estado, Rede, Testes, Mudanças) e `docs/porte/platform.md` para o contexto do `Common` e da plataforma.

Pontos que não podem escapar:
- **A API do `KODBC` e as macros `DO_QUERY*`/`FETCH_DATA` não mudam.** Os servidores (1.828 `DO_QUERY`) vão usar o código sem alterações.
- `wchar_t` tem 4 bytes no Linux e `SQLWCHAR` tem 2. Toda passagem de texto entre os dois tipos passa pelas conversões de `x2/platform/odbc.h`, sem cast.
- Os indicadores são `SQLLEN`, não `SQLINTEGER`/`int`.
- Leitura de `std::wstring`/`std::string` em pedaços, sem o limite de 512 caracteres. `NULL` deixa a variável como estava.
- O `.dsn` vira connection string montada por nós (função pura, testável). O driver padrão é `ODBC Driver 18 for SQL Server`; `DBLayer:SetODBCDriver` e `DBLayer:SetODBCOptions` são opcionais. A senha nunca vai para o log.
- Remova os stubs de `KODBC` de `src/server/Common/tests/server_hooks.cpp`.
- Os testes sem banco entram no ctest. O de integração só roda com `X2_TEST_ODBC` definido; sem ele, o caso passa como "skipped".

Antes de instalar algo, pergunte ao usuário. São as duas perguntas do fim da página de desenho:
- pode instalar `msodbcsql18` (AUR) e/ou `freetds` e subir o SQL Server 2022 em Docker?
- qual padrão de `Encrypt`/`TrustServerCertificate` usar na conversão de `.dsn`?
Enquanto ele não responde, faça os passos 1 a 3 do desenho, que não dependem disso.

Regras da sessão (também estão no AGENTS.md e nas memórias):
- Comentários em inglês. Traduza os comentários coreanos que tocar.
- Builds: só o alvo afetado, incremental, com `-j` livre. A matriz completa (GCC Debug/Release, Clang num diretório do scratchpad, MinGW + Wine) só antes de integrar.
- Ao terminar, faça o commit sem perguntar; ajustes depois vão por amend.
- Ao concluir: transforme a página de desenho no registro do que foi feito e do que mudou em relação ao original. Atualize o Estado, as Mudanças e as Pendências de `server-common.md`, o roadmap e `platform.md`. Rode `python3 tools/docs/check_links.py`.
- Apague este arquivo (`.claude/prompts/server-common-db.md`) no último commit do branch. Integre na `main` por fast-forward só quando o usuário pedir.
