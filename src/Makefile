.PHONY: all 

all: runr

runr: locker.c cgroups.c capabilities.c
	gcc -static -Wno-implicit-function-declaration -Wl,--no-as-needed -o runr-static locker.c cgroups.c capabilities.c -lcap
	gcc -Wno-implicit-function-declaration -Wl,--no-as-needed -o runr locker.c cgroups.c capabilities.c -lcap

