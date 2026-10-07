# Builds everything as 32-bit i386 (needed by swtch.s).
# One-time setup:  sudo apt install gcc-multilib
# All objects and executables go in build/.
CC     = gcc
CFLAGS = -m32 -g -Wall -fno-pie -I.
LDFLAGS = -m32 -no-pie
BUILD  = build

OBJS = $(addprefix $(BUILD)/, swtch.o chan.o sem.o queue.o thread.o)

EXAMPLES = $(patsubst examples/%.c, $(BUILD)/%, $(wildcard examples/*.c))

all: $(OBJS) $(BUILD)/main

# executables (need thread.o, so enable it above first):  make examples
examples: $(EXAMPLES)

$(BUILD):
	mkdir -p $(BUILD)

# swtch.s uses #if, so it must go through the C preprocessor
$(BUILD)/swtch.o: swtch.s | $(BUILD)
	$(CC) $(CFLAGS) -x assembler-with-cpp -c $< -o $@

$(BUILD)/%.o: %.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/main: main.c $(OBJS) | $(BUILD)
	$(CC) $(CFLAGS) $(LDFLAGS) $< $(OBJS) -o $@

$(BUILD)/%: examples/%.c $(OBJS) | $(BUILD)
	$(CC) $(CFLAGS) $(LDFLAGS) $< $(OBJS) -o $@

clean:
	rm -rf $(BUILD)
.PHONY: all examples clean
