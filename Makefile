CC:=gcc
CFLAGS:=-Wall -O2
LIBS:=-lpthread

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

all: sync

sync: sync.o
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

clean:
	-rm sync sync.o

