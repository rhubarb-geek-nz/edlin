# Copyright (c) 2024 Roger Brown.
# Licensed under the MIT License.

all: dist

APPNAME=edlin

dist: $(APPNAME)
	pkg/$$(uname)

$(APPNAME): src/$(APPNAME).c src/mbcs.c src/readline.c src/edposix.c config.h
	$(CC) $(CFLAGS) -I. -DHAVE_CONFIG_H src/$(APPNAME).c src/mbcs.c src/readline.c src/edposix.c -o $@

config.h: configure
	CFLAGS="$(CFLAGS)" ./configure

clean:
	rm -rf $(APPNAME) *.pkg *.deb *.rpm *.tgz *.txz *.pub *.ipk *.qpr *.hpkg config.h *.cat

install: $(APPNAME)
	if test -n "$(INSTALL)"; \
	then \
		$(INSTALL) -d "$(DESTDIR)/usr/bin"; \
	else \
		install -d "$(DESTDIR)/usr/bin"; \
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

$(APPNAME).cat: src/$(APPNAME).msg
	gencat $@ src/$(APPNAME).msg
