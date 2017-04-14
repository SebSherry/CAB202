all: Program

#LIBS=
#DIRS=
FLAGS=-std=c11

clean:
	rm *.exe

rebuild: clean all

SRC=main.c
HDR=main.h

Program: $(SRC) $(HDR)
	gcc $(FLAGS) $(SRC) -o $@ #$(DIRS) $(LIBS)
