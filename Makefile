CXX = g++
CXXFLAGS = -std=c++17 -Wall -g -I./SDL2/include  -Wno-narrowing -Wno-sign-compare
LDFLAGS = -L./SDL2/lib -lSDL2 -lSDL2_ttf -lSDL2_image

main: main.o players.o game.o achievements.o
	$(CXX) main.o players.o game.o achievements.o $(LDFLAGS) -o main

main.o: main.cpp players.h game.h achievements.h
	$(CXX) $(CXXFLAGS) -c main.cpp -o main.o

players.o: players.cpp players.h
	$(CXX) $(CXXFLAGS) -c players.cpp -o players.o

game.o: game.cpp game.h
	$(CXX) $(CXXFLAGS) -c game.cpp -o game.o

achievements.o: achievements.cpp achievements.h
	$(CXX) $(CXXFLAGS) -c achievements.cpp -o achievements.o

clean:
	rm -f *.o main