[Início](../README.md) › [Código original](README.md) › Bugs do original

# Bugs do original (pendências da Fase 5)

Comportamento errado que **já existia no original** e que o porte **mantém de propósito**, para não mudar o jogo no meio do porte.
São consertados na [Fase 5](../projeto/roadmap.md#fase-5--limpeza-profunda), cada um com uma decisão sobre o comportamento novo.

Os bugs que o porte **já corrigiu** (leitura fora do array, `%s` com `std::wstring`...) não entram aqui: ficam na seção
"Bugs do original corrigidos" da página do módulo em [Porte](../porte/README.md).

## Código

| Onde | Bug | Efeito no jogo | Achado |
|---|---|---|---|
| `Common/X2Data/XSLSkillTree.cpp`, `LoadSkillTemplet_Stat` | A defesa mágica por nível é lida da tabela `m_DefPhysic`, e não da `m_DefMagic` (o bloco foi copiado e só o vetor mudou) | Skills com bônus de stat dão defesa mágica igual à física. As skills de guilda 500 e 501 têm `m_DefMagic` própria, que é ignorada | 2026-10-31, `server/game-run` |
| `Common/X2Data/XSLSkillTree.cpp`, `LoadSkillTemplet_Stat` | A checagem de tamanho está invertida (`empty() != false && size() != SLV_MAX_LEVEL`): recusa só a tabela vazia, e uma tabela com menos de 30 níveis passa | O original lia fora do vetor nos níveis que faltam. O porte põe 0 nesses níveis ([GameServer](../porte/server-game.md#bugs-do-original-corrigidos)); falta decidir se a tabela curta é válida (as skills de guilda têm 10 níveis) | 2026-10-31, `server/game-run` |
| `GameServer/NewPartyListManager.cpp`, `KPartyListManager::IsPossibleLowLevel` | O `OUT bool isPartyNULL` é recebido por valor | Quem chama nunca vê o valor | 2026-10-30, `server/game-build` |
| `GameServer`, grau de guilda | Testa `== -1` num `u_char`, o que é sempre falso (o teste saiu no porte, com o mesmo efeito) | A conferir no `server/game-run`: o que a procedure devolve quando o personagem não tem guilda | 2026-10-30, `server/game-build` |
| `GameServer`, nível de skill e página de skill ativa | Testam `unsigned < 0`, sempre falso (os testes saíram no porte, com o mesmo efeito) | A validação de valor negativo nunca dispara | 2026-10-30, `server/game-build` |
| `LoginServer/GuildSkillTree.cpp`, `KGuildSkillTree::Init` | O nível de skill de guilda acima do máximo é corrigido numa variável local (`iSkillLevel`) que nunca é usada: a skill entra com o nível do banco | Uma skill de guilda gravada acima do nível máximo continua acima, só com o log de erro | 2026-10-31, `server/login-build` |

## Dados (`ServerResource`, `ClientScript`)

Erros ao carregar os scripts de produção com o `ClientScript` gerado do cliente (contagem do GameServer de 2026-10-31). O conjunto de
itens (49.837) e os scripts de dungeon foram conferidos: são os mesmos do `ClientScript` do servidor original, então os erros de item e
de dungeon já aconteciam em produção. O servidor sobe assim, ignorando as entradas ruins:

- Itens que não existem no `Item.lua` citados pelo `ServerResource`: 719 itens atrativos (`AddAttractionItemInfo`),
  650 grupos de item aleatório (`AddRandomItemGroup`, com a soma da probabilidade passando de 100%), 172 pacotes do cash com item repetido,
  41 itens selados e 17 de aviso de resultado.
- 19 pets (IDs 37 em diante) e 1 montaria (ID 11) que o script de criação cita e o template não tem.
- 24 scripts de dungeon citados pelo `DungeonData.lua` que nem o cliente nem o `ClientScript` original têm (tutoriais `ElTutorial_*`,
  treinos `Training_*`, `Besma_MINE2_*`, `Elder_NewYear`); o `Dungeon_Event_KidDay.lua` existe só no `ClientScript` original.
- 2 linemaps de campo (`Field_Velder_Stage2/3_LineMap.lua`) que não existem.
- A procedure `dbo.P_GItem_Def_GET` (ferramenta de GM) não existe no banco de desenvolvimento.
