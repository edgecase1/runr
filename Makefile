.PHONY: all 

all: locker

locker: locker.c locker.h utils.c utils.h cgroups.c cgroups.h
	gcc -Wall -Wl,--no-as-needed -o locker locker.c utils.c cgroups.c -lcap

