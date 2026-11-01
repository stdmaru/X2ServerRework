[Início](README.md) › Compilar no Windows

# Compilar no Windows

O Windows compila com o **clang-cl** (o Clang no modo do MSVC), usando os headers, a STL e o linker do Visual Studio
(decisão T26). Só x64. Estado em 2026-10-31: compila sem warnings e os testes passam, mas **os servidores ainda não foram
testados rodando no Windows**, só no Linux.

## 1. Instalar

| O quê | Para quê |
|---|---|
| **Visual Studio 2026** (Community) ou o **Build Tools for Visual Studio 2026** | compilador, CMake e Ninja |
| **Git** | baixar o código |
| **Python 3** (opcional) | gerar o `ClientScript` a partir do cliente (ver o passo 4) |

No instalador do Visual Studio, marque:

- a carga **Desenvolvimento para desktop com C++** (*Desktop development with C++*);
- em *Componentes individuais*: **Compilador C++ Clang para Windows** e **Ferramentas de CMake do C++ para Windows**;
- um **Windows 11 SDK** (vem com a carga).

O mesmo pela linha de comando, com o instalador do Build Tools (`vs_BuildTools.exe`, baixado do site da Microsoft):

```bat
vs_BuildTools.exe --passive --wait ^
  --add Microsoft.VisualStudio.Workload.VCTools --includeRecommended ^
  --add Microsoft.VisualStudio.Component.VC.Llvm.Clang ^
  --add Microsoft.VisualStudio.Component.VC.CMake.Project
```

**Observação:** com mais de uma instalação do Visual Studio na máquina, abra o prompt **da que tem esses componentes**.
Uma instalação sem C++ dá `fatal error: 'cstdio' file not found` até nos headers padrão.

## 2. Pegar o código

```bat
git clone https://github.com/stdmaru/X2ServerRework.git
cd X2ServerRework
```

O cliente original (`_client\`) e os backups do banco (`_database\`) não fazem parte do repositório ([Rodar os servidores](running.md)).

## 3. Compilar e testar

Abra o **x64 Native Tools Command Prompt for VS 2026** (ou o *Developer Command Prompt*, com `-arch=amd64`).
O clang-cl do Visual Studio não fica no `PATH`, então ponha-o lá antes:

```bat
set PATH=%VSINSTALLDIR%VC\Tools\Llvm\x64\bin;%PATH%
cmake --preset windows-debug
cmake --build --preset windows-debug
ctest --preset windows-debug
```

| Preset | Tipo | Ferramentas de dev (`X2_DEV_TOOLS`) |
|---|---|---|
| `windows-debug` | Debug | sim |
| `windows-development` | RelWithDebInfo | sim |
| `windows-release` | Release | não |

- As dependências (zlib, LuaJIT, Asio, doctest) são baixadas pelo CMake na primeira configuração, que precisa de internet.
  O LuaJIT é compilado pelo `msvcbuild.bat` dele (T25).
- Os executáveis levam o ícone do servidor original.
- Para trocar de terminal ou de instalação do Visual Studio, apague `build\<preset>` e configure de novo.

## 4. O resultado

- Executáveis em `build\<preset>\src\server\<Servidor>\`.
- Pastas montadas em `build\<preset>\run\<Servidor>\`, como no Linux ([Rodar os servidores](running.md)). Diferenças no Windows:
  - O `ClientScript` só é gerado com o Python 3 e um `luajit.exe` no `PATH`. No Windows o build não compila o interpretador,
    só a biblioteca. Sem eles, o build avisa e segue. A pasta também pode ser gerada no Linux e copiada.
  - Os configs usam o **Microsoft ODBC Driver 18 for SQL Server**, que precisa estar instalado para os servidores
    acessarem o banco.

## Compilar de outra máquina (opcional)

Quem desenvolve no Linux pode compilar e testar numa máquina Windows por SSH, sem sair do Linux:

```bash
X2_WIN_HOST=usuario@host tools/win/winbuild.sh
```

Requisitos da máquina e detalhes: [Ambiente de build](tools/build-environment.md#windows-clang-cl-por-ssh).
