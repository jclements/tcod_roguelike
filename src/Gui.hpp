#ifndef GUI_HPP
#define GUI_HPP
#include <list>

class Menu {
public:
	enum MenuItemCode {
		NONE,
		NEW_GAME,
		CONTINUE,
		EXIT,
		CONSTITUTION,
		STRENGTH,
		AGILITY
	};
	enum DisplayMode {
		MAIN,
		PAUSE
	};
	~Menu();
	void clear();
	void addItem(MenuItemCode code, const char *label);
	MenuItemCode pick(DisplayMode mode=MAIN);

protected:
	struct MenuItem {
		MenuItemCode code;
		const char *label;
	};
	std::list<MenuItem *> items;
};

class Gui : public Persistent {
public:
	Gui();
	~Gui();
	Menu menu;
	void render();
	void message(const TCODColor &col, std::string text, ...);
	void load(TCODZip &zip);
	void save(TCODZip &zip);
	void clear();

protected:
	TCODConsole *con;
	struct Message {
		std::string text;
		TCODColor col;
		Message(std::string &text, const TCODColor &col);
		~Message();
	};
	std::list<Message *> log;

	void renderBar(int x, int y, int width, const char *name,
		float value, float maxValue, const TCODColor &barColor,
		const TCODColor &backColor);

	void renderMouseLook();

};

#endif // !GUI_HPP
