[Início](../README.md) › [Código original](README.md) › Ferramentas de debug / desenvolvimento / administração

# Ferramentas de debug / desenvolvimento / administração

Objetivo (decisão T10): build base = **`_SERVICE_`**, mas **manter** tudo que serve para administrar,
desenvolver ou depurar. Pode ser redesenhado; não precisa ser fiel ao original.

## Como o original controla isso

Ambiente escolhido por define no vcxproj, interpretado em `KTDXLIB/KTDX.h`:

| Macro | Significado (comentários do `KTDX.h`) |
|---|---|
| `_SERVICE_` | patch de produção |
| `_SERVICE_` + `_OPEN_TEST_` | servidor de teste aberto |
| `_IN_HOUSE_` | build interna **com** código de teste |
| `_IN_HOUSE_SERVICE_READY_QA_` | build interna **sem** código de teste (QA) |

O debug aparece de três formas:

1. **Macros derivadas**: cada bloco de ambiente no `KTDX.h` define macros de feature, e `_IN_HOUSE_` inclui
   `InHouseEtc.h` e `InHouse1..6.h`, um arquivo por desenvolvedor.
2. **Checagem direta** no código: `#ifndef _SERVICE_` / `#ifdef _IN_HOUSE_` / `_OPEN_TEST_` (~82 / 68 / 40 arquivos no cliente).
3. **Servidor**: comandos de admin são **pacotes** (`EGS_ADMIN_*`, 33 tipos: kick, mudar ED/level/classe, criar NPC,
   completar quest, observer mode, auth level...) verificados por nível de autorização em tempo de execução.
   Quase não dependem de `#ifdef`, então seguem com o porte normal. No original, a janela MFC do servidor também
   tinha funções de operação; elas viram comandos de console.

## Problema: `_IN_HOUSE_` não é só debug

`InHouse*.h` mistura ferramentas de dev com **mudanças de gameplay não lançadas** e correções experimentais.
Exemplos: `BALANCE_PATCH_20131107`, `UPGRADE_TRADE_SYSTEM`, `OPTION_UI_RENEWAL`, `FIX_MEMORY_LEAK`, `REGULATE_STATE_CHANGE_LOGIC`.
Ligar `_IN_HOUSE_` inteiro mudaria o jogo e quebraria a compatibilidade com o `x2.exe` original (T4).
Por isso a regra não pode ser "liga tudo de in-house".

## Estratégia no importador

Uma flag nossa, **`X2_DEV_TOOLS`**: ligada na build `Development`, desligada na `Release`.

1. Todo o resto é resolvido como na build `_SERVICE_` (US).
2. **Whitelist de macros de debug**: elas não são resolvidas, e o importador reescreve o teste para `X2_DEV_TOOLS`.
   Candidatas (lista inicial, a revisar):
   - itens/inventário: `ITEM_CHEAT_POPUP_TEST` (janela de compra de itens via cheat), `ITEM_VIEW_CHEAT`, `AUTH_AUTO_SELL_ITEM`, `AUTH_DELETE_ITEM_ALL`, `JUNK_AVATAR`
   - personagem/buffs: `ERASE_BUFF_CHEAT`, `CHEAT_CLASS_CHANGE`, `VILLAGE_FLASH_MAN_DEV_ONLY`, `DAMAGE_ZERO`
   - comandos: `CHEAT_COMMAND_SHORT_CUT_KEY_TEST`, `ADD_SHORT_CUT_KEY_IN_HOUSE`
   - monstros/mapas: `SUMMON_MONSTER_BY_PRESS_KEY_AT_TRAINING_GAME`, `MONSTER_TEST_DUNGEON`, `CREATE_MONSTER_LIST_TEST`, `MONSTER_STATE_LIST_TEST`, `FREE_SELECT_PVP_MAP`, `ENABLE_SET_UNIT_POSITION_TEST`
   - visualização: `SHOW_ATTACK_BOX_DUMMY`, `IN_GAME_MANUAL_CAMERA_POSITION_TEST`, `SHOW_ONLY_MY_DAMAGE`
   - logs: `ADD_DAMAGE_LOG`, `WRITE_SCRIPT_LOG_AT_CONSOLE_WINDOW`, `LOG_LOAD_DLG_SCRIPT`, `ADD_RESOURCE_ERROR_LOG`, `EFFECT_USE_LOG`
   - scripts: `REALTIME_SCRIPT_PATCH` (recarregar script em tempo real), `EXPAND_DEVELOPER_SCRIPT`
   - conveniência: `AUTO_LOGIN_IN_HOUSE`, `KHELP_SYSTEM_TEST`, `X2OPTIMIZE_INDICATE_LOCAL_MASSFILE`/`MASS_FILE_FIRST` (ler arquivo solto antes do `.kom`)
3. **Checagens diretas** (`#ifndef _SERVICE_`, `#ifdef _IN_HOUSE_`) viram `#if X2_DEV_TOOLS` por padrão.
   O importador gera um **relatório** com cada bloco convertido para triagem manual. Bloco que não for de debug
   (gameplay, outro país, anti-cheat) é resolvido como `_SERVICE_`.
4. Tudo o que estiver em `InHouse*.h` e **não** entrar na whitelist é descartado, ou seja, fica como no `_SERVICE_`.
5. Requisito: com `X2_DEV_TOOLS` ligado, o cliente **continua compatível** com o servidor e o protocolo. Debug não pode mudar formato de pacote.
   Se alguma feature exigir isso, ela fica fora da whitelist e é refeita depois.

## Depois do porte (redesenho, Fase 5+)

- Juntar os cheats espalhados num **console/painel de dev** único no cliente (comandos + janelas: itens, monstros, posição, buffs).
- Servidor: console de admin (kick, anúncio, reload de scripts, status) no lugar da UI MFC.
- Os pacotes `EGS_ADMIN_*` continuam sendo a fonte da verdade no servidor, sempre checando o nível de autorização.
- Avaliar o uso de ImGui no cliente para as janelas de dev.

## A verificar

- Lista completa de blocos `#ifndef _SERVICE_` no X2Lib (sai do relatório do importador).
- Onde o cliente decide mostrar cheats: por macro ou também pelo auth level recebido do servidor.
- Encoding misto confirmado: `KTDX.h` está em UTF-8, enquanto `InHouse*.h` estão em CP949. A detecção tem de ser por arquivo.
