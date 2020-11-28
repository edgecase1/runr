.PHONY: all 

all: locker

locker: locker.c
	gcc -Wall -Wl,--no-as-needed -o locker locker.c utils.c -lcap

