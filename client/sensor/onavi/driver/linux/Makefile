CC=gcc -c

INC = -I/usr/src/linux-$(shell uname -r)/include

DEF = -D__KERNEL__ -DMODULE -DLINUX -DKERNEL2_0
SMP= $(shell if [ "`uname -a | grep SMP`" = "" ]; then echo "UP"; else echo "SMP"; fi)

ifeq ($(SMP),SMP)
	DEF += -D__SMP__
endif

CFLAGS= $(DEF) -O2 -Wall -Wstrict-prototypes -fomit-frame-pointer $(INC)

vizzini.o: vizzini.c
	$(CC) $(CFLAGS) vizzini.c

all: module

module: vizzini.o

clean:
	rm -rf *.o
