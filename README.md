# T4MP v2 (Turok 4 Multiplayer)

```
████████╗██╗  ██╗███╗   ███╗██████╗     ██╗   ██╗██████╗ 
╚══██╔══╝██║  ██║████╗ ████║██╔══██╗    ██║   ██║╚════██╗
   ██║   ███████║██╔████╔██║██████╔╝    ██║   ██║ █████╔╝
   ██║   ╚════██║██║╚██╔╝██║██╔═══╝     ╚██╗ ██╔╝██╔═══╝ 
   ██║        ██║██║ ╚═╝ ██║██║          ╚████╔╝ ███████╗
   ╚═╝        ╚═╝╚═╝     ╚═╝╚═╝           ╚═══╝  ╚══════╝
                                                         
   Online Multiplayer for Turok: Evolution
```

## Overview
T4MP v2 is a modification for Turok: Evolution (also known as Turok 4) that aims to add online multiplayer functionality to the PC version of the game. The mod utilizes the existing split-screen multiplayer experience but removes the split-screen limitation while synchronizing player data over the internet.

This is the second version of the T4MP project, featuring improved code organization and network architecture.

## 🚨 Current Status
**Pre-Alpha Stage**: This project is currently in early development. Many features are not yet implemented or may not function as intended. Please check the [GitHub Issues](https://github.com/pnill/T4MP_V2/issues) page for known limitations and bugs.

## ✨ Features
- Networked multiplayer for Turok: Evolution
- Game server browser (work in progress)
- Log system/console (accessible via the tilde key ~)
- Actor spawner for testing (accessible via HOME key)

## 🔮 Planned Features
- Custom scoreboard
- Player character selection
- Quality of life improvements (built-in windowing, mouse acceleration disabling)
- Improved graphics (textures, shaders, etc.)
- Custom maps
- Modding/extensibility options (custom death sounds, scripting)
- Custom game modes
- Co-op gameplay
- "Invasion" mode with waves of AI enemies

## 🖥️ System Requirements
- Same requirements as the original Turok: Evolution PC release
- Stable and fast internet connection (high latency will cause unexpected behavior)

## 📦 Installation
### For Users
Currently, there is no installer available as the project is in pre-alpha. An installer will be provided when the project is closer to an official release.

### For Developers
1. Compile the T4MP.dll using the source code
2. Place the compiled DLL next to the provided T4MP.exe
3. Launch T4MP.exe to inject the DLL into the game

### Notes for Developers
- A lot of the includes are sloppy in the way that they're handled.
- A lot of the externs/forward declares are also sloppy.
- The reliance on OldEngineStruct.h vs proper implementation of game classes is something I hope to address over time and deprecate but the first goals of V2 were to move away from the old engine hooks, and all of the code existing in a single file "Engine.cpp" and attempt to come up with a 'cleaner' code base.

### Notes about code
- APIHooks.cpp is abused to prevent input where necessary.
- dinput\IDInDev8.cpp and dinput\IDIn8.cpp are to hijack direct input as the game utilizes both windows APIs for raw input and dinput, this way input can be prevented from reaching the game when ImGui or other windows related to the modification are open such as the server browser.

- D3D9Hooks\d3d9dev.cpp is a wrapper for the entire device but isn't directly utilized for ImGui rendering, instead imgui_renderloop() is called within ImGui\T4MP UIs\imgui_renderloop.cpp which then renders each ImGui window/console/popup when necessary based on global variables belonging to each to toggle them.

- T4MP\T4MP_Engine.cpp - T4MP::GetDMPlayer() doesn't actually work to grab players by index, it used to in T4MP V1 but because I managed to actually remove players from the camera array the method it uses no longer works.

- T4MP\T4MP_Engine.cpp - T4MP::GetLocalPlayer() does not work as expected, it will return the latest spawned player (possibly the latest added actor to the level not entirely sure) in a lot of places I sort of utilize this as a "IsInGame" function of sorts, the game actually uses this in a similar way for some debugging purposes. Basically looking at the pointers contained within the rest of the chain I'd normally use within T4MP::GetDMPlayer() isn't enough and there's still potential for things to go wrong/you to access an invalid pointer.

Meaning using T4MP::GetLocalPlayer() before T4MP::GetDMPlayer() just to make sure you get a valid response is recommended as T4MP::GetDMPlayer() is not guaranteeed to always return valid data especially when they're in menus vs in-game.

- Any thiscall hooks from the game also have the "edx" as the second parameter, for the most part when calling the original implementations using "o" prefixing the function such as oFireWeapon() you can pass "0" for edx unless explicitly stated otherwise/commented on the typedef for the routine.

## 🛠️ Building from Source
- Visual Studio 2015 is required
- Build for x86 (x64 is not supported as there's no 64-bit version of the game)
- All project settings should be default

## 📋 Dependencies
The following libraries are included in the 3rd party folder:
- D3D9
- enet-1.3.18
- ImGui-1.79
- MinHook_133

These are included directly rather than as submodules to maintain compatibility.

## 🎮 Controls
- `~` (Tilde) - Open log console (Note: Logs must be cleared periodically to prevent FPS drops)
- `HOME` - Open actor spawner (for testing only, not synced between players)

## 🎯 Getting the Game
Turok: Evolution is no longer commercially available. The mod works with:
- MYTH rip version
- Retail version
- Copies from preservation sites like MyAbandonware

## 🐛 Reporting Issues & Feedback
- Please report issues with detailed information on the [GitHub Issues](https://github.com/pnill/T4MP_V2/issues) page
- Feature requests can also be submitted through Issues
- For discussion and testing, join us on:
  - [Turok Forums](https://turokforums.com/)
  - [Turok Forums Discord](https://discord.gg/Vd68wBrqE3) in the #turok-evolution-mods channel

## 💎 Improvements Over T4MP v1
- More organized code base
- Less reliance on inline assembly or hacky hooks
- Migrated from Microsoft Detours to MinHook
- Removed protobuf in favor of custom serialization
- Switched from custom winsock code to ENet
- Simplified player/actor tracking over the network
- Tracks data differently from the original, and tracks a bit more.
- Fixes a few bugs from T4MP v1 while also bringing some of it's own.
- Server browser integrated into the game.
- Map selection, and game settings integrated into the game.
- Host and clients sync based on when the host starts the game (If the host has already started the game and a client connects it will appropriately, start their game into the correct map and etc.)

## 👏 Credits / Shouts
Special thanks to the following people for their help with testing, architecture, development assistance or advice, and most importantly listening to rants about the problems over discord while developing the project:
- Stinkee
- Dinomite
- CesartriplePizza
- djduty
- Drashid
- Edward850
- Nukeulater
- Berthalamew
- rel
- Noseos
- Reptilethatscool

## 📜 License
This project is released under a Modified MIT License.

```
Modified MIT License

Copyright (c) 2025 pnill (Chris)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

1. The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

2. Public code reviews, analyses, or featuring of the source code in any media format
(including but not limited to YouTube videos, Twitch streams, blog posts, or other
content creation platforms) requires explicit written permission from the copyright
holder(s).

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

Note: 
1. This license applies only to the T4MP modification code. Turok: Evolution and its assets remain the property of their respective copyright holders.
2. While you may freely use and modify this code for personal and community use, public code reviews or source code features in content creation require permission from the author(s).

---

*This project is not affiliated with or endorsed by the original creators of Turok: Evolution.*