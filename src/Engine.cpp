#include "main.hpp"
#include <SDL2/SDL.h>
#include <iostream>
#include "math.h"


Engine::Engine(int screenWidth, int screenHeight) : gameStatus(STARTUP),
	player(NULL),map(NULL),running(true),
	screenWidth(screenWidth),screenHeight(screenHeight),fovRadius(10),
	mousex(0),mousey(0),level(1) {
	TCODConsole::initRoot(screenWidth,screenHeight,"libtcod C++ tutorial",false);
	gui = new Gui();
	computeFov=true;
}

Engine::~Engine() {
	term();
	delete gui;
}

void Engine::term() {
	for(auto actor : actors) {
		delete(actor);
	}
	actors.clear();
	if(map) delete map;
	gui->clear();
}

void Engine::init() {
	player = new Actor(40,25,'@',"player",TCODColor::white);
	player->destructible = new PlayerDestructible(30,2,"your cadaver");
	player->attacker = new Attacker(5);
	player->ai = new PlayerAi();
	player->container = new Container(26);
	actors.push_back(player);
	stairs = new Actor(0,0,'>',"stairs",TCODColor::white);
	stairs->blocks = false;
	stairs->fovOnly = false;
	actors.push_back(stairs);
	map = new Map(80,45);
	map->init(true);
	gui->message(TCODColor::red,
							"Welcome stranger!\nPrepare to perish in the Tombs of the Ancient Kings.");
	gameStatus=STARTUP;
}

void Engine::update() {
	SDL_Event event;
	SDL_PollEvent(&event);
	if(gameStatus==STARTUP) map->computeFov();
	gameStatus=IDLE;
	if(event.type == SDL_QUIT) {
		running = false;
	}
	if(event.type == SDL_KEYDOWN) {
		lastKey = event.key.keysym.sym;
		if(lastKey == SDLK_ESCAPE) {
			save();
			load();
		} 
		player->update();
		if(gameStatus==NEW_TURN) {
			map->currentScentValue++;
			for(auto actor: engine.actors) {
				if(actor!=player) {
					actor->update();
				}
			}
		}
	}
	if(event.type==SDL_MOUSEMOTION) {
		SDL_GetMouseState(&mousex,&mousey);
		mousex /= 8;
		mousey /= 8;
		// std::cout << "x:" << mousex << ", y:" << mousey << std::endl;
		
	}
}

void Engine::render() {
	TCODConsole::root->clear();
	// draw the map
	map->render();

	// show the player's stats
	gui->render();

	// draw the actors
	for(auto actor:actors) {
		if((!actor->fovOnly && map->isExplored(actor->x,actor->y)) || map->isInFov(actor->x,actor->y)) {
			actor->render();
		}
	}
}

void Engine::sendToBack(Actor *actor) {
	actors.remove(actor);
	actors.push_front(actor);
}

Actor *Engine::getClosestMonster(int x, int y, float range) const {
	Actor *closest = NULL;
	float bestDistance=1E6f;
	for(auto actor : actors) {
		if(actor != player && actor->destructible && !actor->destructible->isDead()) {
			float distance = actor->getDistance(x,y);
			if(distance < bestDistance && (distance<=range || range==0.0f)) {
				bestDistance = distance;
				closest = actor;
			}
		}
	}
	return closest;
}

bool Engine::pickATile(int *x, int *y, float maxRange, float range) {
	while(!TCODConsole::isWindowClosed()) {
		render();

		// highlight the possible range
		for(int cx=0; cx < map->width; cx++) {
			for(int cy=0; cy < map->height; cy++) {
				if(map->isInFov(cx,cy)
					&& (maxRange==0 || player->getDistance(cx,cy)<=maxRange)) {
					TCODColor col = TCODConsole::root->getCharBackground(cx,cy);
					col = col*1.2f;
					TCODConsole::root->setCharBackground(cx,cy,col);
				}
			}
		}

		SDL_Event event;
		SDL_PollEvent(&event);
		int mousecx, mousecy;
		SDL_GetMouseState(&mousecx,&mousecy);
		mousecx /= 8;
		mousecy /= 8;
		if(map->isInFov(mousecx,mousecy) 
			&& (maxRange==0 || player->getDistance(mousecx,mousecy)<=maxRange)) {
			TCODConsole::root->setCharBackground(mousecx,mousecy,TCODColor::white);
			if(range > 0) {
				for(int i = -range; i<=range; i++) {
					for(int j = -range; j<=range; j++) {
						if(sqrt(i*i+j*j) <= range) {
							TCODColor col = TCODConsole::root->getCharBackground(mousecx+i, mousecy+j);
							col = col*1.2f;
							TCODConsole::root->setCharBackground(mousecx+i, mousecy+j,col);
						}
					}
				}
			}

			if(event.type == SDL_MOUSEBUTTONDOWN) {
				if(event.button.button == SDL_BUTTON_LEFT) {
					*x=mousecx;
					*y=mousecy;
					return true;
				}
			}
		}
		if(event.button.button == SDL_BUTTON_RIGHT) {
			return false;
		}
		TCODConsole::flush();
	}
	return false;
}

Actor *Engine::getActor(int x, int y) const {
	for(auto actor : actors) {
		if(actor->x==x && actor->y==y && actor->destructible && !actor->destructible->isDead()) {
			return actor;
		}
	}
	return NULL;
}

void Engine::nextLevel() {
	level++;
	gui->message(TCODColor::lightViolet,"You take a moment to rest, and recover your strength.");
	player->destructible->heal(player->destructible->maxHp/2);
	gui->message(TCODColor::red,"After a rare moment of peace, you decend\ndeeper into the heart of the dungeon...");

	delete map;
	// delete all actors but player and stairs
	for(auto it=actors.begin(); it!=actors.end(); ) {
		Actor* actor = *it;
		if(actor != player && actor != stairs) {
			it = actors.erase(it);
			delete(actor);
		} else {
			it++;
		}
	}

	// create a new map
	map = new Map(80,43);
	map->init(true);
	gameStatus = STARTUP;
}
