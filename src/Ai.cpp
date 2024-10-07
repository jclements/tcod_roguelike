#include "main.hpp"
#include <iostream>

// how many turns the monster chases the player
// after losing sight
static const int TRACKING_TURNS = 3;
const int LEVEL_UP_BASE = 200;
const int LEVEL_UP_FACTOR = 150;

PlayerAi::PlayerAi() : xpLevel(1) {
}

int PlayerAi::getNextLevelXp() {
	return LEVEL_UP_BASE + xpLevel * LEVEL_UP_FACTOR;
}

void PlayerAi::update(Actor *owner) {
	int levelUpXp = getNextLevelXp();
	if(owner->destructible->xp >= levelUpXp) {
		xpLevel++;
		owner->destructible->xp -= levelUpXp;
		engine.gui->message(TCODColor::yellow,"Your battle skills grow stronger! You reached level %d", xpLevel);

		engine.gui->menu.clear();
		engine.gui->menu.addItem(Menu::CONSTITUTION,"Constitution (+20HP)");
		engine.gui->menu.addItem(Menu::STRENGTH,"Strength (+1 attack)");
		engine.gui->menu.addItem(Menu::AGILITY,"Agility (+1 defense)");
		Menu::MenuItemCode menuItem = engine.gui->menu.pick(Menu::PAUSE);

		switch(menuItem) {
			case Menu::CONSTITUTION:
				owner->destructible->maxHp+=20;
				owner->destructible->hp+=20;
				break;
			case Menu::STRENGTH:
				owner->attacker->power+=1;
				break;
			case Menu::AGILITY:
				owner->destructible->defense+=1;
				break;
			default:break;
		}
	}

	if(owner->destructible && owner->destructible->isDead()) {
		return;
	}

	int dx=0, dy=0;
	switch(engine.lastKey)
	{
		case SDLK_UP: dy=-1; break; 
		case SDLK_DOWN: dy=1; break; 
		case SDLK_RIGHT: dx=1; break; 
		case SDLK_LEFT: dx=-1; break; 
		case SDLK_PERIOD:
			if(SDL_GetModState() & KMOD_SHIFT) {
				if(engine.stairs->x == owner->x && engine.stairs->y == owner->y) {
					engine.nextLevel();
				} else {
					engine.gui->message(TCODColor::lightGrey,"There are no stairs here.");
				}
			} break;
		default:
			if(engine.lastKey >= 33 && engine.lastKey <= 126) {
				handleActionKey(owner, SDL_GetKeyName(engine.lastKey)[0]);
			}
			break;
	}
	if(dx!=0 || dy!=0) {
		engine.gameStatus=Engine::NEW_TURN;
		if(moveOrAttack(owner, owner->x+dx, owner->y+dy)) {
			engine.map->computeFov();
		}
	}
}

bool PlayerAi::moveOrAttack(Actor *owner, int targetx, int targety) {
	if(engine.map->isWall(targetx, targety)) return false;
	// Look for living actors to attack
	for(auto actor: engine.actors) {
		if(actor->destructible && !actor->destructible->isDead()
			&& actor->x == targetx && actor->y == targety) {
			owner->attacker->attack(owner, actor);
			return false;
		}
	}
	// Look for corpses or items
	for(auto actor : engine.actors) {
		bool corpseOrItem = (actor->destructible && actor->destructible->isDead())
			|| actor->pickable;
		if(corpseOrItem	&& actor->x == targetx && actor->y == targety) {
			engine.gui->message(TCODColor::lightGrey,"There's a %s here\n", actor->name);
		}
	}

	owner->x = targetx;
	owner->y = targety;
	return true;
}

void PlayerAi::handleActionKey(Actor *owner, int ascii) {
	switch(ascii) {
		case 'G' : // pickup item
			{
				bool found=false;
				for(auto& actor : engine.actors) {
					char name[100] = "";
					strcat(name,actor->name);
					if(actor->pickable && actor->x == owner->x && actor->y == owner->y) {
						if(actor->pickable->pick(actor,owner)) {
							found = true;
							engine.gui->message(TCODColor::lightGrey,"You pick up the %s.", name);
							break;
						} else if(!found) {
							found = true;
							engine.gui->message(TCODColor::red,"Your inventory is full.");
						}
					}
				}
				if(!found) {
					engine.gui->message(TCODColor::lightGrey,"There's nothing here that you can pick up.");
				}
				engine.gameStatus=Engine::NEW_TURN;
		break;
		}
	case 'I' : // display inventory
		{
				Actor *actor=chooseFromInventory(owner);
				if(actor) {
					actor->pickable->use(actor,owner);
					engine.gameStatus=Engine::NEW_TURN;
				}
				break;
		}
	case 'D' : // drop item
		{
				Actor *actor=chooseFromInventory(owner);
				if(actor) {
					actor->pickable->drop(actor,owner);
					engine.gameStatus=Engine::NEW_TURN;
				}
				break;
		}
	case 'W' : // wait
		{
				engine.gameStatus=Engine::NEW_TURN;
				break;
		}

	default: break;
	} // end switch
} // end handleActionKey

Actor *PlayerAi::chooseFromInventory(Actor *owner) {
	static const int INVENTORY_WIDTH = 50;
	static const int INVENTORY_HEIGHT = 28;
	static TCODConsole con(INVENTORY_WIDTH, INVENTORY_HEIGHT);

	// display the inventory frame
	con.setDefaultForeground(TCODColor(200,180,50));
	con.printFrame(0,0,INVENTORY_WIDTH,INVENTORY_HEIGHT,true,
								TCOD_BKGND_DEFAULT,"inventory");

	// display the items with their keyboard shortcut
	con.setDefaultForeground(TCODColor::white);
	int shortcut='a';
	int y=1;
	for(auto actor: owner->container->inventory) {
		con.printf(2,y,"(%c) %s", shortcut, actor->name);
		y++;
		shortcut++;
	}

	// blit the inventory console on the root console
	TCODConsole::blit(&con,0,0,INVENTORY_WIDTH,INVENTORY_HEIGHT,
									 TCODConsole::root,engine.screenWidth/2 - INVENTORY_WIDTH/2,
									 engine.screenHeight/2 - INVENTORY_HEIGHT/2);
	TCODConsole::flush();

	// wait for a keypress
	SDL_Event event;
	bool running = true;
	while(running) {
		SDL_WaitEvent(&event);
		if(event.type == SDL_KEYDOWN) {
			SDL_Keysym keysym = event.key.keysym;
			if(keysym.sym >= SDLK_a && keysym.sym <= SDLK_z) {
				unsigned int actorIndex = keysym.sym - SDLK_a;
				running = false;

				// do we have that many items?
				if(actorIndex >= 0 && actorIndex < owner->container->inventory.size()) {
					unsigned int i = 0;
					for(auto actor : owner->container->inventory) {
						if(i == actorIndex) {
							return actor;
						}
						i++;
					}
					return NULL;
				}
				return NULL;
			}
		}
	}
	return NULL;
}

void MonsterAi::update(Actor *owner) {
	if(owner->destructible && owner->destructible->isDead()) {
		return;
	}
		moveOrAttack(owner, engine.player->x,engine.player->y);
}

void MonsterAi::moveOrAttack(Actor *owner, int targetx, int targety) {
	int dx = targetx - owner->x;
	int dy = targety - owner->y;
	float distance = sqrtf(dx*dx+dy*dy);

	if(distance < 2) {
		// at melee distance attack!
		if(owner->attacker) {
			owner->attacker->attack(owner,engine.player);
		}
		return;
	} else if(engine.map->isInFov(owner->x,owner->y)) {
		// player in sight, go towards him
		dx = (int)(round(dx/distance));
		dy = (int)(round(dy/distance));
		if(engine.map->canWalk(owner->x+dx,owner->y+dy)) {
			owner->x += dx;
			owner->y += dy;
			return;
		}
	}

	// player not visible, use scent tracking
	// find adjacent cell with the highest scent level
	unsigned int bestLevel=0;
	int bestCellIndex = -1;
	static int tdx[8] = {-1,0,1,-1,1,-1,0,1};
	static int tdy[8] = {-1,-1,-1,0,0,1,1,1};

	for(int i = 0; i < 8; i++) {
		int cellx = owner->x+tdx[i];
		int celly = owner->y+tdy[i];
		if(engine.map->canWalk(cellx,celly)) {
			unsigned int cellScent = engine.map->getScent(cellx,celly);
			if(cellScent > engine.map->currentScentValue - SCENT_THRESHOLD
					&& cellScent > bestLevel) {
				bestLevel = cellScent;
				bestCellIndex = i;
			}
		}
	}

	if(bestCellIndex != -1) {
		// the moster smells the player, follow the scent
		owner->x += tdx[bestCellIndex];
		owner->y += tdy[bestCellIndex];
	}
}

ConfusedMonsterAi::ConfusedMonsterAi(int nbTurns, Ai *oldAi)
	: nbTurns(nbTurns),oldAi(oldAi) {
}

void ConfusedMonsterAi::update(Actor *owner) {
	TCODRandom *rng=TCODRandom::getInstance();
	int dx=rng->getInt(-1,1);
	int dy=rng->getInt(-1,1);

	if(dx!=0 || dy!=0) {
		int destx = owner->x + dx;
		int desty = owner->y + dy;
		if(engine.map->canWalk(destx,desty)) {
			owner->x = destx;
			owner->y = desty;
		} else {
			Actor *actor=engine.getActor(destx, desty);
			if(actor) {
				owner->attacker->attack(owner,actor);
			}
		}
	}

	nbTurns--;
	if(nbTurns==0) {
		owner->ai = oldAi;
		delete this;
	}
}
