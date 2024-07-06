all:
	g++ -Isrc/include -Lsrc/lib -o main main.cpp Chip.cpp ChipFrame.cpp -lmingw32 -lSDL2main -lSDL2
