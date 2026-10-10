# Builds everything as 32-bit i386 (needed by swtch.s).
# One-time setup:  sudo apt install gcc-multilib
CC      = gcc
SAN     =
CFLAGS  = -m32 -g -Wall -fno-pie -MMD -MP -I. $(SAN)
LDFLAGS = -m32 -no-pie
BUILD   = build

OBJS = $(addprefix $(BUILD)/, swtch.o chan.o sem.o queue.o thread.o)
EXAMPLES = $(patsubst examples/%.c, $(BUILD)/%, $(wildcard examples/*.c))

all: $(OBJS) $(BUILD)/main
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

# table/queue unit test: includes thread.c directly, so it links
# queue.o and swtch.o but NOT thread.o
test_table: test_table.c $(BUILD)/queue.o $(BUILD)/swtch.o | $(BUILD)
	$(CC) $(CFLAGS) $(LDFLAGS) -fsanitize=address -DSTART_THREAD_COUNT=2 $< $(BUILD)/queue.o $(BUILD)/swtch.o -o $(BUILD)/$@

clean:
	rm -rf $(BUILD)
.PHONY: all examples test_table clean

-include $(wildcard $(BUILD)/*.d)