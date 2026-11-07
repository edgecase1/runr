.PHONY: all 

all: runr

runr: locker.c cgroups.c capabilities.c
	gcc -Wno-implicit-function-declaration -Wl,--no-as-needed -o runr locker.c cgroups.c capabilities.c -lcap

