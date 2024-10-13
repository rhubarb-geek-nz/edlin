# Copyright (c) 2024 Roger Brown.
# Licensed under the MIT License.

all: dist

APPNAME=edlin
LANGLIST=de en es fr it

dist: $(APPNAME) readutf8
	pkg/$$(uname)

$(APPNAME): src/$(APPNAME).c src/mbcs.c src/readline.c posix/edposix.c config.h posix/codepage.c
	$(CC) -I. -Isrc -DHAVE_CONFIG_H src/$(APPNAME).c src/mbcs.c src/readline.c posix/edposix.c posix/codepage.c -o $@ $(CFLAGS)

config.h: configure
	CFLAGS="$(CFLAGS)" ./configure

readutf8: src/readutf8.c posix/codepage.c src/mbcs.c
	$(CC) -o $@ src/readutf8.c src/mbcs.c posix/codepage.c -I. -Isrc -DMBCS_LOOKUP -DHAVE_CONFIG_H $(CFLAGS)

clean:
	rm -rf $(APPNAME) *.pkg *.deb *.rpm *.tgz *.txz *.pub *.ipk *.qpr *.hpkg config.h *.cat *.gz *.mo readutf8

$(APPNAME).cat:
	for d in $(LANGLIST); do gencat $(APPNAME)-$$d.cat posix/nls/$(APPNAME)-$$d.msg; done

$(APPNAME).mo:
	for d in $(LANGLIST); do msgfmt -o $(APPNAME)-$$d.mo posix/nls/$(APPNAME)-$$d.po; done

$(APPNAME).1.gz: $(APPNAME).1
	gzip < $(APPNAME).1 >$@

install: $(APPNAME) $(APPNAME).1.gz $(APPNAME).mo
	if test -n "$(INSTALL)"; \
	then \
		$(INSTALL) -d "$(DESTDIR)/usr/bin"; \
		$(INSTALL) -d "$(DESTDIR)/usr/share/man/man1"; \
		for d in $(LANGLIST); do $(INSTALL) -d "$(DESTDIR)/usr/share/locale/$$d/LC_MESSAGES"; done; \
	else \
		install -d "$(DESTDIR)/usr/bin"; \
		install -d "$(DESTDIR)/usr/share/man/man1"; \
		for d in $(LANGLIST); do install -d "$(DESTDIR)/usr/share/locale/$$d/LC_MESSAGES"; done; \
	fi
	if test -n "$(INSTALL_PROGRAM)"; \
	then \
		$(INSTALL_PROGRAM) $(APPNAME) "$(DESTDIR)/usr/bin/$(APPNAME)"; \
	else \
		if test -n "$(INSTALL)"; \
		then \
			$(INSTALL) $(APPNAME) "$(DESTDIR)/usr/bin/$(APPNAME)"; \
		else \
			install $(APPNAME) "$(DESTDIR)/usr/bin/$(APPNAME)"; \
		fi; \
	fi
	if test -n "$(INSTALL_DATA)"; \
	then \
		$(INSTALL_DATA) $(APPNAME).1.gz "$(DESTDIR)/usr/share/man/man1/$(APPNAME).1.gz"; \
		for d in $(LANGLIST); do $(INSTALL_DATA) $(APPNAME)-$$d.mo "$(DESTDIR)/usr/share/locale/$$d/LC_MESSAGES/$(APPNAME).mo"; done; \
	else \
		if test -n "$(INSTALL)"; \
		then \
			$(INSTALL) -m 644 $(APPNAME).1.gz "$(DESTDIR)/usr/share/man/man1/$(APPNAME).1.gz"; \
			for d in $(LANGLIST); do $(INSTALL) -m 644 $(APPNAME)-$$d.mo "$(DESTDIR)/usr/share/locale/$$d/LC_MESSAGES/$(APPNAME).mo"; done; \
		else \
			install -m 644 $(APPNAME).1.gz "$(DESTDIR)/usr/share/man/man1/$(APPNAME).1.gz"; \
			for d in $(LANGLIST); do install -m 644 $(APPNAME)-$$d.mo "$(DESTDIR)/usr/share/locale/$$d/LC_MESSAGES/$(APPNAME).mo"; done; \
		fi; \
	fi
