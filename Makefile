CC = gcc
CFLAGS = -Wextra -Wall -Wconversion -O3
source = hw1_5 hw1_6

ifeq ("$(DEBUG)", "1")
	CFLAGS += -g
	source += bigint10-test
else
	CFLAGS += -DNDEBUG
endif

all: $(source)

hw1_5: hw1_5.c
	$(CC) $(CFLAGS) $< -o $@
hw1_6: hw1_6.c bigint10.c
	$(CC) $(CFLAGS) $^ -o $@
bigint-test: bigint-test.c bigint.c
	$(CC) $(CFLAGS) $^ -o $@
bigint10-test: bigint10-test.c bigint10.c
	$(CC) $(CFLAGS) $^ -o $@
clean:
	rm -rf $(source) *.dSYM
run5:
	./hw1_5 < input
run6:
	./hw1_6 < input