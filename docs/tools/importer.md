[Início](../README.md) › [Ferramentas](README.md) › Importador `_reference` → `src/`

# Importador `_reference` → `src/`

Copia um módulo do código original já limpo. O `_reference` nunca é modificado (decisão T9).

```bash
python3 tools/import/import.py kncsdk            # importa e gera relatório
python3 tools/import/import.py kncsdk --dry-run  # só o relatório
python3 tools/import/tests/test_condres.py       # testes do resolvedor de #if
```

## O que ele faz, por arquivo

1. **Seleção**: fontes que o `.vcxproj` compila na config indicada (respeita `ExcludedFromBuild`), mais os headers listados,
   os `extra` do módulo e tudo o que eles incluem (fechamento transitivo) dentro de `source_root`. Fica de fora o que bate em `exclude`.
2. **Encoding**: UTF-8 / UTF-8 BOM / UTF-16 / CP949 → UTF-8 sem BOM. Também normaliza LF, remove espaço no fim das linhas e garante uma única quebra final.
3. **`#if`** (`tools/import/x2import/condres.py`): resolve as macros conhecidas (`tools/import/flags/*.toml`) e reescreve as macros de dev para `X2_DEV_TOOLS`.
   Além disso:
   - remove `#if 0`;
   - remove os `#define`/`#undef` das flags;
   - remove `#pragma comment(lib, …)`, que vai para o relatório;
   - converte `#endif FOO` em `#endif // FOO`.
4. **Includes**: resolve sem diferenciar maiúsculas e corrige a grafia (maiúsculas, `\` → `/`), porque no Linux o sistema de arquivos diferencia.
5. **Destino** pelas regras `[[map]]`. Hoje o layout espelha o original, para não ter de reescrever includes. Se algum include quebrar no destino, o relatório avisa.
6. **Relatório** em `tools/import/reports/<módulo>/REPORT.md`: arquivos e origem, blocos `X2_DEV_TOOLS` para triagem, includes corrigidos,
   dependências externas e diferenças contra `alt_root` (ignorando espaços; diffs em `tools/import/reports/<módulo>/alt-diff/`).

## Regras

- `tools/import/manifests/<módulo>.json` guarda o hash de cada arquivo escrito. Se um arquivo de `src/` foi editado depois da importação, rodar de novo **falha**.
  A partir daí, corrija à mão em `src/`. `--force` sobrescreve e perde as edições.
- Módulo com `frozen = true` no `.toml` já foi portado à mão: o importador recusa escrever nele, mesmo com `--force` (só `--dry-run`).
- **Integridade**: todo arquivo lido do `_reference` é comparado com o hash do git do próprio `_reference`. Se for diferente (cópia corrompida ou editada), a importação para.
  Isso foi adicionado depois que a cópia original no HDD mostrou 90 arquivos corrompidos ([Inventário](../original/inventory.md#integridade-da-cópia)).
- Os arquivos de módulo (`tools/import/modules/*.toml`) declaram `name`, `source_root`, `alt_root`, `include_dirs`, `flags`, `extra` e `exclude` **antes** de qualquer `[[tabela]]`.
  Num TOML, chaves escritas depois de uma tabela passam a pertencer a ela; o carregador recusa esse caso.

## Flags

- `tools/import/flags/base.toml`: ambiente (`_SERVICE_` → `!X2_DEV_TOOLS`, `_IN_HOUSE_` → `X2_DEV_TOOLS`), país US, anti-cheat e whitelist de dev.
- `server.generated.toml` + `server.toml`: flags do servidor, geradas por [`gen_flags.py`](flags.md). As do cliente (`KTDX.h`/`Always.h`) entram quando o cliente for importado.
- Uma flag fora dos arquivos carregados fica **intacta**: nunca é resolvida por palpite.
