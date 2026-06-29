CC = gcc
CFLAGS = -Wall -pthread
TARGET = formigopolis
SRCS = main.c monitor.c escalonador.c

all:
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET)

clean:
	rm -f $(TARGET)