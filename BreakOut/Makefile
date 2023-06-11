CC=gcc

CC_SDL2_INCLUDE=
CC_SDL2_LIB=
CC_LIBS=-lmingw32 -lSDL2main -lSDL2 -mwindows

CC_FLAGS=-Wall -pedantic -std=c11

.PHONY: all
all:
	$(CC) $(CC_FLAGS) -o main main.c -I"$(CC_SDL2_INCLUDE)" -L"$(CC_SDL2_LIB)" $(CC_LIBS)
  
