#pragma once
#include"gameNode.h"

struct aStarlist
{
	int x;
	int y;

	int F;
	int G;
	int H;

	aStarlist* parent;
};

struct walk
{
	int x, y;
};

class astar
{
private:
	tagTile tmp_tile[TILE_COUNT_X][TILE_COUNT_Y];

	vector<aStarlist*> _openList;
	vector<aStarlist*> _closeList;
	vector<walk>v_walk;
public:
	astar();
	~astar();
	void init(gameScene* _GameScene);
	void release();
	void update();
	void astarStart(int startX, int startY, int endX, int endY);
	
	vector<walk> getMoveRoute()
	{
		return v_walk;
	}

	void popBack_moveRoute()
	{
		v_walk.pop_back();
	}
	void clear_moveRoute()
	{
		v_walk.clear();
	}
};

