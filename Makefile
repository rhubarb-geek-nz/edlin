# Copyright (c) 2024 Roger Brown.
# Licensed under the MIT License.

all: dist

dist: edlin
	pkg/$$(uname)

edlin: src/edlin.c src/mbcs.c src/readline.c src/edposix.c config.h
	$(CC) $(CFLAGS) -I. -DHAVE_CONFIG_H src/edlin.c src/mbcs.c src/readline.c src/edposix.c -o $@

config.h: configure
	CFLAGS="$(CFLAGS)" ./configure

clean:
	rm -rf edlin *.pkg *.deb *.rpm *.tgz *.txz *.pub *.ipk *.qpr *.hpkg config.h *.cat

install: edlin
	if test -n "$(INSTALL)"; \
	then \
		$(INSTALL) -d "$(DESTDIR)/usr/bin"; \
	else \
		install -d "$(DESTDIR)/usr/bin"; \
	fi
	if test -n "$(INSTALL_PROGRAM)"; \
	then \
		$(INSTALL_PROGRAM) edlin "$(DESTDIR)/usr/bin/edlin"; \
	else \
		if test -n "$(INSTALL)"; \
		then \
			$(INSTALL) edlin "$(DESTDIR)/usr/bin/edlin"; \
		else \
			install edlin "$(DESTDIR)/usr/bin/edlin"; \
		fi; \
	fi

edlin.cat: edlin.msg
	gencat edlin.cat edlin.msg
