#ifndef DESTRUCTIBLE_HPP
#define DESTRUCTIBLE_HPP

class Destructible : public Persistent {
public :
	float maxHp;	// maximum health points
	float hp;			// current health points
	float defense;	// hit points deflected
	std::string corpseName;	// the actor's name once dead/destroyed
	int xp;				// XP gained when killing this monster (or player xp)

	Destructible(float maxHp, float defense, std::string corpseName, int xp);
	inline bool isDead() {return hp <= 0;}

	float takeDamage(Actor *owner, float damage);
	float heal(float amount);

	virtual void die(Actor *owner);
	virtual ~Destructible();

	void load(TCODZip &zip);
	void save(TCODZip &zip);
	static Destructible *create(TCODZip &zip);

protected:
	enum DestructibleType {
		MONSTER,PLAYER
	};
};

class MonsterDestructible : public Destructible {
public :
	MonsterDestructible(float maxHp, float defense, std::string corpseName, int xp);
	void die(Actor *owner);
	void save(TCODZip &zip);
};

class PlayerDestructible : public Destructible {
public :
	PlayerDestructible(float maxHp, float defense, std::string corpseName);
	void die(Actor *owner);
	void save(TCODZip &zip);
};

#endif
