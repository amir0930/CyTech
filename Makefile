PROG = projet

SRC = main.c

all: $(PROG)
	./$(PROG)

$(PROG): $(SRC) 
	gcc -Wall -o $(PROG) $(SRC)

clean :
	rm -f $(PROG)
