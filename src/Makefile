.PHONY: all 

all: locker

locker: locker.c cgroups.c capabilities.c
	gcc -Wno-implicit-function-declaration -Wl,--no-as-needed -o locker locker.c cgroups.c capabilities.c -lcap

