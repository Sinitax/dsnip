LDLIBS = -lX11 -lImlib2

PREFIX ?= /usr/local
BINDIR ?= /bin

all: dsnip

clean:
	rm -f dsnip

dsnip: dsnip.c

install: dsnip
	install -m 755 dsnip -t "$(DESTDIR)$(PREFIX)$(BINDIR)"

uninstall:
	rm -f "$(DESTDIR)$(PREFIX)$(BINDIR)/dsnip"

.PHONY: all clean install uninstall
