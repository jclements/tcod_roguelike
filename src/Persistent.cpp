#include "main.hpp"
#include <iostream>
#include <filesystem>

void MonsterAi::load(TCODZip &zip) {
	moveCount = zip.getInt();
}

void MonsterAi::save(TCODZip &zip) {
	zip.putInt(MONSTER);
	zip.putInt(moveCount);
}

void ConfusedMonsterAi::load(TCODZip &zip) {
	nbTurns = zip.getInt();
	oldAi = Ai::create(zip);
}

void ConfusedMonsterAi::save(TCODZip &zip) {
	zip.putInt(CONFUSED_MONSTER);
	zip.putInt(nbTurns);
	oldAi->save(zip);
}

void PlayerAi::load(TCODZip &zip) {
	xpLevel = zip.getInt();
}

void PlayerAi::save(TCODZip &zip) {
	zip.putInt(PLAYER);
	zip.putInt(xpLevel);
}

Ai *Ai::create(TCODZip &zip) {
	AiType type = (AiType)zip.getInt();
	Ai *ai=NULL;
	switch(type) {
		case PLAYER : ai = new PlayerAi(); break;
		case MONSTER : ai = new MonsterAi(); break;
		case CONFUSED_MONSTER : ai = new ConfusedMonsterAi(0,NULL); break;
	}
	ai->load(zip);
	return ai;
}

void Healer::load(TCODZip &zip) {
	amount = zip.getFloat();
}

void Healer::save(TCODZip &zip) {
	zip.putInt(HEALER);
	zip.putFloat(amount);
}

void LightningBolt::load(TCODZip &zip) {
	range = zip.getFloat();
	damage = zip.getFloat();
}

void LightningBolt::save(TCODZip &zip) {
	zip.putInt(LIGHTNING_BOLT);
	zip.putFloat(range);
	zip.putFloat(damage);
}

void Confuser::load(TCODZip &zip) {
	nbTurns = zip.getInt();
	range = zip.getFloat();
}

void Confuser::save(TCODZip &zip) {
	zip.putInt(CONFUSER);
	zip.putInt(nbTurns);
	zip.putFloat(range);
}

void Fireball::save(TCODZip &zip) {
	zip.putInt(FIREBALL);
	zip.putFloat(range);
	zip.putFloat(damage);
}

Pickable *Pickable::create(TCODZip &zip) {
	PickableType type=(PickableType)zip.getInt();
	Pickable *pickable=NULL;
	switch(type) {
		case HEALER : pickable=new Healer(0); break;
		case LIGHTNING_BOLT : pickable=new LightningBolt(0,0); break;
		case CONFUSER : pickable=new Confuser(0,0); break;
		case FIREBALL : pickable=new Fireball(0,0); break;
	}
	pickable->load(zip);
	return pickable;
}

void PlayerDestructible::save(TCODZip &zip) {
	zip.putInt(PLAYER);
	Destructible::save(zip);
}

void MonsterDestructible::save(TCODZip &zip) {
	zip.putInt(MONSTER);
	Destructible::save(zip);
}

Destructible *Destructible::create(TCODZip &zip) {
	DestructibleType type = (DestructibleType)zip.getInt();
	Destructible *destructible = NULL;
	switch(type) {
		case MONSTER : destructible=new MonsterDestructible(0,0,"",0); break;
		case PLAYER : destructible=new PlayerDestructible(0,0,""); break;
	}
	destructible->load(zip);
	return destructible;
}

void Destructible::load(TCODZip &zip) {
	maxHp = zip.getFloat();
	hp = zip.getFloat();
	defense = zip.getFloat();
	corpseName = strdup(zip.getString());
	xp = zip.getInt();
}

void Destructible::save(TCODZip &zip) {
	zip.putFloat(maxHp);
	zip.putFloat(hp);
	zip.putFloat(defense);
	zip.putString(corpseName.c_str());
	zip.putInt(xp);
}

void Container::load(TCODZip &zip) {
	size = zip.getInt();
	int nbActors = zip.getInt();
	while(nbActors > 0) {
		Actor *actor = new Actor(0,0,0,NULL,TCODColor::white);
		actor->load(zip);
		inventory.push_back(actor);
		nbActors--;
	}
}

void Container::save(TCODZip &zip) {
	zip.putInt(size);
	zip.putInt(inventory.size());
	for(auto item : inventory) {
		item->save(zip);
	}
}

void Attacker::load(TCODZip &zip) {
	power = zip.getFloat();
}

void Attacker::save(TCODZip &zip) {
	zip.putFloat(power);
}

void Actor::save(TCODZip &zip) {
	zip.putInt(x);
	zip.putInt(y);
	zip.putInt(ch);
	zip.putColor(&col);
	zip.putString(name);
	zip.putInt(blocks);

	zip.putInt(attacker != NULL);
	zip.putInt(destructible != NULL);
	zip.putInt(ai != NULL);
	zip.putInt(pickable != NULL);
	zip.putInt(container != NULL);

	if(attacker) attacker->save(zip);
	if(destructible) destructible->save(zip);
	if(ai) ai->save(zip);
	if(pickable) pickable->save(zip);
	if(container) container->save(zip);
}

void Actor::load(TCODZip &zip) {
	x = zip.getInt();
	y = zip.getInt();
	ch = zip.getInt();
	col = zip.getColor();
	name = strdup(zip.getString());
	blocks = zip.getInt();

	bool hasAttacker = zip.getInt();
	bool hasDestructible = zip.getInt();
	bool hasAi = zip.getInt();
	bool hasPickable = zip.getInt();
	bool hasContainer = zip.getInt();

	if(hasAttacker) {
		attacker = new Attacker(0.0f);
		attacker->load(zip);
	}

	if(hasDestructible) {
		destructible = Destructible::create(zip);
	}

	if(hasAi) {
		ai = Ai::create(zip);
	}

	if(hasPickable) {
		pickable = Pickable::create(zip);
	}

	if(hasContainer) {
		container = new Container(0);
		container->load(zip);
	}
}

void Map::save(TCODZip &zip) {
	// zip.putInt(static_cast<int>(seed & 0xFFFFFFFF));	// lower 32 bits
	// zip.putInt(static_cast<int>(seed >> 32));					// upper 32 bits
	zip.putInt(seed);
	zip.putInt(currentScentValue);
	for(int i=0; i< width*height; i++) {
		zip.putInt(tiles[i].explored);
		zip.putInt(tiles[i].scent);
	}
}

void Map::load(TCODZip &zip) {
	// int low = zip.getInt();
	// int high = zip.getInt();
	// seed = static_cast<long>(low) | (static_cast<long>(high) << 32);
	seed = zip.getInt();
	currentScentValue = zip.getInt();
	init(false);
	for(int i=0; i< width*height; i++) {
		tiles[i].explored=zip.getInt();
		tiles[i].scent=zip.getInt();
	}
}

void Gui::save(TCODZip &zip) {
	zip.putInt(log.size());
	for(auto message : log) {
		zip.putString(message->text.c_str());
		zip.putColor(&message->col);
	}
}

void Gui::load(TCODZip &zip) {
	int nbMessages = zip.getInt();
	while(nbMessages > 0) {
		const char *text=zip.getString();
		TCODColor col = zip.getColor();
		message(col,text);
		nbMessages--;
	}
}

void Engine::save() {
	if(player->destructible->isDead()) {
		std::remove("game.sav");
	} else {
		TCODZip zip;
		// save the map first
		zip.putInt(map->width);
		zip.putInt(map->height);
		map->save(zip);
		// then the player
		player->save(zip);
		// then the stairs
		stairs->save(zip);
		// and the level
		zip.putInt(level);
		// then all the other actors
		zip.putInt(actors.size()-2);
		for(auto& actor : actors) {
			if(actor != player && actor != stairs) {
				actor->save(zip);
			}
		}
		// finally the message log
		gui->save(zip);
		zip.saveToFile("game.sav");
	}
}

void Engine::load() {
	engine.gui->menu.clear();
	engine.gui->menu.addItem(Menu::NEW_GAME,"New game");
	if(std::filesystem::exists("game.sav")) {
		engine.gui->menu.addItem(Menu::CONTINUE,"Continue");
	}
	engine.gui->menu.addItem(Menu::EXIT,"Exit");

	Menu::MenuItemCode menuItem=engine.gui->menu.pick();
	if(menuItem == Menu::EXIT || menuItem == Menu::NONE) {
		// exit or window closed
		exit(0);
	} else if(menuItem == Menu::NEW_GAME) {
		// new game
		engine.term();
		engine.init();
	} else {
		TCODZip zip;
		// continue a saved game
		engine.term();
		zip.loadFromFile("game.sav");
		
		// load the map
		int width = zip.getInt();
		int height = zip.getInt();
		map = new Map(width,height);
		map->load(zip);

		// then the player
		player = new Actor(0,0,0,NULL,TCODColor::white);
		actors.push_back(player);
		player->load(zip);

		// then the stairs
		stairs = new Actor(0,0,0,NULL,TCODColor::white);
		stairs->load(zip);
		actors.push_back(stairs);

		// and the level
		level = zip.getInt();

		// then all the other actors
		int nbActors = zip.getInt();
		while(nbActors > 0) {
			Actor *actor = new Actor(0,0,0,NULL,TCODColor::white);
			actor->load(zip);
			actors.push_back(actor);
			nbActors--;
		}

		// finally the message log
		gui->load(zip);

		// to force FOV recomp
		gameStatus = STARTUP;
	}
}
