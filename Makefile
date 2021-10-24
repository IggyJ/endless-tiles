SRC = src/*.c
OUT = game
CC  = gcc
CFLAGS = `sdl2-config --libs --cflags` -lSDL2 -lSDL2_image -lSDL2_ttf -lm


build: $(SRC)
	$(CC) $(SRC) -o $(OUT) $(CFLAGS)


clean: 
	rm game
