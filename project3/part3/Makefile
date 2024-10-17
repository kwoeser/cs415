cc = gcc
CFLAGS = -g -W -Wall
SRCS = bank.c
DEPS = string_parser.h account.h
OBJECTS = bank.o 
PROGRAMS = bank

$(PROGRAMS): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS)

$(OBJECTS): $(SRCS) $(HEADERS)
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf $(OBJECTS) $(PROGRAMS)
	rm -r output

.PHONY: clean
