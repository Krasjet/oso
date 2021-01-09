include config.mk

PREFIX = /usr/local

CC = cc
CFLAGS = -std=c99 -Wall -Wextra -pedantic -g -O3 $(JACK_CFLAGS) $(SDL_CFLAGS)
LDLIBS = $(SDL_LDLIBS) $(JACK_LDLIBS)
LDFLAGS =

OBJ = oso.o jack.o util.o gui.o btbuf.o

oso: $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJ) $(LDLIBS)

oso.o: jack.h gui.h oso.h btbuf.h util.h
jack.o: oso.h jack.h util.h
gui.o: oso.h btbuf.h util.h
btbuf.o: btbuf.h

.c.o:
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f oso *.o *.exe

install: oso oso.1
	install -d $(DESTDIR)$(PREFIX)/bin
	install -d $(DESTDIR)$(PREFIX)/share/man/man1
	install -m 755 oso $(DESTDIR)$(PREFIX)/bin
	gzip < oso.1 > $(DESTDIR)$(PREFIX)/share/man/man1/oso.1.gz

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/oso
	rm -f $(DESTDIR)$(PREFIX)/share/man/man1/oso.1.gz

.PHONY: clean install uninstall
