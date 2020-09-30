CC=gcc
CFLAGS=-g
OBJECTS=list.o hash.o bitmap.o testlib.o
TARGET=testlib

all: $(TARGET)

list.o: list.c list.h
	$(CC) -c list.c $(CFLAGS)

hash.o: hash.c hash.h
	$(CC) -c hash.c $(CFLAGS)

bitmap.o: bitmap.c bitmap.h limits.h round.h hex_dump.h
	$(CC) -c bitmap.c $(CFLAGS)

testlib.o: testlib.c testlib.h list.h hash.h bitmap.h
	$(CC) -c testlib.c $(CFLAGS)

$(TARGET): $(OBJECTS)
	$(CC) -o $(TARGET) $(OBJECTS) $(CFLAGS)

clean:
	rm -rf $(OBJECTS) $(TARGET)