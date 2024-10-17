CC = gcc
CFLAGS = -W -Wall -g
OBJ = part1 part2 part3 part4 cpubound iobound

all: $(OBJ)

part1: part1.c
	$(CC) $(CFLAGS) -o part1 part1.c

part2: part2.c
	$(CC) $(CFLAGS) -o part2 part2.c

part3: part3.c
	$(CC) $(CFLAGS) -o part3 part3.c

part4: part4.c
	$(CC) $(CFLAGS) -o part4 part4.c

cpubound: cpubound.c
	$(CC) $(CFLAGS) -o cpubound cpubound.c

iobound: iobound.c
	$(CC) $(CFLAGS) -o iobound iobound.c

.PHONY: clean
clean:
	rm -f $(OBJ)