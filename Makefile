# Builds everything as 32-bit i386 (needed by swtch.s).
# One-time setup:  sudo apt install gcc-multilib
CC     = gcc
CFLAGS = -m32 -g -Wall -fno-pie
LDFLAGS = -m32 -no-pie

OBJS = swtch.o chan.o sem.o
# add thread.o once thread.c exists (therad.c is currently misspelled)
# OBJS += thread.o

all: $(OBJS)

# swtch.s uses #if, so it must go through the C preprocessor
swtch.o: swtch.s
	$(CC) $(CFLAGS) -x assembler-with-cpp -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o
.PHONY: all clean
