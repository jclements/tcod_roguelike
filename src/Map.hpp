#ifndef MAP_HPP
#define MAP_HPP

struct Tile {
	bool explored;
	unsigned int scent;	// amount of player scent on this tile
	Tile() : explored(false),scent(0) {}
};

class Map : public Persistent {
public :
	int width,height;
	unsigned int currentScentValue;

	Map(int width, int height);
	~Map();
	bool isWall(int x, int y) const;
	bool canWalk(int x, int y) const;
	bool isInFov(int x, int y) const;
	bool isExplored(int x, int y) const;
	void computeFov();
	void render() const;
	void addMonster(int x, int y);
	void addItem(int x, int y);
	void init(bool withActors);
	void load(TCODZip &zip);
	void save(TCODZip &zip);
	unsigned int getScent(int x, int y) const;

protected :
	Tile *tiles;
	TCODMap *map;
	int seed;
	TCODRandom *rng;
	friend class BspListener;
	void createRoom(bool first, int x1, int y1, int x2, int y2, bool withActors);
	void dig(int x1, int y1, int x2, int y2);
	void setWall(int x, int y);
};

#endif
