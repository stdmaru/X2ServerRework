#pragma once

// The audio backend's system object, which the original took from FMOD Ex (FMOD_SYSTEM): the client's sound system on
// miniaudio and SDL3 (decision T45, src/client/Audio). CKTDSManager owns it; CKTDSMP3Play and CKTDXDeviceSound play
// through it.
#include "SoundSystem.h"

using KTDSSoundSystem = x2::audio::SoundSystem;
