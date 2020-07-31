CC = gcc

all: verifyme.so

verifyme.so: verifyme.c
	gcc -Wall -shared verifyme.c -o verifyme.so -fPIC -ldl

clean:
	rm verifyme.so
