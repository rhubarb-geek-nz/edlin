# Copyright (c) 2024 Roger Brown.
# Licensed under the MIT License.

APPNAME=edlin
SRC=..\src\$(APPNAME).c ..\src\readline.c ..\src\mbcs.c edwin.c
CL=cl
OBJDIR=obj\$(VSCMD_ARG_TGT_ARCH)
BINDIR=bin\$(VSCMD_ARG_TGT_ARCH)
BUNDLE=bundle
RESFILE=$(OBJDIR)\$(APPNAME).res
APP=$(BINDIR)\$(APPNAME).exe
MSI=$(APPNAME)-$(DEPVERS_edlin_STR4)-$(VSCMD_ARG_TGT_ARCH).msi
MSIX=$(BUNDLE)\$(APPNAME)-$(DEPVERS_edlin_STR4)-$(VSCMD_ARG_TGT_ARCH).msix
MSIXBUNDLE=$(APPNAME)-$(DEPVERS_edlin_STR4).msixbundle

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
		/DEF:$(APPNAME).def			\
		$(RESFILE)					\
		/MANIFEST
	mt -manifest $(APPNAME).manifest $@.manifest -outputresource:$@;#1
	del $@.manifest
	del "$(BINDIR)\$(APPNAME).exp"
	del "$(BINDIR)\$(APPNAME).lib"
	signtool sign /sha1 "$(CertificateThumbprint)" /fd SHA256 /t http://timestamp.digicert.com $@

$(RESFILE): $(APPNAME).rc
	rc /r /I..\src $(RCFLAGS) "/DDEPVERS_edlin_INT4=$(DEPVERS_edlin_INT4)" "/DDEPVERS_edlin_STR4=\"$(DEPVERS_edlin_STR4)\"" /fo$@ $(APPNAME).rc

$(OBJDIR) $(BINDIR) $(BUNDLE):
	mkdir $@

$(MSI): $(APP) $(CHM)
	"$(WIX)bin\candle.exe" -nologo "wix\$(VSCMD_ARG_TGT_ARCH).wxs" -dDEPVERS_edlin_STR4=$(DEPVERS_edlin_STR4)
	"$(WIX)bin\light.exe" -nologo -cultures:null -out $@ "$(VSCMD_ARG_TGT_ARCH).wixobj"
	del "$(VSCMD_ARG_TGT_ARCH).wixobj"
	del "$(APPNAME)-$(DEPVERS_edlin_STR4)-$(VSCMD_ARG_TGT_ARCH).wixpdb"
	signtool sign /sha1 "$(CertificateThumbprint)" /fd SHA256 /t http://timestamp.digicert.com $@

$(MSIX): $(BUNDLE) $(APP)
	COPY /Y $(APP)
	DIR $(APPNAME).exe
	makeappx pack /m AppxManifest.xml /f mapping.ini /p $@
	DEL $(APPNAME).exe
	signtool sign /sha1 "$(BundleThumbprint)" /fd SHA256 /t http://timestamp.digicert.com $@

$(MSIXBUNDLE):
	makeappx bundle /d $(BUNDLE) /p $@
	signtool sign /sha1 "$(BundleThumbprint)" /fd SHA256 /t http://timestamp.digicert.com $@
