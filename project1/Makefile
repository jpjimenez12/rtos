CC=gcc
SRC=src
INC=include
OBJ=obj
PACKAGE=`pkg-config --libs gtk+-3.0`
LIBS=`pkg-config --cflags --libs gtk+-3.0`
CFLAGS=-g -Wall
EXE=buid/main

SRCS= $(SRC)/main.c $(SRC)/context.c $(SRC)/process.c
OBJS= $(OBJ)/main.o $(OBJ)/context.o $(OBJ)/process.objects

$(OBJS): $(SRCs)
	$(CC) -I $(INC)/ -c $(SRCS)

main: $(SRCS)
	$(CC) $(CFLAGS) $(LIBS) -I $(INC)/ -o $(EXE) $(OBJS)

clean:
	-rm -f *.o

clean all:
	-rm -f $(EXE)