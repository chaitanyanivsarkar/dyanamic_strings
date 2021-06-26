CC=gcc

test: test.o
	$(CC) -o test test.c

clean:
	rm -f test test.o
