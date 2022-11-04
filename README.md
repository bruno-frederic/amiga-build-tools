# amiga-build-tools

Tools useful for building *𝔸miga* software.

Theses tools support kickstart version from 1.0+ to *retro-enjoy* with the original *𝔸miga 1000* !


# Pause

Pause the script (or Startup-Sequence) until *RETURN* ⏎ key is pressed.

Implementation using pure Amiga DOS library call, works on all kickstart version 1.0+.

Tested on Kickstart 1.0, 1.1, 1.2, 2.0 (A3000), 2.2 (A3000), 2.04, 2.05, 3.0 (A4000), 3.1 (A4000, A3000, A1200, A500), 3.1.4, 3.2, 3.2.1


# VersionExec

Simple `VERSION` implementation using pure Amigas Dos library call.

Works on all kickstart version from 1.0+.
(The original Amiga shell `VERSION` command crash on KS 1.0)

Example usage in Startup-Sequence :
```
VersionExec 30
If warn
    echo "Kickstart too old"
Else
    echo "Kickstart 1.0 OK !"

    LaunchCoolCommand
    (...)
Endif
```
This table list library versions and corresponding Kickstart releases :
https://wiki.amigaos.net/wiki/AmigaOS_Versions


# UAEexit

Close WinUAE emulator.

Can be used at the end of your *startup-sequence* in your cross-platform development environnement :

Example usage in Startup-Sequence :
```
MyProgram

UAEexit EXIT
```
Warning : parameter is case-sensitive.

Thge machineOr with other parameters : `UAEexit RESET` `UAEexit HARDRESET`



### Compatibility

Tested on Kickstart 1.0, 1.1, 1.2, 1.3, 2.04, 2.05, 3.0 (A4000), 3.1 (A4000, A1200, A500), 3.1.4, 3.2, 3.2.1.

Note : on KS 1.0, 1.1 and 1.2 : resource not found but calltrap found.

Crash WinUAE when :
- **JIT** enabled which is the default on quickstart A4000
- when both ADF and DH0 are mounted in with kickstart **1.0**

Instabilities occurs when using RESET or HARDRESET parameters on :
- A4000 + ROM 3.1 → sometimes, machine is stuck during reset.
- A500 + ROM 3.1 → sometimes, machine is stuck during reset.
- kickstart 1.0 ; 1.1 ; 1.2 ; 1.3 → exit or crash WinUAE when debugger attached to an Amiga process


# kick-1_0-functions.h (for developpers)

The standard VBCC startup code crash under kickstart 1.0. In order to build binaries for kickstart 1.0 with VBCC, the **minimal VBCC startup code** must be used, with +kick13**m** or +aos68k**m** config.

However, the standard C library is very limited in minimal VBCC startup code, as stated in *vbcc.pdf* documentation, §14.5.6 *Minimal Startup* :
>string and ctype functions are ok, but most other functions - especially I/O and memory handling - must not be used

This header file provides useful functions replacement compatible with every kickstart version from kick 1.0+ in the minimal startup code VBCC environment :
- `myprintf()` equivalent to `printf()` function from standard C library.
- `myVPrintf()` equivalent to `VPrintf()` function from Amiga DOS 2.0+ library.
- `WRITE_CONST_STR()` macro to print a `const char[]` literal
  which is more efficient than printf() function

# Building environment
I have used :
- Visual Studio Code 1.72 on Windows 10
- VS Code [example workspace for VBCC](https://github.com/bruno-frederic/vscode-amiga-vbcc-example/) and [Amiga Assembly extension](https://marketplace.visualstudio.com/items?itemName=prb28.amiga-assembly) v1.6.0 by prb28 to intregrate tightly VBCC into VS Code
- AmigaOS 1.3 NDK header files

But it should compile easily on another environment with :
- [VBCC v0.9](http://sun.hasenbraten.de/vbcc/)
- AmigaOS 1.3 NDK
- Make

Compile with target +kick13**m** (**minimal** startup code) :
```
  vc +kick13m (...)
```
