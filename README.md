# Project Punch

# Built with Syriinge
This is an implementation of a [Syriinge](https://github.com/Sammi-Husky/Syriinge) plugin to be used in Super Smash Bros. Brawl. It is made with syriinge in This repository is already set up to use the [BrawlHeaders](https://github.com/Sammi-Husky/BrawlHeaders) repository which includes many if not most in game functions you will need to use in your plugins.

# Requirements
 - DevKitPro

# Building
To build, simply run `make` on the command line.


# Installing on your build.
## A note on dolphin builds
Most Dolphin builds are distributed as SD card `.raw` images. The easiest way that I've found to manipulate these is to download a mainline recent dolphin build, and use its SD managing features to dump the RAW file to a folder. At the end of the installation, you can use the same tool to convert the whole folder back, modifying the original file in place.


1) Copy the `Project+` folder to the root of your SD card.
> Note: recent P+ builds (2.5.2 at time of writing) have included versions of `Project+/module/sy_core.rel`, but they are out of date, and are unused by the stock configuration anyway. You can safely overwrite this file. Using incorrect versions will cause Project Punch to quietly not load.

2) Enter Project+ on your SD card. All future instructions are assuming you are in this directory. Open RSBE01.txt. 

3) Check for a code at the top of your build. At the top of the file, check for a code with the title that starts with "VBI -> AXNextFrame During Boot". If this code does not exist,
   add the line `.include Source/pre_syriinge.asm`.

4) After either the "VBI -> AXNextFrame During Boot" or the `pre_syriinge.asm` line, add the line `.include Source/syriinge.asm`.   

5) Drag 'n drop `RSBE01.txt` onto `GCTRealMate.exe` to regenerate the `RSBE01.gct` file.
