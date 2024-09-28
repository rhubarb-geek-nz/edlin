# rhubarb-geek-nz/edlin

Modern implementation of [edlin](https://en.wikipedia.org/wiki/Edlin) in [C](https://en.wikipedia.org/wiki/C_(programming_language)).

Reference is the [MS-DOS v2.0 version of edlin](https://github.com/microsoft/MS-DOS/blob/main/v2.0/source/EDLIN.ASM).

Works over SSHD including on Windows 10 IoT.

Interface is as defined in original MS-DOS manuals.

Usage - `edlin` _filename_ \[/B\]

## Mission Statement

Provide a working equivalent `edlin` for modern operating systems. The goal is not to enhance or extend it's behaviour, however it should be a good citizen on the hosted operating system. It should work correctly with just a terminal and on a minimal system.

## Commands

The commands are as per the original MS-DOS program and manuals.

| 1 | 2 | 3 | 4 | opt | cmd | argument | comment
|---|---|---|---|-----|-----|-------- | --------
|   |   |   |   |     | A | | append from input
| first | last | dest | count | | C | | copy lines
| first | last |  |  |  | D | | delete lines
| | | | | | E | | exit and save file
| line | | | | | I | | insert text
| first | last | | | | L | | list content
| first | last | dest | | | M | | move lines
| first | last | | | | P  | | page
| | | | | | Q | | quit without saving
| first | last | | | ? | R | search^Zreplace | replace text
| first | last | | | ? | S | search | search for text
| first | | | | | T | filename | merge text from file
| count | | | | | W | | write to output file

## Limitations

- Lines are limited to 255 bytes

## Implementation

- Windows console is treated as UNICODE
- Terminals are treated as UTF-8
- Uses VT-100 escape sequences to handle control keys from console
- Compiles and runs on DOS, OS/2, Windows, Linux, macOS, OpenBSD, FreeBSD, NetBSD, QNX, Haiku and Solaris.
- Uses UTF-8 data on Win32 and POSIX systems. On DOS it defaults to codepage 437 and on OS/2 it uses the default configured codepage, typically 850.
- NLS support is minimal, the original used hardcoded strings. This version supports string tables on Windows and message catalogs on POSIX systems. The OS/2 version uses message files.

- Content is held internally in a single block of memory allocated once at startup.
- Unrelated to the FreeDOS implementation

## Building

No tools such as `autoconf`, `automake` or `libtool` are required. If all goes well the output should be both an executable and an installable package for that operating system.

### Build the easy way

If you have PowerShell installed, use [package.ps1](package.ps1) to do the complete build and package.

### Build the common way

There is a [configure](configure) script, [Makefile](Makefile) will run this to create config.h if not already done.

#### Solaris example

It is recommended to pass strict compilation flags to ensure correctness of the build.

```
$ CC=gcc make CFLAGS="-Wall -Werror -m32"
```

### Build for DOS

Use the [OpenWatcom compiler](https://github.com/open-watcom/open-watcom-1.9/releases/tag/ow1.9) on Windows and run `wmake` in the [dos](dos) subdirectory. This should build both `edlin.exe` and `edlin.com`.

### Build for OS/2

Use the [OpenWatcom compiler](https://github.com/open-watcom/open-watcom-1.9/releases/tag/ow1.9) on OS/2 and run `wmake` in the [dos](dos) subdirectory. This should build both `286\edlin.exe` and `386\edlin.exe`.

### Build for Windows

With Visual Studio installed, run the [package.ps1](package.ps1) script. The build system is set up to sign executables and packages.

```
C:> PowerShell -ExecutionPolicy ByPass .\package.ps1 -CertificateThumbprint 601A8B683F791E51F647D34AD102C38DA4DDB65F -BundleThumbprint 5F88DFB53180070771D4507244B2C9C622D741F8
```
