CC     = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
TARGET = cparser
SRCS   = main.c scanner.c parser.c
OBJS   = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
