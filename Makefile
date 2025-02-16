CC = gcc
CFLAGS = -Wall -g
LDFLAGS = -lm

SRC = lex.yy.c parser.tab.c map.c heap.c map_heap.c strings.c numbers.c handlers.c
OBJ = $(SRC:.c=.o)
EXEC = kompilator

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

lex.yy.c: lex.l parser.tab.h
	sudo flex $<

parser.tab.c parser.tab.h: parser.y
	sudo bison -d $<

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(EXEC) $(OBJ) lex.yy.c parser.tab.c parser.tab.h