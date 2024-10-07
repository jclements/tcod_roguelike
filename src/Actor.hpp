#ifndef ACTOR_HPP
#define ACTOR_HPP

#include "main.hpp"

class Actor : public Persistent {
public:
	int x,y;					// position on map
	int ch;						// ascii code
	const char *name;	// the actor's name
	TCODColor col;		// color
	bool blocks;			// can we walk on this actor?
	bool fovOnly;			// only display when in fov
	Attacker *attacker;	// something that deals damage
	Destructible *destructible;	// something that can be damaged
	Ai *ai;		// something self-updating
	Pickable *pickable;		// something that can be picked up and used
	Container *container;		// something that can contain actors

	Actor(int x, int y, int ch, const char *name, const TCODColor &col);
	~Actor();
	void update();
	void render() const;
	float getDistance(int cx, int cy) const;
	void load(TCODZip &zip);
	void save(TCODZip &zip);
};

#endif
