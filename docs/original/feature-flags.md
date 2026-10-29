[Início](../README.md) › [Código original](README.md) › Feature flags e variantes

# Feature flags e variantes

## Tamanho do problema

- ~**1.175** macros `SERV_*` distintas em servidor + cliente.
- ~**58.600** linhas de `#if/#ifdef/#ifndef` só em `KncWX2Server` + `X2Lib`.
- `#ifdef SERV_*` aparece em 874 arquivos do servidor e 324 do X2Lib.
- `KncWX2Server/Common/ServerDefine.h` tem **4.645 linhas**, ~700 `#define` (133 comentados), cada um com bloco de comentário coreano
  (autor, data, descrição). Equivalente no cliente: `KTDXLIB/Always.h`, `KTDX.h`, `OnlyGlobal/Always_*.h`.

## Eixos de variação

| Eixo | Macros | Origem |
|---|---|---|
| País | `SERV_COUNTRY_{US,EU,JP,TW,HK,TWHK,CN,TH,ID,BR,PH,IN}`, `CLIENT_COUNTRY_*`, `SERV_INT_INTERNAL` | vcxproj por configuração |
| Ambiente | `_SERVICE_`, `_IN_HOUSE_`, `_IN_HOUSE_SERVICE_READY_QA_`, `_OPEN_TEST_`, `_OPEN_TEST_2_`, `_DEBUG`, `X2TOOL`, `EFFECT_TOOL` | vcxproj + `ServerDefine.h:4389` |
| Global | `SERV_GLOBAL_COMMON` (+ `SERV_GLOBAL_AUTH/BILLING/FRIEND/CASH_PACKAGE`) | `ServerDefine.h:4608` → inclui `OnlyGlobal/ServerDefine/ServerDefine_<país>.h` |
| Features | `SERV_<FEATURE>` — eventos, sistemas, correções datadas | `ServerDefine.h`, `ServerDefine_<país>.h` |
| Build | `BUILD_BY_HUDSON`, `_CONVERT_VS_2010` | vcxproj |

O cliente tem ~40 configurações no `X2Lib.vcxproj` (`US_INTERNAL`, `EU_SERVICE`, `JP_TEST`...).

## Estratégia: congelar com ferramenta, não à mão

1. Variante escolhida: US / `_SERVICE_` (decisão T5 em [Decisões](../projeto/decisoes.md)).
2. Extrair o conjunto efetivo de macros definidas: compilar `ServerDefine.h`/`Always.h` com `-dM -E` usando os defines do vcxproj alvo.
3. Rodar **`coan source`** (ou `unifdef`) com `-D`/`-U` de todas as macros conhecidas → remove os ramos mortos e as próprias diretivas.
4. Apagar `ServerDefine_<outros países>.h`, `OnlyGlobal/*/<outros países>`, `ServerDefine.h` vira um arquivo pequeno (ou some).
5. Compilar e comparar comportamento (smoke test) antes/depois.

**Exceção de debug:** macros de dev da whitelist e checagens diretas de `_SERVICE_`/`_IN_HOUSE_` não são resolvidas;
viram `#if X2_DEV_TOOLS`. Ver [debug-e-dev.md](debug-e-dev.md).

Cuidados:
- Macros usadas em `#if` com expressões (`defined(A) && !defined(B)`) — `coan` lida bem; `unifdef` também.
- Macros **definidas e depois `#undef`** (ex. `SERV_INT_INTERNAL` desliga `SERV_TIME_DROP_MONSTER_EVENT`) — usar o estado final.
- Algumas flags podem estar em arquivos `.lua` de recurso ou em SQL — não são afetadas pelo `coan`.
