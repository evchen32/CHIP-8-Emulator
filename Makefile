all:
	g++ -I./include -L./lib -g main.cpp -o main.exe -lmingw32 -lSDL2main -lSDL2
