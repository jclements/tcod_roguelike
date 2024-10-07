#ifndef ATTACKER_HPP
#define ATTACKER_HPP

class Attacker : public Persistent {
public :
	float power;	// attack damage given
	
	Attacker(float power);
	void attack(Actor *owner, Actor *target);
	void load(TCODZip &zip);
	void save(TCODZip &zip);
};

#endif
