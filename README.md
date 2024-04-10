# Project Punch

# Built with Syriinge
This is an implementation of a [Syriinge](https://github.com/Sammi-Husky/Syriinge) plugin to be used in Super Smash Bros. Brawl. It is made with syriinge in This repository is already set up to use the [BrawlHeaders](https://github.com/Sammi-Husky/BrawlHeaders) repository which includes many if not most in game functions you will need to use in your plugins.

# Requirements
 - DevKitPro

# Building
To build, simply run `make` on the command line.


# Installing on your build.
Copy `ProjectPunch.rel` to `Project+/pf/plugins` (make directory if it doesn't exist.). Move `sy_core.rel` to `Project+/pf/modules`. Overwrite
the existing file in that folder if it exists; Mismatched versions of this file can cause plugins to not load. 
Move `syriinge.asm` to `Project+/Source`. Add the following line to of `RSBE01.txt`. Place it after `VBI -> AXNextFrame During Boot v1.0 [Sammi-Husky]` but before any other codes.

```
.include Source/syriinge.asm
```

Drag `RSBE01.txt` onto `GCTRealMate.exe` to recompile the GCT file. 