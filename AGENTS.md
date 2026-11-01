# AGENTS.md

Instruções para agentes (e pessoas) que trabalham neste repositório.

## O projeto

Porte do servidor e do cliente de Elsword (`_reference/X2Server-github`, que é só Windows/DX9/MSVC) para Windows + Linux,
com CMake e x64, limpando o código no caminho. Começa pelas bibliotecas, segue para o servidor e termina no cliente.

## Antes de começar qualquer tarefa

1. Leia a [página principal do wiki](docs/README.md) e as páginas de [Projeto](docs/projeto/README.md):
   [Roadmap](docs/projeto/roadmap.md) (o que está em andamento) e [Decisões](docs/projeto/decisoes.md) (o que já foi decidido).
2. Antes de mexer num módulo ou assunto, abra a página dele no wiki: [Porte](docs/porte/README.md) para módulos de `src/`
   e [Código original](docs/original/README.md) para o comportamento do `_reference`.
3. Não reabra uma decisão registrada em `decisoes.md`. Se algo novo contradiz uma decisão, aponte isso ao usuário.
4. Consulte o wiki sempre que precisar de contexto. Antes de investigar algo do zero, veja se já está documentado.
5. Ao alterar o código, certifique-se de manter o wiki atualizado.

## Regras fixas

- **`_reference/` é somente leitura.** Nunca edite, formate ou faça commit lá. `_reference/X2Server-github` aponta para um clone verificado
  de `github.com/stdmaru/X2Server`.
- **O `_reference/` será apagado no fim do projeto.** Nada (build, execução, testes, ferramentas) pode depender dele, só o importador
  ao trazer arquivos. O que os servidores precisam para rodar fica versionado (`runtime/`), menos binários e assets do cliente (decisão T22).
- **Código do original entra em `src/` apenas pelo importador** (`tools/import`, ver [Importador](docs/ferramentas/importador.md)).
  Depois que um arquivo importado é editado à mão, não reimporte o módulo. Corrija direto em `src/`.
- **Assets do cliente não mudam** (`_client/`, `.kom`, `.dds`, `.x`, `.fx`...). O código se adapta a eles.
- **Protocolo compatível com o cliente original** (`x2.exe`): não mude formato de pacote, tamanhos ou cripto sem uma decisão registrada.
  Tipos que vão para a rede usam tamanho fixo (`int32_t`, `char16_t`...). Ver [Protocolo](docs/original/protocolo.md).
- **Variante única: US / `_SERVICE_`.** As ferramentas de debug/dev/admin ficam sob `X2_DEV_TOOLS` (ver [Debug e dev](docs/original/debug-e-dev.md)).
  Outros países, anti-cheat, integrações de publisher e código legado são descartados.
- **Só 64 bits: x86_64 hoje, arm64 no futuro.** Windows (clang-cl) e Linux (Clang; GCC como verificação) compilam a partir do mesmo CMake
  (decisão T26). Os caminhos `_WIN32` são compilados e testados numa máquina Windows por SSH (`tools/win/winbuild.sh`) e no job **clang-cl** do CI.
  Nada de assembly/intrínsecos x86, nada de supor `char` com ou sem sinal (o build força `-fsigned-char`) e nada de sincronizar threads com `volatile`.
- `windows.h` e APIs de sistema só existem em `src/platform` (`x2::platform`). Código portado inclui `x2/platform/win32.h`
  no lugar de `<windows.h>`/`<atltime.h>`/`<tchar.h>`. Ver [Plataforma](docs/porte/platform.md).
- Módulos marcados `frozen = true` em `tools/import/modules/` já foram portados à mão: o importador se recusa a escrever neles.
- Build sem warnings (GCC, Clang e clang-cl). Warning novo é corrigido, não ignorado.

## Layout

```
src/            código novo (platform/, kncsdk/, depois server/ e client/)
cmake/          módulos CMake do projeto
tools/import/   importador _reference -> src/ (+ relatórios em reports/, manifests/)
tools/docs/     utilitários do wiki
tools/kom/      .kom/.kim do cliente (extrair, decompilar, empacotar)
tools/runtime/  geração do ClientScript a partir do cliente
runtime/        o que os servidores leem ao rodar (ServerResource, configs e .dsn como templates)
docs/           wiki do projeto
_reference/     código original (somente leitura, fora do git)
_client/        cliente original e assets (fora do git)
_database/      backups do SQL Server dos bancos do jogo (fora do git)
docker/         imagem de build Ubuntu (build/, build.sh) e serviços locais (SQL Server)
```

## Comandos

Builds e testes rodam na imagem Ubuntu do projeto (decisão T24): prefixe os comandos de build/teste com `docker/build.sh`
(sem argumentos abre um shell nela). Ver [Ambiente de build](docs/ferramentas/ambiente-build.md).

```bash
docker/build.sh cmake --preset linux-debug                            # configura (dev tools ON)
docker/build.sh cmake --build --preset linux-debug                    # build
docker/build.sh ctest --preset linux-debug                            # testes
docker/build.sh cmake --preset linux-gcc && docker/build.sh cmake --build --preset linux-gcc   # verificação com GCC
X2_WIN_HOST=<user@host> tools/win/winbuild.sh                        # Windows (clang-cl) na máquina de testes, por SSH
gh workflow run ci.yml --ref <branch>                                # CI do Windows, só à mão: minutos contados
python3 tools/import/import.py <módulo> --dry-run                     # importador
python3 tools/import/tests/test_condres.py                            # testes do importador
python3 tools/import/tests/test_flagsim.py
python3 tools/import/gen_flags.py server                              # flags do servidor
python3 tools/import/check_equivalence.py server                      # resolvedor não muda o código
python3 tools/docs/check_links.py                                     # links do wiki
docker compose -f docker/docker-compose.yml up -d --wait              # SQL Server de desenvolvimento
docker/mssql/restore.sh                                               # restaura _database/*.bak nele
docker/build.sh cmake --build --preset linux-debug --target x2_run_channel  # monta build/linux-debug/run/ChannelServer pronto para rodar
docker/build.sh cmake --build --preset linux-debug --target x2_run_epublisher  # idem, EPublisherServer (login/cash do DEB, T23)
```

O código original está em CP949. Para ler: `iconv -f cp949 -t utf-8 -c <arquivo>`. Alguns arquivos já estão em UTF-8.

## Código

- Código novo e comentários em inglês. Docs e mensagens de commit em português.
  Comentários em coreano do original são traduzidos para inglês ao mexer no trecho (ou ao copiá-los). Comente quando ajudar de verdade.
- Siga o estilo do código ao redor. Na limpeza, prefira a biblioteca padrão C++20 a Boost ou Win32.
- Toda mudança de comportamento em relação ao original precisa ser intencional e registrada na página do módulo em `docs/porte/`.

## Documentação

O wiki em `docs/` é a memória do projeto. Ele é organizado como **wiki navegável por índices**:

- **Página principal:** `docs/README.md`. Apresenta o projeto, aponta para onde começar e lista as áreas.
- **Áreas são pastas**, e cada uma tem um `README.md` que é o **índice** da área. Áreas atuais:
  `projeto/` (rumo: missão, roadmap, decisões), `original/` (análise do `_reference`),
  `porte/` (notas por módulo de `src/`), `ferramentas/` (ferramentas do projeto).
- **Assunto que cresce vira subpasta** com índice próprio (`README.md`), e assim por diante, aprofundando em níveis.
  Exemplo: `porte/server/README.md` → `porte/server/gameserver.md`.
- **Toda página nova entra no índice da sua pasta.** Página fora de índice não existe.
- **Toda página começa com a navegação** a partir da raiz: `[Início](../README.md) › [Área](README.md) › Título`.
- **Os índices separam o essencial da referência/ruído**, para que o leitor saiba o que precisa ler e o que é só consulta.
- **Uma informação, um lugar.** Não duplique: crie um link para a página dona do assunto.
  Decisões ficam só em `projeto/decisoes.md` (`T#` tomadas, `D#` pendentes), e o andamento só em `projeto/roadmap.md`.
- **Mantenha atualizado no mesmo trabalho:** ao concluir um passo, marque o roadmap. Ao descobrir algo sobre o original,
  anote na página da área. Ao portar um módulo, atualize a página dele em `porte/`.
- Use links relativos. Datas são absolutas (`2026-10-29`), nunca "ontem" ou "semana passada".
  Contagens e estimativas levam a data em que foram feitas.
- Depois de mover ou criar páginas, rode `python3 tools/docs/check_links.py`.

## Git

- Remoto: `origin` (GitHub, privado). Commit e push dos branches de trabalho sem perguntar (push de branch não dispara CI); integrar na `main` só quando o usuário pedir (depois, push da `main`). Nunca force-push na `main`.
- Mensagem: `tipo: resumo` (`feat`, `fix`, `docs`, `refactor`, `chore`), com corpo em português explicando o porquê.

### Branches

- **`main` está sempre verde**: compila sem warnings (GCC, Clang e clang-cl), `ctest` passa (Linux e Windows, no CI) e `tools/docs/check_links.py` não acusa nada. Nunca se trabalha direto nela.
- **Um branch curto por passo do roadmap**, com nome `área/assunto`. Áreas: `platform`, `kncsdk`, `server`, `client`, `tools`, `docs`, `build`.
  Exemplos: `tools/server-flags`, `server/common-import`, `build/windows-clang-cl`.
- **Integração por fast-forward** (`git merge --ff-only`), com histórico linear. Se a `main` andou, faça `git rebase main` no branch antes de integrar.
- Antes de integrar, rode o build limpo, os testes e o verificador de links, e o Windows (`tools/win/winbuild.sh`, ou um disparo do CI).
  O Actions tem **2.000 minutos/mês** (Windows conta 2×): o CI só roda sozinho na `main`; ver [Integração contínua](docs/ferramentas/ci.md#minutos-orçamento). Depois de integrar, apague o branch.
- Trabalho grande demais para um passo vira vários branches em sequência, não um branch longo.
