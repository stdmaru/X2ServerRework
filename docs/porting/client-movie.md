[Home](../README.md) › [Porting](README.md) › Client: movies

# Client: movies (`src/client/Movie`, target `x2::client_movie`)

The character creation movies, played by `KTDXLIB`'s `CKTDGUIControl::CMovieData` (the Lua dialogs call `SetMovieFileName`,
`SetRect`, `SetShow`, `SetLoop`). The original played them with BandiVideo (`CBandiVideoLibrary`, `bdvid32.dll`, Windows
only, excluded by the importer). New code, no file of the original: chosen by the user on 2026-11-06 (a real decoder now,
not a stub). Overview of the client port: [Client](client.md).

## The files (surveyed 2026-11-06)

`_client/data/movie/*.avi`: **30 files, 47 MB**, loose next to the game (not in a `.kom`). All the same format: AVI with one
video stream, **MPEG-1 video** (`MPG1`), 400×220, 24 fps, I- and P-frames only (no B-frames), **no audio**, 459 to 734
frames (19 to 31 s). One per class and job (`movie_create_<class>_<job>.avi`).

## How it works

- `x2::movie::ReadAviVideoStream`: walks the RIFF chunks (`hdrl` → `strl` → `strh`/`strf`, `movi` with nested `rec `
  lists, `AVIX` extensions), takes the first video stream's FourCC, size, rate (`dwRate/dwScale`) and length, and
  concatenates its `##dc`/`##db` chunks: for MPEG-1 that is the elementary stream. A truncated file keeps its complete
  chunks.
- `x2::movie::MoviePlayer`: decodes that stream with [pl_mpeg](https://github.com/phoboslab/pl_mpeg) (MIT, single header,
  pinned commit `c871f2b`; `x2::pl_mpeg`, compiled without the project's warning options like other third-party code).
  The playback model is BandiVideo's, so `CMovieData` keeps its shape: `Open`, `Play`/`Stop`, `SeekToStart`, a status
  (`Stopped`, `Playing`, `Ended`), `IsNextFrame` and the frame copied as B, G, R, A bytes (`D3DFMT_A8R8G8B8` in memory)
  into a buffer with any pitch. The clock is advanced by the caller (`Advance(seconds)`): frame *n* is due at *n*/rate,
  every frame that became due is decoded (a stall decodes the frames it skipped), and the movie ends once its last frame
  has been shown for one frame time.
- `CMovieData` ([KTDXLIB](client-ktdxlib.md#movies-and-web-pages-2026-11-06-clientvideo-browser-cximage)) opens `movie/<name>` under the working
  directory, advances the player by the frame time (BandiVideo kept its own clock), loops or stops at the end as before,
  and copies each new frame into a managed `A8R8G8B8` texture drawn into its render target. That drawing goes through the
  engine's D3D9 path, which Phase 4 translates with the rest of the engine (T36).

## pl_mpeg limitations found (2026-11-06)

- **Memory buffers lose the end**: `plm_video_decode` discards read bytes from any buffer. For a buffer made with
  `plm_buffer_create_with_memory` that moves the caller's data and shrinks the buffer below its total size, so the end is
  never seen: the last two frames of every client movie were lost, and a rewind would read moved data. The player feeds
  pl_mpeg through callbacks instead (`plm_buffer_create_with_callbacks`, 64 KB at a time, `plm_buffer_signal_end` when the
  data runs out). With that, all 30 movies decode every frame their header counts.
- **A stream ending with a B-frame** loses its last reference frame: pl_mpeg only returns the pending reference at the end
  when the last decoded picture is I or P. The client's movies have no B-frames, so this is not worked around.
- `PLM_NO_STDIO` (used: the files are read into memory) drops `<stdio.h>`, where the header got `size_t` from: includers
  include `<stddef.h>` first.

## Tests (`x2_client_tests -tc="movie*"`)

On `src/client/tests/data/movie_red_blue.avi`, our own fixture (its `ffmpeg` command is in `test_movie.cpp`): 64×48,
24 fps, 6 red then 6 blue I/P frames. AVI headers and the stream, non-AVI data, a truncated file; open errors; the clock
(half a frame, pause, the first frame at once, the switch to blue at frame 6, the end after the last frame's time), seek and
loop, a long stall, buffer and pitch checks, the BGRA channel order and alpha. **Opt-in**: `X2_MOVIE_DIR=_client/data/movie`
plays all 30 movies to the end and checks the frame counts against their headers (2026-11-06: all pass).

## Status

Linux (Clang, GCC) and Windows (clang-cl) build; tests pass (2026-11-06). `CMovieData` itself compiles with `KTDXLIB`.
