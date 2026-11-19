// X2 behavior switches (decision T47), in one place, as the original kept them in ServerDefine.h (servers) and
// Always.h (client). One file for the servers, the client and the tools; a switch only one side reads says so in its
// comment.
//
// The build force-includes this header in every X2 target (x2::options, cmake/X2Compiler.cmake), so code tests the
// switches without including it.
//
// Rules:
// - A switch is on when defined: `#define NAME` turns it on, `//#define NAME` off. Code tests it with #ifdef/#ifndef
//   (defined(NAME) in expressions).
// - This file says how the program behaves. What is compiled comes from the command line only: X2_DEV_TOOLS (the
//   CMake option, set by the presets: the development tools such as the UI editor, and the hooks the client keeps for
//   them) and the platform/toolchain macros (_WIN32, NDEBUG, UNICODE, ...). Neither is set here.
// - Only switches the port adds belong here. The original's own flags were resolved by the importer (T9) and are no
//   longer switches; a feature that must become switchable again gets a new switch here and its code by hand.
// - Runtime knobs (environment variables such as X2_UI_LIVE, X2_RECORD_UI_ANCHORS, X2_RENDER_BACKEND) need no rebuild
//   and stay out.

#pragma once

// --- Servers and client -----------------------------------------------------------------------------------------------

// Debug behavior (T10): what the original did in its _IN_HOUSE_ (not _SERVICE_) builds and the port's own debugging
// aids. Debug overlays and consoles, cheats and admin commands, verbose logs, loose client_data files read before the
// archives, Config_INTERNAL.lua, the client starting without the patcher's argument, and the servers' extra checks
// and logs. On in builds without NDEBUG (the *-debug presets); for an optimized build with them (measuring frame
// rates with the overlays), define it unconditionally here and build *-release.
#ifndef NDEBUG
#define X2_DEBUG
#endif

// (characters, systems and events made switchable go here, one switch each)

// Village movement sync (T53): villages, squares, wedding halls and dungeon lounges sync other players' movement with
// server-validated state snapshots, interpolation, interest management and per-tick aggregation, and the servers check
// every position against the map's line map. Commented out, the original's sync (2 s of replayed input, 5 s square
// snapshots, nothing validated) is built instead.
#define X2_VILLAGE_SYNC

// --- Servers only -----------------------------------------------------------------------------------------------------

// --- Client only ------------------------------------------------------------------------------------------------------
