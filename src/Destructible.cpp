#include <stdio.h>
#include <cstring>
#include "main.hpp"
#include <string>

Destructible::Destructible(float maxHp, float defense, std::string corpseName, int xp) :
	maxHp(maxHp),hp(maxHp),defense(defense),corpseName(corpseName),xp(xp) {
}

Destructible::~Destructible() {
}

float Destructible::takeDamage(Actor *owner, float damage) {
	damage -= defense;
	if(damage>0) {
		hp -= damage;
		if(hp <= 0) {
			die(owner);
		}
	} else {
		damage=0;
	}
	return damage;
}

void Destructible::die(Actor *owner) {
	// transform the actor into a corpse
	owner->ch='%';
	owner->col=TCODColor::darkRed;
	owner->name=corpseName.c_str();
	owner->blocks=false;
	// make sure corpses are drawn before living actors
	engine.sendToBack(owner);
}

MonsterDestructible::MonsterDestructible(float maxHp, float defense, std::string corpseName, int xp) :
	Destructible(maxHp, defense, corpseName, xp) {
}

void MonsterDestructible::die(Actor *owner) {
	// transform it in to a corpse, doesn't block can't be
	// attacked and doesn't move
	engine.gui->message(TCODColor::lightGrey,"%s is dead. You gain %d xp",owner->name, xp);
	engine.player->destructible->xp += xp;
	Destructible::die(owner);
}

PlayerDestructible::PlayerDestructible(float maxHp, float defense, std::string corpseName) :
	Destructible(maxHp, defense, corpseName, 0) {
}

void PlayerDestructible::die(Actor *owner) {
	engine.gui->message(TCODColor::lightGrey,"You died!\n");
	Destructible::die(owner);
	engine.gameStatus=Engine::DEFEAT;
}

float Destructible::heal(float amount) {
	hp += amount;
	if(hp > maxHp) {
		amount -= hp-maxHp;
		hp=maxHp;
	}
	return amount;
}
