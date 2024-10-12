# Copyright (c) 2024 Roger Brown.
# Licensed under the MIT License.

CFLAGS=-I..\src -d0 -DHAVE_UNISTD_H -DWIN32_LEAN_AND_MEAN -DEDLIN_DOSFILESYSTEM
SRCFILES=..\src\edlin.c ..\src\readline.c ..\src\mbcs.c eddos.c
HDRFILES=..\src\edlin.h ..\src\readline.h ..\src\mbcs.h
READFILES=..\src\mbcs.c ..\src\readutf8.c

all: edlin.exe edlin.com 286\edlin.exe 386\edlin.exe 286\readutf8.exe 386\readutf8.exe readutf8.exe

clean: .SYMBOLIC
	if exist *.err del *.err
	if exist edlin*.msg del edlin*.msg
	if exist edlin*.txt del edlin*.txt
	if exist edlin.exe del edlin.exe
	if exist readutf8.exe del readutf8.exe
	if exist edlin.com del edlin.com
	if exist 286\*.exe del 286\*.exe
	if exist 386\*.exe del 386\*.exe
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

readutf8.exe: $(READFILES) $(HDRFILES)
	wcl.exe -0 -bcl=dos -fpc $(READFILES) -fe=$@ $(CFLAGS) -DMBCS_LOOKUP
	del *.obj
	if exist *.err exit 1

286\edlin.exe: $(SRCFILES) $(HDRFILES) 286
	wcl.exe -2 -bcl=os2 -lp $(SRCFILES) -fe=$@ $(CFLAGS) -I$(%WATCOM)\h\os21x -DEDLIN_MALLOC=0x8000
	del *.obj
	if exist *.err exit 1

286\readutf8.exe: $(READFILES) $(HDRFILES) 286
	wcl.exe -2 -bcl=os2 -lp $(READFILES) -fe=$@ $(CFLAGS) -I$(%WATCOM)\h\os21x -DMBCS_LOOKUP
	del *.obj
	if exist *.err exit 1

386\edlin.exe: $(SRCFILES) $(HDRFILES) 386
	wcl386.exe $(SRCFILES) -fe=$@ $(CFLAGS)
	del *.obj
	if exist *.err exit 1

386\readutf8.exe: $(READFILES) $(HDRFILES) 386
	wcl386.exe $(READFILES) -fe=$@ $(CFLAGS) -DMBCS_LOOKUP
	del *.obj
	if exist *.err exit 1

286 386: .SYMBOLIC
	if not exist $@ mkdir $@

edlin-de.txt: nls\edlin-de.txt 386\readutf8.exe
	386\readutf8.exe nls\edlin-de.txt >$@

edlin-de.msg: edlin-de.txt
	mkmsgf edlin-de.txt $@

edlin-en.txt: nls\edlin-en.txt 386\readutf8.exe
	386\readutf8.exe nls\edlin-en.txt >$@

edlin-en.msg: edlin-en.txt
	mkmsgf edlin-en.txt $@

edlin-es.txt: nls\edlin-es.txt 386\readutf8.exe
	386\readutf8.exe nls\edlin-es.txt >$@

edlin-es.msg: edlin-es.txt
	mkmsgf edlin-es.txt $@

edlin-fr.txt: nls\edlin-fr.txt 386\readutf8.exe
	386\readutf8.exe nls\edlin-fr.txt >$@

edlin-fr.msg: edlin-fr.txt
	mkmsgf edlin-fr.txt $@

edlin-it.txt: nls\edlin-it.txt 386\readutf8.exe
	386\readutf8.exe nls\edlin-it.txt >$@

edlin-it.msg: edlin-it.txt
	mkmsgf edlin-it.txt $@

edlin.msg: edlin-de.msg edlin-en.msg edlin-es.msg edlin-fr.msg edlin-it.msg
