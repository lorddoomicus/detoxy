CC=gcc
OBJS=detoxy.o
CFLAGS=-Wall -O2 -pedantic -ansi
TARGET=detoxy

.c.o:	$(CC) -c $*.c

all:	$(OBJS)
	$(CC) -o $(TARGET) $(OBJS)

clean:
	rm -f $(OBJS) $(TARGET)

