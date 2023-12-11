#include "stdafx.h"
#include "astar.h"
#include"gameScene.h"

astar::astar()
{
}


astar::~astar()
{
}

void astar::init(gameScene* _GameScene)
{
	for (int i = 0; i < TILE_COUNT_X; i++)
	{
		for (int j = 0; j < TILE_COUNT_Y; j++)
		{
			tmp_tile[i][j] = _GameScene->getTile(j, i);
		}
	}
}

void astar::release()
{
}

void astar::update()
{
	
}

void astar::astarStart(int startX, int startY, int endX, int endY)
{
	_openList.clear();
	_closeList.clear();	

	aStarlist* __aStarlist = new aStarlist;
	__aStarlist->x = startX;
	__aStarlist->y = startY;
	__aStarlist->G = 0;//ó�����̴ϱ� g�� 0
	__aStarlist->H = abs(startX - endX) + abs(startY - endY);
	__aStarlist->F = __aStarlist->G + __aStarlist->H;
	__aStarlist->parent = NULL;
	_closeList.push_back(__aStarlist);
	int lastindex = 0;
	while (1)
	{
		if (_closeList[lastindex]->x == endX&&_closeList[lastindex]->y == endY)
		{
			v_walk.clear();
			aStarlist* close = _closeList[lastindex];

			while (close->parent != NULL)
			{
				walk _walk;
				_walk.x = close->x;
				_walk.y = close->y;
				v_walk.push_back(_walk);
				close = close->parent;

			}
			break;
		}


		if (_closeList[lastindex]->x != 0 && _closeList[lastindex]->y != 0)//������
		{
			if (!tmp_tile[_closeList[lastindex]->y][_closeList[lastindex]->x - 1]._obstacle &&
				!tmp_tile[_closeList[lastindex]->y][_closeList[lastindex]->x - 1].EnemyIsOn &&
				tmp_tile[_closeList[lastindex]->y][_closeList[lastindex]->x - 1].terrainLevel - tmp_tile[_closeList[lastindex]->y][_closeList[lastindex]->x].terrainLevel < 2 &&
				!tmp_tile[_closeList[lastindex]->y - 1][_closeList[lastindex]->x]._obstacle &&
				!tmp_tile[_closeList[lastindex]->y - 1][_closeList[lastindex]->x].EnemyIsOn &&
				tmp_tile[_closeList[lastindex]->y - 1][_closeList[lastindex]->x].terrainLevel - tmp_tile[_closeList[lastindex]->y][_closeList[lastindex]->x].terrainLevel < 2 &&
				tmp_tile[_closeList[lastindex]->y][_closeList[lastindex]->x].terrainLevel - tmp_tile[_closeList[lastindex]->y][_closeList[lastindex]->x - 1].terrainLevel < 2 && 
				tmp_tile[_closeList[lastindex]->y][_closeList[lastindex]->x].terrainLevel - tmp_tile[_closeList[lastindex]->y-1][_closeList[lastindex]->x].terrainLevel < 2)
			{
				if (!tmp_tile[_closeList[lastindex]->y - 1][_closeList[lastindex]->x - 1]._obstacle && 
					!tmp_tile[_closeList[lastindex]->y - 1][_closeList[lastindex]->x - 1].EnemyIsOn &&
					tmp_tile[_closeList[lastindex]->y - 1][_closeList[lastindex]->x - 1].terrainLevel - tmp_tile[_closeList[lastindex]->y][_closeList[lastindex]->x].terrainLevel < 2 && 
					tmp_tile[_closeList[lastindex]->y][_closeList[lastindex]->x].terrainLevel - tmp_tile[_closeList[lastindex]->y - 1][_closeList[lastindex]->x - 1].terrainLevel < 2)
				{
					aStarlist* _aStarlist = new aStarlist;
					tmp_tile[_closeList[lastindex]->y - 1][_closeList[lastindex]->x - 1]._obstacle = true;
					_aStarlist->x = _closeList[lastindex]->x - 1;
					_aStarlist->y = _closeList[lastindex]->y - 1;
					_aStarlist->G = _closeList[lastindex]->G + 10;
					_aStarlist->H = (abs(_aStarlist->x - endX) + abs(_aStarlist->y - endY)) * 10;
					_aStarlist->F = _aStarlist->G + _aStarlist->H;
					_aStarlist->parent = _closeList[lastindex];
					_openList.push_back(_aStarlist);
				}
			}
		}

		if (_closeList[lastindex]->x != 0 && _closeList[lastindex]->y != TILE_COUNT_Y - 1)//���ʾƷ�
		{
			if (!tmp_tile[_closeList[lastindex]->y][_closeList[lastindex]->x - 1]._obstacle &&
				!tmp_tile[_closeList[lastindex]->y][_closeList[lastindex]->x - 1].EnemyIsOn &&
				tmp_tile[_closeList[lastindex]->y][_closeList[lastindex]->x - 1].terrainLevel - tmp_tile[_closeList[lastindex]->y][_closeList[lastindex]->x].terrainLevel < 2 &&
				!tmp_tile[_closeList[lastindex]->y + 1][_closeList[lastindex]->x]._obstacle && 
				!tmp_tile[_closeList[lastindex]->y + 1][_closeList[lastindex]->x].EnemyIsOn &&
				tmp_tile[_closeList[lastindex]->y + 1][_closeList[lastindex]->x].terrainLevel - tmp_tile[_closeList[lastindex]->y][_closeList[lastindex]->x].terrainLevel < 2 &&
				tmp_tile[_closeList[lastindex]->y][_closeList[lastindex]->x].terrainLevel - tmp_tile[_closeList[lastindex]->y][_closeList[lastindex]->x-1].terrainLevel < 2 && 
				tmp_tile[_closeList[lastindex]->y][_closeList[lastindex]->x].terrainLevel - tmp_tile[_closeList[lastindex]->y + 1][_closeList[lastindex]->x].terrainLevel < 2)
			{
				if (!tmp_tile[_closeList[lastindex]->y + 1][_closeList[lastindex]->x - 1]._obstacle && 
					!tmp_tile[_closeList[lastindex]->y + 1][_closeList[lastindex]->x - 1].EnemyIsOn &&
					tmp_tile[_closeList[lastindex]->y + 1][_closeList[lastindex]->x - 1].terrainLevel - tmp_tile[_closeList[lastindex]->y][_closeList[lastindex]->x].terrainLevel < 2 &&
					tmp_tile[_closeList[lastindex]->y][_closeList[lastindex]->x].terrainLevel - tmp_tile[_closeList[lastindex]->y + 1][_closeList[lastindex]->x - 1].terrainLevel < 2)
				{
					aStarlist* _aStarlist = new aStarlist;
					tmp_tile[_closeList[lastindex]->y + 1][_closeList[lastindex]->x - 1]._obstacle = true;
					_aStarlist->x = _closeList[lastindex]->x - 1;
					_aStarlist->y = _closeList[lastindex]->y + 1;
					_aStarlist->G = _closeList[lastindex]->G + 10;
					_aStarlist->H = (abs(_aStarlist->x - endX) + abs(_aStarlist->y - endY)) * 10;
					_aStarlist->F = _aStarlist->G + _aStarlist->H;
					_aStarlist->parent = _closeList[lastindex];
					_openList.push_back(_aStarlist);
				}
			}
		}

		if (_closeList[lastindex]->x != TILE_COUNT_X - 1 && _closeList[lastindex]->y != TILE_COUNT_Y - 1)//�����ʾƷ�
		{
			if (!tmp_tile[_closeList[lastindex]->y][_closeList[lastindex]->x + 1]._obstacle &&
				!tmp_tile[_closeList[lastindex]->y][_closeList[lastindex]->x + 1].EnemyIsOn &&
				tmp_tile[_closeList[lastindex]->y][_closeList[lastindex]->x + 1].terrainLevel - tmp_tile[_closeList[lastindex]->y][_closeList[lastindex]->x].terrainLevel < 2 &&
				!tmp_tile[_closeList[lastindex]->y + 1][_closeList[lastindex]->x]._obstacle &&
				!tmp_tile[_closeList[lastindex]->y + 1][_closeList[lastindex]->x].EnemyIsOn &&
				tmp_tile[_closeList[lastindex]->y + 1][_closeList[lastindex]->x].terrainLevel - tmp_tile[_closeList[lastindex]->y][_closeList[lastindex]->x].terrainLevel < 2 &&
				tmp_tile[_closeList[lastindex]->y][_closeList[lastindex]->x].terrainLevel - tmp_tile[_closeList[lastindex]->y][_closeList[lastindex]->x + 1].terrainLevel < 2 &&
				tmp_tile[_closeList[lastindex]->y][_closeList[lastindex]->x].terrainLevel - tmp_tile[_closeList[lastindex]->y + 1][_closeList[lastindex]->x].terrainLevel < 2)
			{
				if (!tmp_tile[_closeList[lastindex]->y + 1][_closeList[lastindex]->x + 1]._obstacle && 
					!tmp_tile[_closeList[lastindex]->y + 1][_closeList[lastindex]->x + 1].EnemyIsOn &&
					tmp_tile[_closeList[lastindex]->y + 1][_closeList[lastindex]->x + 1].terrainLevel - tmp_tile[_closeList[lastindex]->y][_closeList[lastindex]->x].terrainLevel < 2 &&
					tmp_tile[_closeList[lastindex]->y][_closeList[lastindex]->x].terrainLevel - tmp_tile[_closeList[lastindex]->y + 1][_closeList[lastindex]->x + 1].terrainLevel < 2)
				{
					aStarlist* _aStarlist = new aStarlist;
					tmp_tile[_closeList[lastindex]->y + 1][_closeList[lastindex]->x + 1]._obstacle = true;
					_aStarlist->x = _closeList[lastindex]->x + 1;
					_aStarlist->y = _closeList[lastindex]->y + 1;
					_aStarlist->G = _closeList[lastindex]->G + 10;
					_aStarlist->H = (abs(_aStarlist->x - endX) + abs(_aStarlist->y - endY)) * 10;
					_aStarlist->F = _aStarlist->G + _aStarlist->H;
					_aStarlist->parent = _closeList[lastindex];
					_openList.push_back(_aStarlist);
				}
			}
		}
		if (_closeList[lastindex]->x != TILE_COUNT_X - 1 && _closeList[lastindex]->y != 0)//��������
		{
			if (!tmp_tile[_closeList[lastindex]->y][_closeList[lastindex]->x + 1]._obstacle &&
				!tmp_tile[_closeList[lastindex]->y][_closeList[lastindex]->x + 1].EnemyIsOn &&
				tmp_tile[_closeList[lastindex]->y][_closeList[lastindex]->x + 1].terrainLevel - tmp_tile[_closeList[lastindex]->y][_closeList[lastindex]->x].terrainLevel < 2 &&
				!tmp_tile[_closeList[lastindex]->y - 1][_closeList[lastindex]->x]._obstacle &&
				!tmp_tile[_closeList[lastindex]->y - 1][_closeList[lastindex]->x].EnemyIsOn &&
				tmp_tile[_closeList[lastindex]->y - 1][_closeList[lastindex]->x].terrainLevel - tmp_tile[_closeList[lastindex]->y][_closeList[lastindex]->x].terrainLevel < 2 && 
				tmp_tile[_closeList[lastindex]->y][_closeList[lastindex]->x].terrainLevel - tmp_tile[_closeList[lastindex]->y][_closeList[lastindex]->x + 1].terrainLevel < 2 && 
				tmp_tile[_closeList[lastindex]->y][_closeList[lastindex]->x].terrainLevel - tmp_tile[_closeList[lastindex]->y - 1][_closeList[lastindex]->x].terrainLevel < 2)
			{
				if (!tmp_tile[_closeList[lastindex]->y - 1][_closeList[lastindex]->x + 1]._obstacle &&
					!tmp_tile[_closeList[lastindex]->y - 1][_closeList[lastindex]->x + 1].EnemyIsOn &&
					tmp_tile[_closeList[lastindex]->y - 1][_closeList[lastindex]->x + 1].terrainLevel - tmp_tile[_closeList[lastindex]->y][_closeList[lastindex]->x].terrainLevel < 2 &&
					tmp_tile[_closeList[lastindex]->y][_closeList[lastindex]->x].terrainLevel - tmp_tile[_closeList[lastindex]->y - 1][_closeList[lastindex]->x + 1].terrainLevel < 2)
				{
					aStarlist* _aStarlist = new aStarlist;
					tmp_tile[_closeList[lastindex]->y - 1][_closeList[lastindex]->x + 1]._obstacle = true;
					_aStarlist->x = _closeList[lastindex]->x + 1;
					_aStarlist->y = _closeList[lastindex]->y - 1;
					_aStarlist->G = _closeList[lastindex]->G + 10;
					_aStarlist->H = (abs(_aStarlist->x - endX) + abs(_aStarlist->y - endY)) * 10;
					_aStarlist->F = _aStarlist->G + _aStarlist->H;
					_aStarlist->parent = _closeList[lastindex];
					_openList.push_back(_aStarlist);
				}
			}
		}
		if (_closeList[lastindex]->x != 0)
		{
			if (!tmp_tile[_closeList[lastindex]->y][_closeList[lastindex]->x - 1]._obstacle &&
				!tmp_tile[_closeList[lastindex]->y][_closeList[lastindex]->x - 1].EnemyIsOn &&
				tmp_tile[_closeList[lastindex]->y][_closeList[lastindex]->x - 1].terrainLevel - tmp_tile[_closeList[lastindex]->y][_closeList[lastindex]->x].terrainLevel < 2 &&
				tmp_tile[_closeList[lastindex]->y][_closeList[lastindex]->x].terrainLevel - tmp_tile[_closeList[lastindex]->y][_closeList[lastindex]->x - 1].terrainLevel < 2)
			{
				aStarlist* _aStarlist = new aStarlist;
				tmp_tile[_closeList[lastindex]->y][_closeList[lastindex]->x - 1]._obstacle = true;
				_aStarlist->x = _closeList[lastindex]->x - 1;
				_aStarlist->y = _closeList[lastindex]->y;
				_aStarlist->G = _closeList[lastindex]->G + 10;
				_aStarlist->H = (abs(_aStarlist->x - endX) + abs(_aStarlist->y - endY)) * 10;
				_aStarlist->F = _aStarlist->G + _aStarlist->H;
				_aStarlist->parent = _closeList[lastindex];
				_openList.push_back(_aStarlist);
			}
		}
		if (_closeList[lastindex]->x != TILE_COUNT_X - 1)
		{
			if (!tmp_tile[_closeList[lastindex]->y][_closeList[lastindex]->x + 1]._obstacle && 
				!tmp_tile[_closeList[lastindex]->y][_closeList[lastindex]->x + 1].EnemyIsOn &&
				tmp_tile[_closeList[lastindex]->y][_closeList[lastindex]->x + 1].terrainLevel - tmp_tile[_closeList[lastindex]->y][_closeList[lastindex]->x].terrainLevel < 2 && 
				tmp_tile[_closeList[lastindex]->y][_closeList[lastindex]->x].terrainLevel - tmp_tile[_closeList[lastindex]->y][_closeList[lastindex]->x + 1].terrainLevel < 2)
			{
				aStarlist* _aStarlist = new aStarlist;
				tmp_tile[_closeList[lastindex]->y][_closeList[lastindex]->x + 1]._obstacle = true;

				_aStarlist->x = _closeList[lastindex]->x + 1;
				_aStarlist->y = _closeList[lastindex]->y;
				_aStarlist->G = _closeList[lastindex]->G + 10;
				_aStarlist->H = (abs(_aStarlist->x - endX) + abs(_aStarlist->y - endY)) * 10;
				_aStarlist->F = _aStarlist->G + _aStarlist->H;
				_aStarlist->parent = _closeList[lastindex];
				_openList.push_back(_aStarlist);
			}
		}
		if (_closeList[lastindex]->y != 0)
		{
			if (!tmp_tile[_closeList[lastindex]->y - 1][_closeList[lastindex]->x]._obstacle &&
				!tmp_tile[_closeList[lastindex]->y - 1][_closeList[lastindex]->x].EnemyIsOn &&
				tmp_tile[_closeList[lastindex]->y - 1][_closeList[lastindex]->x].terrainLevel - tmp_tile[_closeList[lastindex]->y][_closeList[lastindex]->x].terrainLevel < 2 && 
				tmp_tile[_closeList[lastindex]->y][_closeList[lastindex]->x].terrainLevel - tmp_tile[_closeList[lastindex]->y - 1][_closeList[lastindex]->x].terrainLevel < 2)
			{
				aStarlist* _aStarlist = new aStarlist;
				tmp_tile[_closeList[lastindex]->y - 1][_closeList[lastindex]->x]._obstacle = true;

				_aStarlist->x = _closeList[lastindex]->x;
				_aStarlist->y = _closeList[lastindex]->y - 1;
				_aStarlist->G = _closeList[lastindex]->G + 10;
				_aStarlist->H = (abs(_aStarlist->x - endX) + abs(_aStarlist->y - endY)) * 10;
				_aStarlist->F = _aStarlist->G + _aStarlist->H;
				_aStarlist->parent = _closeList[lastindex];
				_openList.push_back(_aStarlist);
			}
		}
		if (_closeList[lastindex]->y != TILE_COUNT_Y - 1)
		{
			if (!tmp_tile[_closeList[lastindex]->y + 1][_closeList[lastindex]->x]._obstacle &&
				!tmp_tile[_closeList[lastindex]->y + 1][_closeList[lastindex]->x].EnemyIsOn &&
				tmp_tile[_closeList[lastindex]->y + 1][_closeList[lastindex]->x].terrainLevel - tmp_tile[_closeList[lastindex]->y][_closeList[lastindex]->x].terrainLevel < 2 &&
				tmp_tile[_closeList[lastindex]->y][_closeList[lastindex]->x].terrainLevel - tmp_tile[_closeList[lastindex]->y + 1][_closeList[lastindex]->x].terrainLevel < 2)
			{
				aStarlist* _aStarlist = new aStarlist;
				tmp_tile[_closeList[lastindex]->y + 1][_closeList[lastindex]->x]._obstacle = true;

				_aStarlist->x = _closeList[lastindex]->x;
				_aStarlist->y = _closeList[lastindex]->y + 1;
				_aStarlist->G = _closeList[lastindex]->G + 10;
				_aStarlist->H = (abs(_aStarlist->x - endX) + abs(_aStarlist->y - endY)) * 10;
				_aStarlist->F = _aStarlist->G + _aStarlist->H;
				_aStarlist->parent = _closeList[lastindex];
				_openList.push_back(_aStarlist);
			}
		}

		if (_openList.size() == 0)//���±��� �ٸ����ִٸ�
		{
			break;
		}
		if (_openList.size()>100)//���±��� �ٸ����ִٸ�
		{
			break;
		}
		int deletnum = 0;//���¸���Ʈ �߾���� ���� �������� �Ǵ��ϰ� �׹迭���� �־��ٰ���
		int min = 50000;//����ū f���� �������� ����
		for (int i = 0; i < _openList.size(); i++)
		{
			if (_openList[i]->F < min)
			{
				min = _openList[i]->F;
				deletnum = i;
			}
		}

		_closeList.push_back(_openList[deletnum]);
		_openList.erase(_openList.begin() + deletnum);

		lastindex++;//�߰��ɶ����� ������ ���˱�����?
	}
}


