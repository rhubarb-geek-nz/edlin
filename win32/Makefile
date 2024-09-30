# Copyright (c) 2024 Roger Brown.
# Licensed under the MIT License.

APPNAME=edlin
DLLNAME=edlmes
LINKVERSION=0.9
SRC=..\src\$(APPNAME).c ..\src\readline.c ..\src\mbcs.c edwin.c
CL=cl
OBJDIR=obj\$(VSCMD_ARG_TGT_ARCH)
BINDIR=bin\$(VSCMD_ARG_TGT_ARCH)
BUNDLE=bundle
RESFILE=$(OBJDIR)\$(APPNAME).res
DLLRES=$(OBJDIR)\$(DLLNAME).res
APP=$(BINDIR)\$(APPNAME).exe
DLL=$(BINDIR)\$(DLLNAME).dll
MSI=$(APPNAME)-$(DEPVERS_edlin_STR4)-$(VSCMD_ARG_TGT_ARCH).msi
MSIX=$(BUNDLE)\$(APPNAME)-$(DEPVERS_edlin_STR4)-$(VSCMD_ARG_TGT_ARCH).msix
MSIXBUNDLE=$(APPNAME)-$(DEPVERS_edlin_STR4).msixbundle
CHM=htmlhelp\$(APPNAME).chm

all: $(APP) $(DLL) $(MSI) $(MSIX)
	
clean: 
	if exist $(APP) del $(APP)
	if exist $(DLL) del $(DLL)
	if exist $(CHM) del $(CHM)
	if exist $(OBJDIR)\*.obj del $(OBJDIR)\*.obj
	if exist $(OBJDIR) rmdir $(OBJDIR)
	if exist $(BINDIR) rmdir $(BINDIR)

$(APP): $(SRC) $(OBJDIR) $(BINDIR) $(RESFILE)
	$(CL) 							\
		/Fe$@ 						\
		/Fo$(OBJDIR)\				\
		/W3 						\
		/WX 						\
		/MT 						\
		/I.							\
		/I..\src					\
		/DNDEBUG 					\
		/DWIN32_LEAN_AND_MEAN		\
		$(SRC) 						\
		/link						\
		/INCREMENTAL:NO				\
		/PDB:NONE					\
		/SUBSYSTEM:CONSOLE			\
		user32.lib					\
		/VERSION:$(LINKVERSION)		\
		$(RESFILE)					\
		/MANIFEST
	mt -manifest $(APPNAME).manifest $@.manifest -outputresource:$@;#1
	del $@.manifest
	del "$(BINDIR)\$(APPNAME).exp"
	del "$(BINDIR)\$(APPNAME).lib"
	signtool sign /sha1 "$(CertificateThumbprint)" /fd SHA256 /t http://timestamp.digicert.com $@

$(DLL): $(BINDIR) $(DLLRES)
	link									\
		/DLL								\
		/OUT:$@								\
		/INCREMENTAL:NO						\
		/PDB:NONE							\
		/SUBSYSTEM:CONSOLE					\
		/MACHINE:$(VSCMD_ARG_TGT_ARCH) 		\
		/NOENTRY							\
		/VERSION:$(LINKVERSION)				\
		$(DLLRES)
	del "$(BINDIR)\$(DLLNAME).exp"
	del "$(BINDIR)\$(DLLNAME).lib"
	signtool sign /sha1 "$(CertificateThumbprint)" /fd SHA256 /t http://timestamp.digicert.com $@

$(RESFILE): $(APPNAME).rc
	rc /r /I..\src $(RCFLAGS) "/DDEPVERS_edlin_INT4=$(DEPVERS_edlin_INT4)" "/DDEPVERS_edlin_STR4=\"$(DEPVERS_edlin_STR4)\"" /fo$@ $(APPNAME).rc

$(DLLRES): $(DLLNAME).rc
	rc /r /I..\src $(RCFLAGS) "/DDEPVERS_edlin_INT4=$(DEPVERS_edlin_INT4)" "/DDEPVERS_edlin_STR4=\"$(DEPVERS_edlin_STR4)\"" /fo$@ $(DLLNAME).rc

$(OBJDIR) $(BINDIR) $(BUNDLE):
	mkdir $@

$(MSI): $(APP) $(CHM) $(DLL)
	"$(WIX)bin\candle.exe" -nologo "edlin.wxs" -dDEPVERS_edlin_STR4=$(DEPVERS_edlin_STR4)
	"$(WIX)bin\light.exe" -nologo -cultures:null -out $@ "edlin.wixobj"
	del "edlin.wixobj"
	del "$(APPNAME)-$(DEPVERS_edlin_STR4)-$(VSCMD_ARG_TGT_ARCH).wixpdb"
	signtool sign /sha1 "$(CertificateThumbprint)" /fd SHA256 /t http://timestamp.digicert.com $@

$(MSIX): $(BUNDLE) $(APP) $(DLL)
	COPY /Y $(APP)
	COPY /Y $(DLL)
	DIR $(APPNAME).exe
	DIR $(DLLNAME).dll
	makeappx pack /m AppxManifest.xml /f mapping.ini /p $@
	DEL $(APPNAME).exe
	DEL $(DLLNAME).dll
	signtool sign /sha1 "$(BundleThumbprint)" /fd SHA256 /t http://timestamp.digicert.com $@

$(MSIXBUNDLE):
	makeappx bundle /d $(BUNDLE) /p $@
	signtool sign /sha1 "$(BundleThumbprint)" /fd SHA256 /t http://timestamp.digicert.com $@

$(CHM): htmlhelp\$(APPNAME).hhp
	cd htmlhelp
	-"$(COMSPEC)" /C "%%ProgramFiles(x86)%%\\HTML Help Workshop\hhc.exe" $(APPNAME).hhp
	cd ..
	dir $@
