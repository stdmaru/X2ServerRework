[Início](../README.md) › [Porte](README.md) › Console dos servidores

# Console dos servidores (`KServerConsole`, `x2ctl`)

O que fica no lugar da janela MFC do original (`Common/ui/ServerUIDlg`, ver [Servidor](../original/servidor.md)): o terminal da
decisão T15 e o protocolo de controle da T30, a base do ServerManager. Código: `src/kncsdk/ServerConsole.{h,cpp}` (`KServerConsole`),
sobre o `x2::platform::Terminal` e o `x2::platform::ControlServer` ([Plataforma](platform.md)); `src/server/x2ctl`. Feito em 2026-10-31
(`server/terminal`, `platform/terminal-windows`, `server/control`).

O `KServerMain` abre o console antes do `Init` (a janela original existia durante a carga), com o nome do executável como nome (a janela se
chamava `GameServer`...), e o fecha depois do `ShutDown`. O EPublisherServer usa o mesmo console, com as mesmas opções.

## Modos

| Modo | Quando | Comportamento |
|---|---|---|
| Terminal | stdin e stdout são um terminal | Entrada crua e editor de linha (←/→, Home/End, Ctrl+A/E/U/K/W, Ctrl+←/→, ↑/↓ no histórico, Ctrl+L limpa a tela). O log (`SetConsoleWriter`) e tudo o que vai para stdout/stderr (um pipe, como o `CHookStdio` do original) saem acima do prompt, que é redesenhado. Título `<nome>_Loading`, depois `_Ready` (consultado a cada ~100 ms, como o laço de 5 ms do original). Ctrl+C limpa a linha; com a linha vazia, Ctrl+C ou Ctrl+D perguntam `Are you sure you want to exit? [y/N]` (texto do original) |
| Linhas | stdin é pipe ou arquivo | Cada linha é um comando Lua, sem prompt nem eco; o fim do stdin encerra |
| Máquina | `--machine` | stdin e stdout falam o [protocolo](#protocolo): o stdout só leva JSON (o log humano é desligado e o stdout/stderr soltos viram mensagens `output`); o fim do stdin encerra |
| Serviço | `--no-stdin` | Não lê nada; espera SIGTERM/SIGINT/SIGHUP (Windows: Ctrl+C, fechamento do console, logoff, desligamento) |

Em todos os modos o servidor também escuta o **socket de controle** (abaixo), salvo com `--no-control`. Opções do console não chegam ao
servidor como argumentos: `--no-stdin`, `--machine`, `--control=<nome>` (nome do socket; o padrão é o do executável) e `--no-control`.

- Comando como no `lua.c`: uma expressão imprime o valor (`1+1` → `2`), o resto roda como instrução; no terminal o erro sai em vermelho.
  O original rodava `luaL_dostring` e descartava o erro.
- Teclas digitadas durante a carga esperam o prompt; um Ctrl+C nesse intervalo abre a pergunta assim que o prompt aparece.
- SIGTERM e o fechamento do console encerram sem perguntar. No Windows, o fechamento dá ~5 s para o `ShutDown` (limite do sistema).
- Num terminal, uma falha fatal (SIGSEGV, SIGABRT...) devolve o modo do terminal antes de terminar, para não deixar o shell em modo cru.
- Linux: `termios` sem `ECHO`/`ICANON`/`ISIG` (o processamento da saída fica). Windows: `ReadConsoleW` com `ENABLE_VIRTUAL_TERMINAL_INPUT`
  (as teclas chegam como texto e sequências VT, o mesmo decodificador do Linux), porque o UTF-8 pelo stdin do conhost não é confiável.
- Sem biblioteca de edição de linha (T13): as opções (readline, linenoise, replxx) não cobrem o console do Windows ou trazem bem mais
  do que o necessário; o editor é pequeno e testado (`test_terminal.cpp`).
- `x2_terminal_demo` (`src/kncsdk/tests/terminal_demo.cpp`, compilado com os testes): o mesmo console sem servidor nem banco (socket
  `TerminalDemo`), com uma linha de log a cada 2 s e o estado pronto depois de 3 s, para testar à mão. Testado no Linux e no Windows.

## Socket de controle

Um socket Unix (`AF_UNIX`, que o Windows tem desde o 10 1803: um caminho de código só, em vez de *named pipe*), numa pasta só do usuário:
`$XDG_RUNTIME_DIR/x2/<nome>.sock` (ou `/tmp/x2-<uid>/`, modo 0700) no Linux, `%TEMP%\x2\<nome>.sock` no Windows. Fala o mesmo
[protocolo](#protocolo) com qualquer número de clientes, que recebem todos o log e o status; o resultado de um comando vai só para quem o pediu.

- Segundo servidor com o mesmo nome: o socket não abre (aviso no log) e o servidor segue sem ele; use `--control=<outro>`. Um arquivo de
  socket sem ninguém atrás (processo que caiu) é substituído.
- Cada cliente tem uma fila e uma thread de escrita; um cliente com mais de 8 MB parados na fila é desligado, para nunca travar o log.
- Autenticação e criptografia ficam com o SSH (T30): o socket só aceita o usuário local.
- No modo serviço não há desvio do stdout, então quem se liga não recebe as mensagens `output` (só o log).
- O lado do servidor nunca bloqueia num `accept`/`recv`/`send`: espera com `poll` (`WSAPoll`) de 100 ms e um sinal de parada, com os
  sockets dos clientes não bloqueantes. No Windows, fechar um socket `AF_UNIX` de outra thread não acorda quem está bloqueado nele, e o
  `Stop` travava (achado na VM em 2026-10-31). O `Stop` dá até 2 s para a fila de cada cliente sair (o `status` final).
- No Windows, o arquivo do socket é um *reparse point* que o `std::filesystem` não consegue consultar (`exists` lança exceção): a sobra de
  um processo que caiu é reconhecida tentando conectar.

## `x2ctl`

Compilado com os servidores e copiado para `run/x2ctl`. Testado no Linux e no Windows (2026-10-31); no Windows os argumentos são lidos
em UTF-16 (`x2::platform::Utf8Arguments`), senão o coreano de um `x2ctl run` chegava como `?`.

| Comando | Faz |
|---|---|
| `x2ctl list` | os nomes dos servidores escutando |
| `x2ctl attach <nome>` | liga o stdin e o stdout ao socket (linhas do protocolo): `ssh host x2ctl attach GameServer` é como o ServerManager vai falar com um servidor remoto |
| `x2ctl run <nome> <lua...>` | um comando; imprime o que ele imprimiu e o erro no stderr; código 1 se o Lua falhou |
| `x2ctl stop <nome>` | encerra o servidor e espera ele terminar |

## Protocolo

JSON, um objeto por linha (UTF-8), igual no `--machine` e no socket. Versão 1 (o `hello` traz `protocol`).

Do servidor:

| `type` | Campos | Quando |
|---|---|---|
| `hello` | `protocol`, `server` (nome), `pid`, `state` | primeira linha (no socket, antes de qualquer outra) |
| `log` | `time` (`AAAA-MM-DD hh:mm:ss`), `level` (`error`, `warn`, `info`, `note`), `stream` (`cerr`, `cout2`...), `func`, `file`, `line`, `message`, `fields` (`[{"name","value"}]`, do `BUILD_LOG`), `text` (as outras linhas) | cada registro do [log](kncsdk.md#log-t16) no nível do console (`consolelog( n )` muda) |
| `output` | `text` | uma linha escrita no stdout/stderr fora do log (`std::cout`, `printf`) |
| `status` | `state`: `ready`, `stopping` (o `ShutDown` começa), `stopped` (o console fecha) | mudança de estado |
| `result` | `id` (o do pedido, ou `null`), `ok`, `output` (o que o comando imprimiu com `print`), `error` | resposta a um comando |

Do cliente: `{"id":1,"lua":"print(1)"}` (o `id` é qualquer número ou string e volta no `result`), `{"id":2,"stop":true}` (encerra) ou
uma linha de Lua pura (resposta com `id` `null`). Só o `print` é capturado na resposta; o que o comando escreve de outro jeito
(`io.write`, `os.execute`) sai como `output`. Os comandos do socket rodam na thread do prompt (a dona do estado Lua), entre as teclas.

Exemplo (`x2ctl attach ChannelServer`, encurtado):

```
{"type":"hello","protocol":1,"server":"ChannelServer","pid":577544,"state":"loading"}
{"type":"log","time":"2026-10-31 20:21:31","level":"note","stream":"cout","func":"SetPacketAuthFailLimit_LUA","file":".../NetLayer.cpp","line":181,"message":"Packet Auth Fail Limit : 100","fields":[],"text":[]}
{"type":"status","state":"ready"}
{"id":1,"lua":"GetLocalIP()"}
{"type":"result","id":1,"ok":true,"output":"127.0.0.1\n","error":""}
```
