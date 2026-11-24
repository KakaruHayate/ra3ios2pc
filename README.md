# Red Alert 3 (iOS 2009) on PC

Compatibility-layer port of *Command & Conquer: Red Alert 3* (iPhone/iPod touch, v1.7.0, EA Mobile, 2009) to Windows.

The original ARM/iOS binary runs on [touchHLE](https://github.com/touchHLE/touchHLE). This repository contains the additions needed for this game:

- `patches/0001-ra3ios-support.patch` — touchHLE changes:
  - EA FUFS (`data.vfs`) file resolver. Entries are keyed by a 32-bit name hash: `h = h * 67 + toupper(c) - 0x71` (case-insensitive, `/` and `\` equivalent). The patch looks entries up and serves them to the app.
  - `NSFileManager` filesystem attributes (`NSFileSystemSize`, node counts).
  - Mouse wheel mapped to a two-finger pinch gesture (camera zoom).
- `launcher/` — Win32 launcher: starts `touchHLE.exe` with `--landscape-left --scale-hack=3`.

No game assets are included. Extract `Payload/Red Alert.app` from your own IPA into `touchHLE_apps/`.

## Build

Apply the patch to touchHLE (tested at commit `ca1de89`) and build it with Cargo. Compile `launcher/main.cpp` with MSVC and put the resulting exe next to `touchHLE.exe`.

## Status

Menus, briefing, level loading, gameplay, mouse control, wheel zoom and saves work. Multiplayer and the EA store are offline.

Game assets © Electronic Arts. touchHLE is MPL-2.0.
