# Copyright (c) 2024 Roger Brown.
# Licensed under the MIT License.

CFLAGS=-I..\src -d0
SRCFILES=..\src\edlin.c ..\src\readline.c ..\src\mbcs.c eddos.c
HDRFILES=..\src\edlin.h ..\src\readline.h ..\src\mbcs.h

all: edlin.exe 286\edlin.exe 386\edlin.exe

clean: .SYMBOLIC
	if exist edlin.exe del edlin.exe
	if exist 286\edlin.exe del 286\edlin.exe
	if exist 386\edlin.exe del 386\edlin.exe
	if exist 286 rmdir 286
	if exist 386 rmdir 386

edlin.exe: $(SRCFILES) $(HDRFILES)
	wcl.exe -0 -bcl=dos -fpc $(SRCFILES) -fe=$@ $(CFLAGS) -DEDLIN_MALLOC=0x8000
	del *.obj

286\edlin.exe: $(SRCFILES) $(HDRFILES) 286
	wcl.exe -2 -bcl=os2 -lp $(SRCFILES) -fe=$@ $(CFLAGS) -I$(%WATCOM)\h\os21x -DEDLIN_MALLOC=0x8000
	del *.obj

386\edlin.exe: $(SRCFILES) $(HDRFILES) 386
	wcl386.exe $(SRCFILES) -fe=$@ $(CFLAGS)
	del *.obj

286 386: .SYMBOLIC
	mkdir $@
