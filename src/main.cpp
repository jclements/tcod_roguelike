#include "libtcod.hpp"
#include <SDL2/SDL.h>
#include <iostream>
#include "Actor.hpp"
#include "Map.hpp"
#include "Engine.hpp"

Engine engine(80,52);

int main() {
	engine.load();

	while(engine.running && !TCODConsole::isWindowClosed()) {
		engine.update();
		engine.render();
		TCODConsole::flush();
	}
	std::cout << "Done" << std::endl;
	engine.save();
	return 0;
}
