#pragma once
#include <cocos2d.h>
#include "Global.h"

class Block : public cocos2d::Node
{
public:
	GridPosition m_gridPosition;

	static Block* createBlock(BlockType type, GridPosition gridPosition);
	virtual void onEnter();

	void setActive(bool isActive);
	void blink(int times = 3);
	BlockType getType() { return m_type; }
	void setType(BlockType type);
	void explode();

private:
	BlockType m_type;
	cocos2d::Sprite* m_backgroundSprite;
	cocos2d::Sprite* m_blockSprite;
};
