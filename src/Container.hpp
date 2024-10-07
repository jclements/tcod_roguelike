#ifndef CONTAINER_HPP
#define CONTAINER_HPP
#include <list>

class Container : public Persistent {
public:
	unsigned int size;		// maximum number of actors. 0=unlimited
	std::list<Actor *> inventory;

	Container(int size);
	~Container();
	bool add(Actor *actor);
	void remove(Actor *actor);
	void load(TCODZip &zip);
	void save(TCODZip &zip);
};

#endif // !CONTAINER_HPP
