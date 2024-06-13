all:
	g++ -Isrc/include -Lsrc/lib -std=c++11 -o task_201 task_201.cpp -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_mixer -lSDL2_image
	