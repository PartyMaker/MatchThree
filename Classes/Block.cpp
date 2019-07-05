#include "Block.h"

USING_NS_CC;

//--------------------------------------------------------------------------------------------------------
Block* Block::createBlock(BlockType type, GridPosition gridPosition)
{
	Block* block = new Block();

	if (block->init())
	{
		block->autorelease();
		block->m_type = type;
		block->m_gridPosition = gridPosition;
		return block;
	}
	return nullptr;
}
//--------------------------------------------------------------------------------------------------------
void Block::setType(BlockType type)
{
	this->m_type = type;

	if (m_blockSprite != nullptr && type != BlockType::NONE)
	{
		m_blockSprite->setSpriteFrame(gBlockTypeToFrameName.at(type));
		m_blockSprite->setScale(0.3f);
	}
}
//--------------------------------------------------------------------------------------------------------
void Block::onEnter()
{
	Node::onEnter();

	setContentSize(gBlockSize);
	setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	
	m_backgroundSprite = Sprite::createWithSpriteFrameName("popup_base.png");
	m_backgroundSprite->setScale(0.3f);
	m_backgroundSprite->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	m_backgroundSprite->setPosition(gBlockSize * 0.5f);
	addChild(m_backgroundSprite, 0);

	m_blockSprite = Sprite::createWithSpriteFrameName(gBlockTypeToFrameName.at(m_type));
	m_blockSprite->setScale(0.3f);
	m_blockSprite->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	m_blockSprite->setPosition(gBlockSize * 0.5f);
	addChild(m_blockSprite, 1);
}
//--------------------------------------------------------------------------------------------------------
void Block::setActive(bool isActive)
{
	if (isActive)
	{
		m_backgroundSprite->setSpriteFrame("popup_base1.png");
		m_backgroundSprite->setScale(0.3f);
	}
	else
	{
		m_backgroundSprite->setSpriteFrame("popup_base.png");
		m_backgroundSprite->setScale(0.3f);
	}
}
//--------------------------------------------------------------------------------------------------------
void Block::blink(int times)
{
	m_backgroundSprite->runAction(Repeat::create(Sequence::create(
											  TintTo::create(0.1f, Color3B::BLACK),
											  TintTo::create(0.1f, Color3B::WHITE)
											  , NULL), times));
}
//--------------------------------------------------------------------------------------------------------
void Block::explode()
{
	auto explodeParticles = ParticleSystemQuad::create("explode.plist");
	explodeParticles->setAutoRemoveOnFinish(true);
	explodeParticles->setPosition(getPosition());
	getParent()->addChild(explodeParticles, 10);
}
//--------------------------------------------------------------------------------------------------------