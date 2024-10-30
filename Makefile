all:
	g++ -Isrc/include -Lsrc/lib -o morpion main.cpp Game.cpp -lmingw32 -lSDL2main -lSDL2 -lm