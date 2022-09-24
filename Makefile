CFLAGS =
LDLIBS = -lX11 -lImlib2

BINDIR ?= /usr/bin

all: dsnip

clean:
	rm overdraw

dsnip: main.c
	$(CC) -o $@ $^ $(CFLAGS) $(LDLIBS)

install: dsnip
	install -m 755 dsnip $(DESTDIR)$(PREFIX)$(BINDIR)

uninstall:
	rm $(DESTDIR)$(PREFIX)$(DINDIR)/dsnip

.PHONY: all clean
