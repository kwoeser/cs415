CC = gcc
CFLAGS = -W -Wall -g
my_exe = pseudo-shell
OBJECTS = main.o string_parser.o command.o

my_exe: $(my_exe)

$(my_exe): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(my_exe) $(OBJECTS)

main.o: main.c string_parser.h command.h
	$(CC) $(CFLAGS) -c main.c -o main.o

string_parser.o: string_parser.c string_parser.h
	$(CC) $(CFLAGS) -c string_parser.c -o string_parser.o

command.o: command.c command.h
	$(CC) $(CFLAGS) -c command.c -o command.o

clean:
	rm -f $(OBJECTS) $(my_exe)





