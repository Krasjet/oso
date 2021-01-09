include config.mk

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
	rm -f oso *.o

.PHONY: clean
