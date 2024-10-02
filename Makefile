# src = $(wildcard *.c)
# obj = $(src:.c=.o)
# CC = gcc
# LDFLAGS = -lnsl

# a.out: $(obj)
# 	$(CC) -o $@ $^ $(LDFLAGS)

# .PHONY: clean
# clean:
# 	rm -f $(obj) a.out

src = $(wildcard *.c)
obj = $(src:.c=.o)
CC = gcc
CFLAGS = -w  # Add this line to suppress warnings
LDFLAGS =    # Remove -lnsl since it's not needed on macOS


a.out: $(obj)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f $(obj) a.out