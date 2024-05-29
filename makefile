all:
	g++ -I src/include -L src/lib -o task_201 task_201.cpp -lmingw32 -lSDL2main -lSDL2
	