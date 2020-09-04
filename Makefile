# This is a makefile for myShell.c
all:
	gcc myShell.c -o a.out

exec:
	./a.out

clean:
	rm -rf a.out
