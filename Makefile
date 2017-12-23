default:
	g++ main.cpp -lsdl2 -O3 -o bin/game

debug:
	g++ main.cpp -lsdl2 -g3 -o bin/game

run: default
	./bin/game
