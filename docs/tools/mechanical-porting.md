[Início](../README.md) › [Ferramentas](README.md) › Porte mecânico

# Porte mecânico (`tools/port`)

Scripts para a passada mecânica sobre um módulo **depois** de importado (ver [Importador](importer.md)).
Diferente do importador, eles editam `src/` no lugar: rode, revise o diff e faça commit. Criados em 2026-10-29
para o `server/common-build` ([Servidor: Common](../porting/server-common.md)).

| Script | O que faz | Entrada |
|---|---|---|
| `mechanical.py` | Includes Win32/ATL/D3DX/winsock → plataforma, Boost → `std`/`x2::`, `BOOST_FOREACH` → range-for, colagens `##` inválidas | pasta do módulo |
| `fix_typename.py` | Insere o `typename` que o GCC pede (`need 'typename' before ...`), na coluna indicada | log do build |
| `gen_packet_tests.py` | Gera `src/server/Common/tests/test_packets_*.cpp` (ida e volta de todos os tipos serializados) e confere a simetria `PUT`/`GET`. `--check` só verifica (é um teste do ctest) | `.cpp` dos pacotes |
| `fix_warnings.py` | `-Wswitch` → `default: break;`; `-Wunused-parameter` → `/*nome*/`; `-Wreorder` → ordena a lista de inicialização na ordem de declaração | log do build |

Uso típico:

```bash
cmake --build --preset linux-debug --target x2_server_common -- -k 0 > build.log 2>&1
python3 tools/port/fix_typename.py build.log
python3 tools/port/fix_warnings.py build.log --only switch,unused-parameter,reorder
```

## Cuidados

- **Colunas do GCC são de exibição** (tab = 8 colunas), não bytes. Os scripts convertem (`display_to_index`).
- No `-Wswitch` o GCC aponta o parêntese, não a palavra `switch`: o script procura o `switch` na mesma linha.
  Uma versão anterior procurava adiante e editava o `switch` seguinte.
- No `-Wreorder` a ordem vem da declaração **no nível da própria classe**, usando a linha que o GCC informa.
  Só os inicializadores trocam de lugar; vírgulas e comentários ficam onde estavam. A execução não muda, porque os membros
  sempre foram inicializados na ordem de declaração.
- Cada local é corrigido uma vez, mesmo que vários arquivos `.cpp` o reportem. Uma correção pode revelar outra: compile de novo.
- **Use o log de um build limpo** (`--clean-first`) para ver todos os warnings: o build incremental só mostra os dos arquivos recompilados.
