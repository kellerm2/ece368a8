CC = gcc
CFLAGS = -g -O3 -std=c99 -Wall -Wshadow -Wvla -pedantic
RM = rm -f
VAL = valgrind --tool=memcheck --log-file=memcheck.txt --leak-check=full --verbose

SRCS = main.c
OBJS = $(SRCS:.c=.o)

TARGET = a8

# default: all
testmemory: $(TARGET)
	$(VAL) ./$(TARGET)

all: a8

a6: $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

clean:
	$(RM) $(TARGET) $(OBJS) memcheck.txt