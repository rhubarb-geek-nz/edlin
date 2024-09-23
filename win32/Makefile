# Copyright (c) 2024 Roger Brown.
# Licensed under the MIT License.

SRC=..\src\edlin.c ..\src\readline.c ..\src\mbcs.c edwin.c
CL=cl
OBJDIR=obj\$(VSCMD_ARG_TGT_ARCH)
BINDIR=bin\$(VSCMD_ARG_TGT_ARCH)
BUNDLE=bundle
RESFILE=$(OBJDIR)\edlin.res
APP=$(BINDIR)\edlin.exe
MSI=edlin-$(DEPVERS_edlin_STR4)-$(VSCMD_ARG_TGT_ARCH).msi
MSIX=$(BUNDLE)\edlin-$(DEPVERS_edlin_STR4)-$(VSCMD_ARG_TGT_ARCH).msix
MSIXBUNDLE=edlin-$(DEPVERS_edlin_STR4).msixbundle

all: $(APP) $(MSI) $(MSIX)
	
clean: 
	if exist $(APP) del $(APP)
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
		/DEF:edlin.def				\
		$(RESFILE)
	del "$(BINDIR)\edlin.exp"
	del "$(BINDIR)\edlin.lib"
	signtool sign /sha1 "$(CertificateThumbprint)" /fd SHA256 /t http://timestamp.digicert.com $@

$(RESFILE): edlin.rc
	rc /r /I..\src $(RCFLAGS) "/DDEPVERS_edlin_INT4=$(DEPVERS_edlin_INT4)" "/DDEPVERS_edlin_STR4=\"$(DEPVERS_edlin_STR4)\"" /fo$@ edlin.rc

$(OBJDIR) $(BINDIR) $(BUNDLE):
	mkdir $@

$(MSI): $(APP) $(CHM)
	"$(WIX)bin\candle.exe" -nologo "wix\$(VSCMD_ARG_TGT_ARCH).wxs" -dDEPVERS_edlin_STR4=$(DEPVERS_edlin_STR4)
	"$(WIX)bin\light.exe" -nologo -cultures:null -out $@ "$(VSCMD_ARG_TGT_ARCH).wixobj"
	del "$(VSCMD_ARG_TGT_ARCH).wixobj"
	del "edlin-$(DEPVERS_edlin_STR4)-$(VSCMD_ARG_TGT_ARCH).wixpdb"
	signtool sign /sha1 "$(CertificateThumbprint)" /fd SHA256 /t http://timestamp.digicert.com $@

$(MSIX): $(BUNDLE) $(APP)
	COPY /Y $(APP)
	DIR edlin.exe
	makeappx pack /m AppxManifest.xml /f mapping.ini /p $@
	DEL edlin.exe
	signtool sign /sha1 "$(BundleThumbprint)" /fd SHA256 /t http://timestamp.digicert.com $@

$(MSIXBUNDLE):
	makeappx bundle /d $(BUNDLE) /p $@
	signtool sign /sha1 "$(BundleThumbprint)" /fd SHA256 /t http://timestamp.digicert.com $@
