CFLAGS =
LDLIBS = -lX11 -lImlib2

.PHONY: all clean

all: dsnip

clean:
	rm overdraw

dsnip: main.c
	$(CC) -o $@ $^ $(CFLAGS) $(LDLIBS)
