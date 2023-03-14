PREFIX ?= /usr/local
BINDIR ?= /bin

LDLIBS = -lX11 -lImlib2

all: dsnip

clean:
	rm -f dsnip

install: dsnip
	install -m 755 dsnip -t "$(DESTDIR)$(PREFIX)$(BINDIR)"

uninstall:
	rm -f "$(DESTDIR)$(PREFIX)$(BINDIR)/dsnip"

.PHONY: all clean install uninstall
