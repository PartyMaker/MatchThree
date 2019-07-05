#pragma once
#include <cocos2d.h>
#include "Global.h"
#include "Block.h"

class Grid: public cocos2d::Layer
{
public:
	TLevelConfig m_Config;
	std::vector<std::vector<Block*>> m_blocks;

	static Grid* createGrid(const TLevelConfig& a_Config);
	virtual bool init(const TLevelConfig& a_Config);
	virtual void onEnter();

	void removeAllBoxes();
	void generateRandomBloxes();
	bool areNeighbours(Block* first, Block*second);
	void swapBoxes(Block* first, Block* second);
	bool checkForMatches(Block* block);
	void removeBlockAt(GridPosition pos);
	void fillBlanks();
	void resolveMatchesForBlocks(std::vector<Block*> blocks);
	Block* getBlockForPosition(cocos2d::Vec2 position);
	std::vector<Block*> findMatches(Block* block, bool isRoot = true);
	std::vector<GridMove> findAvailableMoves();
	bool isBisy() { return m_bisy; };

private:
	std::vector<Block*> m_matches;
	bool m_bisy;
};
