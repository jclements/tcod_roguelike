#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <SDL2/SDL.h>
#include "Gui.hpp"
#include <list>

class Actor;
class Map;

class Engine {
public :
	enum GameStatus {
		STARTUP,
		IDLE,
		NEW_TURN,
		VICTORY,
		DEFEAT
	} gameStatus;
	
	std::list<Actor *> actors;
	Actor *player;
	Actor *stairs;
	Map *map;
	bool running;
	int screenWidth;
	int screenHeight;
	Gui *gui;
	SDL_Keycode lastKey;
	int fovRadius;
	int mousex, mousey;
	int level;

	Engine(int screenWidth, int screenHeight);
	~Engine();
	void update();
	void render();
	void sendToBack(Actor *actor);
	void nextLevel();

	Actor *getClosestMonster(int x, int y, float range) const;
	bool pickATile(int *x, int *y, float maxRange = 0.0f, float range = 0.0f);
	Actor *getActor(int x, int y) const;

	void init();
	void term();
	void load();
	void save();

private :
	bool computeFov;
};

extern Engine engine;

#endif
