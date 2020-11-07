CC=gcc

all: clean table guess

table:
	$(CC) table.c hashtable.c -o table -I/usr/include/openssl -lcrypto -lm

guess:
	$(CC) guess.c hashtable.c -o guess -I/usr/include/openssl -lcrypto -lm 

clean:
	rm -rf guess table hashtable