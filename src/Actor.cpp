#include "main.hpp"
#include "math.h"

Actor::Actor(int x, int y, int ch, const char *name, const TCODColor &col) :
	x(x),y(y),ch(ch),name(name),col(col),
	blocks(true),fovOnly(true),attacker(NULL),destructible(NULL),ai(NULL),
	pickable(NULL),container(NULL) {}

Actor::~Actor() {
	if(attacker) delete attacker;
	if(destructible) delete destructible;
	if(ai) delete ai;
	if(pickable) delete pickable;
	if(container) delete container;
}

void Actor::render() const {
	TCODConsole::root->setChar(x,y,ch);
	TCODConsole::root->setCharForeground(x,y,col);
}

void Actor::update() {
	if(ai) ai->update(this);
}

float Actor::getDistance(int cx, int cy) const {
	int dx=x-cx;
	int dy=y-cy;
	return sqrtf(dx*dx+dy*dy);
}

