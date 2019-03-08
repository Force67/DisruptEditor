## Disrupt Editor

[![Build status](https://ci.appveyor.com/api/projects/status/tpcpf00rbkk8i1n0?svg=true)](https://ci.appveyor.com/project/j301scott/disrupteditor) [![License: Unlicense](https://img.shields.io/badge/license-Unlicense-blue.svg)](http://unlicense.org/)

Latest Build: https://ci.appveyor.com/project/j301scott/disrupteditor/build/artifacts

# Setup

1. Run Disrupt Editor, it will automatically create a settings.json for you to fill in
2. Assuming Z:/scratch/bin is where you extracted each of the fat/dat files, add them to searchPaths like so:
patchDir is where modified files like wlu and cbatch are written. Files in the patchDir will always take precedence
```xml
<searchPaths>
    <Elem>Z:/scratch/bin/patch_unpack/</Elem>
    <Elem>Z:/scratch/bin/dlc_solo_unpack/</Elem>
    <Elem>Z:/scratch/bin/dlc_solo_english_unpack/</Elem>
    <Elem>Z:/scratch/bin/dlc_pill_people_unpack/</Elem>
    <Elem>Z:/scratch/bin/windy_city_unpack/</Elem>
    <Elem>Z:/scratch/bin/windy_city_english_unpack/</Elem>
    <Elem>Z:/scratch/bin/shaders_unpack/</Elem>
    <Elem>Z:/scratch/bin/shadersobj_unpack/</Elem>
    <Elem>Z:/scratch/bin/sound_unpack/</Elem>
    <Elem>Z:/scratch/bin/sound_english_unpack/</Elem>
    <Elem>Z:/scratch/bin/common_unpack/</Elem>
</searchPaths>
<patchDir>C:/Program Files/Ubisoft/WATCH_DOGS/bin/patch/</patchDir>
```
3. Configure camera keyboard settings keyForward, keyBackward, etc. with https://wiki.libsdl.org/SDLScancodeLookup
keyFast, keySlow with
https://hg.libsdl.org/SDL/file/05aff4771d9a/include/SDL_keycode.h#l325

# Default Controls
* W A S D - Move Camera
* R - Ascend
* F - Descend
* Shift - Increase camera speed
* Ctrl - Decrease camera speed
* Middle mouse button - Rotate camera

# Features

* WLU Editing (Export/Import as XML)
* Display of some xbgs
* Material Editing

# Roadmap

* Finish compound cbatch files
* XBG Redering/Export
* SBAO
* CBox Editor
* Cache Files for faster loading
