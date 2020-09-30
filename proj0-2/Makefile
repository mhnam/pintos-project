CC = gcc
CFLAGS = -W -Wall
TARGET = testlib

$(TARGET): testlib.o list.o bitmap.o hash.o
	   $(CC) $(CFLAGS) -o $(TARGET) testlib.o list.o bitmap.o hash.o

testlib.o: testlib.c testlib.h list.c list.h bitmap.c bitmap.h hash.c hash.h
	$(CC) $(CFLAGS) -c -o testlib.o testlib.c

list.o: list.c list.h
	$(CC) $(CFLAGS) -c -o list.o list.c

bitmap.o: bitmap.c bitmap.h
	$(CC) $(CFLAGS) -c -o bitmap.o bitmap.c

hash.o: hash.c hash.h
	$(CC) $(CFLAGS) -c -o hash.o hash.c

clean:
	rm *.o testlib