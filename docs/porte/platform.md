[Início](../README.md) › [Porte](README.md) › Plataforma

# Plataforma (`src/platform`, alvo `x2::platform`)

Única parte do código que fala direto com o sistema operacional (regra do [AGENTS.md](../../AGENTS.md)).
Código novo, sem equivalente no original. Criada em 2026-10-29 (decisão T11).

## Ideia

O original usa APIs Win32/ATL em massa. Contagem só no servidor:

| API | Uso |
|---|---|
| `CTime` | ~1.700 vezes |
| `DWORD` | ~900 |
| Eventos/`WaitForSingleObject` | ~50 arquivos |
| `CString` | ~300 |

Reescrever tudo isso agora seria inviável e arriscado. Então a camada oferece **substitutos com os mesmos nomes e semântica**:
- **no Windows**, usa o `windows.h` real, e este é o único lugar onde ele é incluído;
- **no Linux**, emula o subconjunto usado.

A modernização (trocar por `std::` direto) fica para a Fase 5.

Código portado inclui `x2/platform/win32.h` no lugar de `<windows.h>`, `<atltime.h>` e `<tchar.h>`.

## Conteúdo

| Header | O que oferece | Linux | Windows |
|---|---|---|---|
| `types.h` | `DWORD`, `BYTE`, `LONG`, `INT64`, `TCHAR`, `_T()`, `MAX_PATH`... com os tamanhos do Windows (`DWORD`/`LONG` = 32 bits) | typedefs | `windows.h` |
| `sync.h` | `CRITICAL_SECTION` (recursiva), eventos, `WaitForSingle/MultipleObjects`, `_beginthreadex`, `Sleep`, `GetCurrentThreadId` | emulação (um mutex/condvar global) | nativo |
| `time.h` | `GetTickCount`, `timeGetTime`, `QueryPerformanceCounter`, `GetLocalTime`, **`CTime`/`CTimeSpan`** | `steady_clock` / implementação própria | nativo + `CTime` próprio |
| `string.h` | `CStringW`/`CStringA`/`CString` (subconjunto) | implementação própria | idem (sem ATL) |
| `crt.h` | `_wtoi`, `swprintf_s`, `wcscpy_s`, `_wcsicmp`, `_itow_s`, `_mkdir`... | implementação própria | nativo |
| `unicode.h` | wide ↔ UTF-8 ↔ UTF-16; `TranslateWideFormat` | — | — |
| `debug.h` | `__WFILE__`, `__WFUNCTION__` em tempo de execução | — | — |

## Armadilhas que a camada resolve

- **`%s` em formato largo**: no MSVC, `%s` em `swprintf`/`CString::Format` significa string *larga*; na glibc significa *estreita* (crash).
  `TranslateWideFormat` converte `%s`→`%ls`, `%c`→`%lc`, `%S`→`%s` e `%I64d`→`%lld`, e é usada por `CString::Format` e pelos `*printf_s`.
- **`L ## __FUNCTION__`** só existe no MSVC. `__WFUNCTION__` agora é um `const wchar_t*` em tempo de execução, que pode ser passado e concatenado via `<<`,
  mas não colado a outro literal. O servidor só usa como argumento.
- **`GetCurrentTime`** é macro no `windows.h`: o `types.h` faz `#undef` para não quebrar `CTime::GetCurrentTime()`.
- `GetTickCount` dá a volta a cada ~49,7 dias, como no Windows (comparações com subtração sem sinal continuam certas).

## Limitações conhecidas

- `_beginthreadex` não suporta `CREATE_SUSPENDED`.
- `CString` cobre só o subconjunto usado até agora (vai crescer conforme o servidor for portado).
- Windows ainda **não foi compilado** (só Linux GCC 16 / Clang 23). O código separa os caminhos, mas falta validar no MSVC.

## Testes

`src/platform/tests` (doctest): unicode, formatos MSVC, `CString`, `CTime`/`CTimeSpan`, eventos, threads, CRT.
