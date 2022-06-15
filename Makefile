OBJS = main.cpp chip8.cpp

CC = g++

COMPILER_FLAGS = -I.

LINKER_FLAGS = -lSDL2

OBJ_NAME = main

all : $(OBJS)
		$(CC) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)