# Changelog

## 1.0.0 — 2026-08-13

- First private universal PortMaster release.
- Preserves the Android GameMaker startup and runtime flow through a reproducible
  ARMv7 gmloader-next adapter.
- Adds transactional NXExtract 1.2.6 owner-package discovery for loose APK and
  base APK inside APKM/APKS/XAPK containers.
- Validates the exact 24 consumed owner members and emits one deterministic
  STORE-only runtime archive.
- Provides GLES2 video, music and sound effects, native SDL controllers, saves,
  clean Select+Start exit and twelve persistent language choices.
- Requires at most GLIBC 2.28; no RPATH/RUNPATH or proprietary data is shipped.
