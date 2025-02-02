#include "main.hpp"
#include "stdarg.h"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

static const int PANEL_HEIGHT=7;
static const int BAR_WIDTH=20;
static const int MSG_X=BAR_WIDTH+2;
static const int MSG_HEIGHT=PANEL_HEIGHT-1;
const int PAUSE_MENU_WIDTH = 30;
const int PAUSE_MENU_HEIGHT = 15;

Gui::Gui() {
	con = new TCODConsole(engine.screenWidth,PANEL_HEIGHT);
}

Gui::~Gui() {
	delete con;
	clear();
}

void Gui::clear() {
	for(auto entry : log) {
		delete(entry);
	}
	log.clear();
}

void Gui::render() {
	// clear the GUI console
	con->setDefaultBackground(TCODColor::black);
	con->clear();

	// draw the health bar
	renderBar(1,1,BAR_WIDTH,"HP",engine.player->destructible->hp,
		engine.player->destructible->maxHp, TCODColor::lightRed,
		TCODColor::darkerRed);

	// draw the XP bar
	PlayerAi *ai = (PlayerAi *)engine.player->ai;
	std::string xpTxt = "XP(" + std::to_string(ai->xpLevel) + ")";
	renderBar(1,5,BAR_WIDTH,xpTxt.c_str(),engine.player->destructible->xp, ai->getNextLevelXp(), TCODColor::lightViolet,TCODColor::darkerViolet);
	
	// draw the message log
	int y=1;
	float colorCoef=0.4f;
	for(auto message: log) {
		con->setDefaultForeground(message->col*colorCoef);
		con->printf(MSG_X,y,"%s",message->text.c_str());
		y++;
		if(colorCoef<1.0f) {
			colorCoef+=0.3f;
		}
	}

	// mouse look
	renderMouseLook();

	// dungeon level
	con->setDefaultForeground(TCODColor::white);
	con->printf(3,3,"Dungeon level %d",engine.level);

	// blit the GUI console on the root console
	TCODConsole::blit(con,0,0,engine.screenWidth,PANEL_HEIGHT,
		TCODConsole::root,0,engine.screenHeight-PANEL_HEIGHT);

}

void Gui::renderBar(int x, int y, int width, const char *name, 
										float value, float maxValue, const TCODColor &barColor,
										const TCODColor &backColor) {
	std::ostringstream oss;
	oss << name << " : " << value << "/" << maxValue;
	std::string message = oss.str();

	// fill the background
	con->setDefaultBackground(backColor);
	con->rect(x,y,width,1,false,TCOD_BKGND_SET);

	int barWidth = (int)(value / maxValue * width);
	if(barWidth>0) {
		// draw the bar
		con->setDefaultBackground(barColor);
		con->rect(x,y,barWidth,1,false,TCOD_BKGND_SET);
	}


	// print text on top of the bar
	con->setDefaultForeground(TCODColor::white);
	// con->printEx(x+width/2,y,TCOD_BKGND_NONE,TCOD_CENTER,"%s", message.c_str());	// working but deprecated
	con->print(x+width/2,y,message,TCOD_CENTER,TCOD_BKGND_NONE);
}

Gui::Message::Message(std::string &text, const TCODColor &col):
	text(text),col(col) {
}

Gui::Message::~Message() {
}

void Gui::message(const TCODColor &col, std::string text, ...) {
	std::stringstream messageStream;

	// initialize
	va_list args;
	va_start(args,text);

	// format message
	char buf[512];
	vsnprintf(buf, sizeof(buf), text.c_str(), args);
	va_end(args);

	// populate string stream
	messageStream << buf;

	// convert to string
	std::string messageText = messageStream.str();

	// process the text
	std::string::size_type start = 0;
	std::string::size_type end;

	do {
		// make room for the new message
		if(log.size() == MSG_HEIGHT) {
			Message *toRemove=log.front();
			log.pop_front();
			delete toRemove;
		}

		// detect end of the line
		end = messageText.find('\n', start);
		std::string line = messageText.substr(start, end - start);

		// add a new message to the log
		Message *msg=new Message(line, col);
		log.push_back(msg);

		// go to the next line
		start = end + 1;
	} while (end != std::string::npos);
}

void Gui::renderMouseLook() {
	if(!engine.map->isInFov(engine.mousex, engine.mousey)) {
		// if mouse is out of fov, nothing to render
		return;
	}

	std::string buf;
	bool first=true;
	for(auto actor: engine.actors) {
		if(actor->x == engine.mousex && actor->y == engine.mousey) {
			if(!first) {
				buf += ", ";
			} else {
				first=false;
			}
			buf += actor->name;
		}
	}
	
	// display the list of actors under the mouse cursor
	con->setDefaultForeground(TCODColor::lightGrey);
	con->printf(1,0,"%s",buf.c_str());
}

Menu::~Menu() {
	clear();
}

void Menu::clear() {
	for(auto item : items) {
		delete(item);
	}
	items.clear();
}

void Menu::addItem(MenuItemCode code, const char *label) {
	MenuItem *item = new MenuItem();
	item->code = code;
	item->label = label;
	items.push_back(item);
}

Menu::MenuItemCode Menu::pick(DisplayMode mode) {
	int selectedItem=0;
	int menux,menuy;

	if(mode == PAUSE) {
		menux = engine.screenWidth/2 - PAUSE_MENU_WIDTH/2;
		menuy = engine.screenHeight/2 - PAUSE_MENU_HEIGHT/2;
		TCODConsole::root->setDefaultForeground(TCODColor(200,180,50));
		TCODConsole::root->printFrame(menux,menuy,PAUSE_MENU_WIDTH,PAUSE_MENU_HEIGHT,true,TCOD_BKGND_ALPHA(70),"menu");
		menux += 2;
		menuy += 3;
	} else {
		static TCODImage img("menu_background1.png");
		img.blit2x(TCODConsole::root,0,0);
		menux = 10;
		menuy = TCODConsole::root->getHeight()/3;
	}
	
	while(!TCODConsole::isWindowClosed()) {

		int currentItem = 0;
		for(auto it : items) {
			if(currentItem == selectedItem) {
				TCODConsole::root->setDefaultForeground(TCODColor::lighterOrange);
			} else {
				TCODConsole::root->setDefaultForeground(TCODColor::lighterGrey);
			}
			TCODConsole::root->printf(menux,menuy+currentItem*3,"%s",it->label);
			currentItem++;
		}
		TCODConsole::flush();

		// check key presses
		TCOD_key_t key;
		TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS,&key,NULL);
		switch(key.vk) {
			case TCODK_UP:
				selectedItem--;
				if(selectedItem < 0) {
					selectedItem = items.size() - 1;
				}
			break;

			case TCODK_DOWN:
				selectedItem = (selectedItem + 1) % items.size();
				break;

			case TCODK_ENTER:
			{
				int i = 0;
				for (auto item : items)
				{
					if (i == selectedItem)
					{
						return item->code;
					}
					i++;
				}
			}
			break;
			default: break;
		}
	}
	return NONE;
}
