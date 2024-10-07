#include "main.hpp"

Container::Container(int size) : size(size) {
}

Container::~Container() {
	for(Actor* actor : inventory) {
		delete actor;
	}
	inventory.clear();
}

bool Container::add(Actor *actor) {
	if(size>0 && inventory.size()>=size) {
		// inventory full
		return false;
	}
	inventory.push_back(actor);
	return true;
}

void Container::remove(Actor *actor) {
	inventory.remove(actor);
}
