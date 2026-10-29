[Início](../README.md) › [Ferramentas](README.md) › Flags e verificação

# Flags e verificação (`gen_flags.py`, `check_equivalence.py`)

Duas ferramentas ligadas ao [Importador](importador.md):
- a primeira descobre **quais flags uma build original define de fato**;
- a segunda prova que o resolvedor de `#if` **não muda o código compilado**.

```bash
python3 tools/import/gen_flags.py server            # gera flags/server.generated.toml + relatório
python3 tools/import/check_equivalence.py server    # verificação diferencial em toda a árvore
python3 tools/import/check_equivalence.py server --show Common/BaseServer.cpp   # diff de um arquivo
python3 tools/import/tests/test_flagsim.py
```

## `gen_flags.py <perfil>`

O perfil (`tools/import/flags/<perfil>.profile.toml`) descreve a build original:
- o header de entrada;
- os defines do vcxproj;
- os defines extras de uma build de dev (`_IN_HOUSE_`);
- quais arquivos são headers de configuração.

1. **Simula** o pré-processador sobre os headers de configuração, seguindo `#include "..."` e em mundo fechado: o que não foi definido é falso.
   Roda duas vezes, em serviço e em dev (`x2import/flagsim.py`).
2. **Universo de flags**: todo nome que aparece em `#define`/`#undef` nos headers de configuração de *todos* os países, inclusive comentado (`//#define SERV_X`),
   mais todo nome com prefixo de flag (`SERV_`) usado em `#if` no código.
3. Remove do universo:
   - macros com parâmetros;
   - nomes tratados à mão (`base.toml`);
   - defines do build;
   - nomes que o código comum também define, que ficam sem resolução e são reportados.
4. Escreve `flags/<perfil>.generated.toml` (`true` = definido na build de serviço, `false` = o resto) e `reports/flags-<perfil>/REPORT.md`, com:
   - as flags que só existem em dev (as do whitelist viram `X2_DEV_TOOLS`);
   - as flags com valor;
   - as definidas localmente;
   - as usadas no código mas sem menção em configuração.

Ordem de carga num módulo do importador: `flags = ["base", "server.generated", "server"]`. **O arquivo carregado depois vence.**
`server.toml` é escrito à mão e tem as exceções do servidor (abaixo).

## `check_equivalence.py <perfil>`

Para cada arquivo da árvore:
1. pré-processa por completo o **original** sob a configuração real (resultado da simulação + exceções), respeitando os `#define`/`#undef` locais;
2. faz o mesmo com o **resolvido** pelo importador;
3. as linhas de código que sobrevivem têm de ser idênticas.

Ficam de fora da comparação as remoções intencionais: `#define`/`#undef` de flags, `#pragma comment(lib)` e linhas em branco.

Repete para as configurações serviço e dev, cada uma com dois cenários: macros desconhecidas (`_WIN32`, guards...) indefinidas, e todas definidas.

## Resultado no servidor (2026-10-29)

- `server.generated.toml`: **736 flags verdadeiras, 423 falsas**. Headers visitados: `ServerDefine.h` → `ServerDefine_Global.h` → `ServerDefine_US.h`.
- Só em dev: `DELETE_INCORRECT_ERROR_LOG` e `SERV_LOG_UNDEFINED_QUEST_TEMPLET`, ambos de log, que entram no whitelist de dev.
- Verificação: **1.415 de 1.423 arquivos equivalentes**.
  Os 8 restantes são os próprios headers de configuração. O `ServerDefine.h` liga `_IN_HOUSE_` em builds `_DEBUG`, papel que agora é do `X2_DEV_TOOLS`.
  Os headers de outros países testam flags que eles mesmos definem e nunca são compilados no US.

## Exceções do servidor (`flags/server.toml`)

| Flag | Estado | Por quê |
|---|---|---|
| `_SERVICE_` | falso | Os vcxproj `Release_US` dos servidores **não** o definem (só o cliente define). No servidor, `#ifndef _SERVICE_` era código ativo |
| `_DEB_AUTH` | verdadeiro | DEB ("Debelzak Custom Auth", customização do dono do projeto, com servidor SOAP próprio): autenticação (Channel) e cobrança (Game) usadas por este servidor, no lugar do publisher US. Definido só no Channel e no Game; todo arquivo que o testa só é compilado nesses dois |
