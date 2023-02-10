SOURCE_FILES = Timer.cpp Chip8.cpp
BEFORE_FLAGS =  -g -I./include
AFTER_FLAGS = -lmingw32 -lSDL2main -lSDL2 -L./lib

all: main.cpp $(SOURCE_FILES)
	g++ $(BEFORE_FLAGS) main.cpp $(SOURCE_FILES) -o main.exe $(AFTER_FLAGS) 

test: test.cpp $(SOURCE_FILES)
	g++ $(BEFORE_FLAGS) test.cpp $(SOURCE_FILES) -o test.exe $(AFTER_FLAGS)

clean:
	rm *.exe
