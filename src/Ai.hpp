#ifndef AI_HPP
#define AI_HPP

class Ai : public Persistent {
public :
	virtual void update(Actor *owner)=0;
	virtual ~Ai() {};
	static Ai *create(TCODZip &zip);
protected :
	enum AiType {
		MONSTER,CONFUSED_MONSTER,PLAYER
	};
};

static const int SCENT_THRESHOLD = 50;

class PlayerAi : public Ai {
public :
	int xpLevel;
	PlayerAi();
	int getNextLevelXp();

	void update(Actor *owner);
	void handleActionKey(Actor *owner, int ascii);
	void load(TCODZip &zip);
	void save(TCODZip &zip);

protected :
	bool moveOrAttack(Actor *owner, int targetx, int targety);
	Actor *chooseFromInventory(Actor *owner);
};

class MonsterAi : public Ai {
public :
	void update(Actor *owner);
	void load(TCODZip &zip);
	void save(TCODZip &zip);
	
protected :
	void moveOrAttack(Actor *owner, int targetx, int targety);
	int moveCount;
};

class ConfusedMonsterAi : public Ai {
public:
	ConfusedMonsterAi(int nbTurns, Ai *oldAi);
	void update(Actor *owner);
	void load(TCODZip &zip);
	void save(TCODZip &zip);
protected:
	int nbTurns;
	Ai *oldAi;
};

#endif
