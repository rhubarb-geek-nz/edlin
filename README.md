# rhubarb-geek-nz/edlin

Modern implementation of [edlin](https://en.wikipedia.org/wiki/Edlin) in [C](https://en.wikipedia.org/wiki/C_(programming_language)).

Reference is the [MS-DOS v2.0 version of edlin](https://github.com/microsoft/MS-DOS/blob/main/v2.0/source/EDLIN.ASM).

Works over SSHD including on Windows 10 IoT.

Interface is as defined in original MS-DOS manuals.

Usage - `edlin` _filename_ \[/B\]

## Commands

The commands are as per the original MS-DOS program and manuals.

| 1 | 2 | 3 | 4 | opt | cmd | details | argument
|---|---|---|---|-----|-----|-------- | --------
|   |   |   |   |     | A | append from input |
| first | last | dest | count | | C | copy lines |
| first | last |  |  |  | D | delete lines |
| | | | | | E | exit and save file |
| line | | | | | I | insert text |
| first | last | | | | L | list content |
| first | last | dest | | | M | move lines |
| first | last | | | | P | page |
| | | | | | Q | quit without saving |
| first | last | | | ? | R | replace text | search^Zreplace
| first | last | | | ? | S | search for text | search
| first | | | | | T | merge text from file | filename
| count | | | | | W | write to output file |

## Limitations

- Lines are limited to 255 bytes

## Implementation

- Console is treated as UNICODE
- File data is in the current ANSI code page
- Uses VT-100 escape sequences to handle control keys from console
- Unrelated to the FreeDOS implementation
- Compiles and runs on DOS, OS/2, Windows, Linux, macOS, OpenBSD, FreeBSD, NetBSD, QNX and Solaris.
