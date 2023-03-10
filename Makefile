all: compile link run

compile:
	g++ -Isrc/include -c *.cpp -o build/main.o

link:
	g++ build/*.o -o build/game -Lsrc/lib -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio

run:
	cd build && ./game.exe