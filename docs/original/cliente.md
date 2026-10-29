[Início](../README.md) › [Código original](README.md) › Cliente — acoplamento com Windows / Direct3D 9

# Cliente — acoplamento com Windows / Direct3D 9

Três camadas: `X2` (exe, 2 fontes) → `X2Lib` (jogo, 285 fontes no vcxproj, 609 arquivos) → `KTDXLIB` (engine, 120 fontes, 338 arquivos).
Build original: Win32 x86, Unicode, MFC em algumas configs (`_AFXDLL`), `_USE_32BIT_TIME_T`.

## Boa notícia: o D3D9 está concentrado no KTDXLIB

| Padrão | X2Lib | KTDXLIB | Servidor |
|---|---:|---:|---:|
| `IDirect3DDevice9` / `LPDIRECT3DDEVICE9` | 2 | 56 | 0 |
| `GetDevice()` | 22 | 29 | 0 |
| `LPDIRECT3DTEXTURE9` | 0 | 11 | 0 |
| `D3DRS_`/`D3DFVF`/`D3DPT_`/`D3DFMT` | 14 | 57 | 1 |
| Tipos de matemática D3DX (`D3DXVECTOR*`, `D3DXMATRIX`...) | 367 | 104 | 13 |
| Funções de matemática D3DX (`D3DXVec*`, `D3DXMatrix*`...) | 50 | 33 | 4 |
| `HWND` | 272 | 58 | 13 |
| `MessageBox` | 40 | 19 | 4 |

Leitura: X2Lib usa D3D quase só como **biblioteca de matemática** (`D3DXVECTOR3` aparece ~8.000 vezes) e via `HWND`
na assinatura de `MsgProc`. Um shim de matemática compatível + tradução de eventos isolam a maior parte do X2Lib do porte gráfico.
Os ~22 arquivos do X2Lib que chamam `GetDevice()` precisam ser revisados um a um.

## O que vem do D3DX e precisa de substituto

| Recurso | Uso | Substituto |
|---|---|---|
| `D3DXLoadMeshFromX` / `...HierarchyFromX`, `ID3DXAnimationController` | `KTDXDeviceXMesh`, `KTDXDeviceXSkinMesh`, `KTDXAnimationController` | parser `.x` próprio (texto/binário) + skinning; formato `.x` fica (asset) |
| `D3DXCreateEffect` | 4 arquivos; `x2.fx` em `_client/data` | traduzir `x2.fx` (HLSL SM2/3 + techniques/passes) para shaders do backend novo; isso é **código**, não asset |
| `D3DXCreateTextureFromFile`, cube textures | `KTDXDeviceTexture` | loader DDS (DXT1/3/5) + stb_image |
| `D3DXCreateFont`, `D3DXCreateSprite` | fontes/UI | FreeType (já usado em `KTDGFreeTypeFont`) + batcher 2D |
| Fixed-function (`SetRenderState`, `DrawPrimitiveUP`, FVF) | 16 / 21 arquivos | emular pipeline fixo com shaders + buffers dinâmicos |

## Outros subsistemas

| Área | Onde | Plano |
|---|---|---|
| Janela / loop | `KTDXApp`, `DXUT/` | SDL3 |
| Input | `KTDIDevice*` (DirectInput8 em 6 arquivos), `KTDIDeviceWinKey` | SDL3 |
| IME (coreano/CJK) | `KTDGUIIMEEditBox`, `KHangulPPWSelector` (`ImmGetContext` em 5 arquivos, `WM_IME_*`) | SDL3 text input/IME — revisar se a variante for US |
| Som | `KTDSManager`, `KTDXDeviceSound`, `KTDSMP3Play`, `FMOD/` (FMOD Ex), DirectSound | miniaudio / SDL3 audio, ou FMOD Core atual |
| Vídeo | `BandiVideo*` (15 arquivos), `bdvid32.dll` | remover ou trocar (ffmpeg/pl_mpeg) conforme formato em `data/movie` |
| Rede | `KTDNManager`, `KTDNUDP`, `KTDNTransfer`, `X2ServerProtocol/Socket` (IOCP no cliente!) | mesma lib de rede do servidor (Asio) |
| Arquivos `.kom` | `KGCMassFileManager`, `KGCMassFileBufferManager` (zlib `uncompress`) | portar; formato é asset |
| Browser embutido | `KBrowserWrapper`, `KPCHWebBrowser`, `Browser.c` | remover |
| Crash/anti-hack | `CrashRpt`, `KncFirewall`, `KProtectedType`, `AntiHackingString.h`, XTRAP, HackShield | remover |
| Publisher | `NexonModule`, `Steamworks`, `IGA` (ads), `X2Lib/OnlyGlobal/<país>` | remover/stub |

## Riscos específicos do cliente

- **x64**: ~600 casts ponteiro→`DWORD/int/long` (estimativa por regex, cliente+servidor) e 12 arquivos com `__asm`.
- `_USE_32BIT_TIME_T`: `time_t` de 32 bits pode estar em structs gravados/serializados.
- Ordem de inicialização de singletons globais (`KJohnSingleton`, `KGlobalSingleton`, `KDynamicSingleton`).
- `x2.fx` mistura estado fixo e shaders — tradução manual, validar visualmente contra o original.
