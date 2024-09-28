# Copyright (c) 2024 Roger Brown.
# Licensed under the MIT License.

CFLAGS=-I..\src -d0 -DHAVE_UNISTD_H -DWIN32_LEAN_AND_MEAN
SRCFILES=..\src\edlin.c ..\src\readline.c ..\src\mbcs.c eddos.c
HDRFILES=..\src\edlin.h ..\src\readline.h ..\src\mbcs.h

all: edlin.exe edlin.com 286\edlin.exe 386\edlin.exe

clean: .SYMBOLIC
	if exist *.err del *.err
	if exist edlin.exe del edlin.exe
	if exist edlin.com del edlin.com
	if exist 286\edlin.exe del 286\edlin.exe
	if exist 386\edlin.exe del 386\edlin.exe
	if exist 286 rmdir 286
	if exist 386 rmdir 386

edlin.com: $(SRCFILES) $(HDRFILES)
	wcl.exe -0 -bcl=com -fpc $(SRCFILES) -fe=$@ $(CFLAGS) -DEDLIN_MALLOC=0x8000
	del *.obj
	if exist *.err exit 1

edlin.exe: $(SRCFILES) $(HDRFILES)
	wcl.exe -0 -bcl=dos -fpc $(SRCFILES) -fe=$@ $(CFLAGS) -DEDLIN_MALLOC=0x8000
	del *.obj
	if exist *.err exit 1

286\edlin.exe: $(SRCFILES) $(HDRFILES) 286
	wcl.exe -2 -bcl=os2 -lp $(SRCFILES) -fe=$@ $(CFLAGS) -I$(%WATCOM)\h\os21x -DEDLIN_MALLOC=0x8000
	del *.obj
	if exist *.err exit 1

386\edlin.exe: $(SRCFILES) $(HDRFILES) 386
	wcl386.exe $(SRCFILES) -fe=$@ $(CFLAGS)
	del *.obj
	if exist *.err exit 1

286 386: .SYMBOLIC
	mkdir $@
