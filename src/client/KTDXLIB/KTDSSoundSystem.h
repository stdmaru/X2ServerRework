#pragma once

// The audio backend's system object, which the original took from FMOD Ex (FMOD_SYSTEM). The port has a silent backend
// for now (roadmap, Phase 4): CKTDSManager, CKTDSMP3Play and CKTDXDeviceSound keep their interface and play nothing,
// and there is no system object (nullptr). The real backend replaces this declaration and those three files only.
struct KTDSSoundSystem;
