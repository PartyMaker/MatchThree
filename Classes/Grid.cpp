#include "Grid.h"
#include "SimpleAudioEngine.h"
#include <bitset>

USING_NS_CC;
//--------------------------------------------------------------------------------------------------------
Grid* Grid::createGrid(const TLevelConfig& a_Config)
{
	Grid* grid = new Grid();
	
	if (grid->init(a_Config))
	{
		grid->autorelease();
		grid->setContentSize(Size(a_Config.rows * gBlockSize.width, a_Config.cols * gBlockSize.height));
		grid->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		return grid;
	}
	return nullptr;
};
//--------------------------------------------------------------------------------------------------------
bool Grid::init(const TLevelConfig& a_Config)
{
	if (!Layer::init())
		return false;
	m_Config = a_Config;
	m_blocks.resize(m_Config.rows);
	for (size_t i = 0; i < m_blocks.size(); ++i)
		m_blocks[i].resize(m_Config.rows);
	return true;
}
//--------------------------------------------------------------------------------------------------------
void Grid::onEnter()
{
	Layer::onEnter();
}
//--------------------------------------------------------------------------------------------------------
void Grid::generateRandomBloxes()
{
	for (size_t col = 0; col < m_Config.cols; ++col)
	{
		std::bitset<16> bitset(m_Config.matrix[col]);
		for (size_t row = 0; row < m_Config.rows; ++row)
		{
			if (!bitset[row])
				continue;
			Block* block = Block::createBlock((BlockType)random((int)BlockType::APPLE, (int)BlockType::STAR), { (int)row, (int)col });

			// Make sure there are no initial groups of matches
			while (checkForMatches(block) == true)
				block->setType((BlockType)random((int)BlockType::APPLE, (int)BlockType::STAR));

			block->setPosition(row * gBlockSize.width + gBlockSize.width * 0.5f,
				col * gBlockSize.height + gBlockSize.height * 0.5f);

			addChild(block, 1);
			m_blocks[row][col] = block;
		}
	}
}
//--------------------------------------------------------------------------------------------------------
void Grid::removeAllBoxes()
{
	stopAllActions();
	m_bisy = false;

	for (size_t col = 0; col < m_Config.cols; ++col)
		for (size_t row = 0; row < m_Config.rows; ++row)
		{
			if (m_blocks[row][col] != nullptr)
			{
				removeChild(m_blocks[row][col]);
				m_blocks[row][col] = nullptr;
			}
		}
}
//--------------------------------------------------------------------------------------------------------
Block* Grid::getBlockForPosition(cocos2d::Vec2 position)
{
	unsigned int row = (unsigned int)ceil((position.x - getPosition().x) / gBlockSize.width) - 1;
	unsigned int col = (unsigned int)ceil((position.y - getPosition().y) / gBlockSize.height) - 1;

	CCASSERT((col < m_Config.cols && row < m_Config.rows), "Incorrect row/col. Index out of range");

	if (m_blocks[row][col])
		return m_blocks[row][col];

	return nullptr;
}
//--------------------------------------------------------------------------------------------------------
void Grid::swapBoxes(Block * first, Block * second)
{
	if (!first || !second)
		return;

	if (!areNeighbours(first, second))
	{
		CCLOGERROR("Blocks [%d, %d] and [%d, %d] are not neighbours!",
			first->m_gridPosition.row,
			first->m_gridPosition.col,
			second->m_gridPosition.row,
			second->m_gridPosition.col
		);
		return;
	}

	m_blocks[first->m_gridPosition.row][first->m_gridPosition.col]   = second;
	m_blocks[second->m_gridPosition.row][second->m_gridPosition.col] = first;

	// Swap bloxes indexes
	GridPosition temp = first->m_gridPosition;
	first->m_gridPosition = second->m_gridPosition;
	second->m_gridPosition = temp;
}
//--------------------------------------------------------------------------------------------------------
void Grid::removeBlockAt(GridPosition pos)
{
	Block* block = m_blocks[pos.row][pos.col];
	if (block)
	{
		m_blocks[pos.row][pos.col] = nullptr;
		block->explode();
		block->setType(BlockType::NONE);
		removeChild(block, true);
	}
}
//--------------------------------------------------------------------------------------------------------
bool Grid::areNeighbours(Block * first, Block * second)
{
	if (!first || !second)
		return false;

	int x = abs(first->m_gridPosition.row - second->m_gridPosition.row);
	int y = abs(first->m_gridPosition.col - second->m_gridPosition.col);

	return (x == 1 && y == 0) || (x == 0 && y == 1);
}
//--------------------------------------------------------------------------------------------------------
bool Grid::checkForMatches(Block * block)
{
	if (!block)
		return false;

	GridPosition gp = block->m_gridPosition;

	// Check horizontal
	int matches = 1;
	int row = gp.row;
	while (--row > -1 && m_blocks[row][gp.col] && m_blocks[row][gp.col]->getType() == block->getType()) // Check left
		matches++;

	row = gp.row;
	while (++row < (int)m_Config.rows && m_blocks[row][gp.col] && m_blocks[row][gp.col]->getType() == block->getType()) // Check right
		matches++;

	if (matches >= MAX_MATCHES)
		return true;

	// Check vertical
	matches = 1;
	int col = gp.col;
	while (--col > -1 && m_blocks[gp.row][col] && m_blocks[gp.row][col]->getType() == block->getType()) // Check down
		matches++;

	col = gp.col;
	while (++col < (int)m_Config.cols && m_blocks[gp.row][col] && m_blocks[gp.row][col]->getType() == block->getType()) // Check up
		matches++;

	return (matches >= MAX_MATCHES);
}
//--------------------------------------------------------------------------------------------------------
std::vector<Block*> Grid::findMatches(Block * block, bool isRoot)
{
	if (isRoot)
		m_matches.clear();

	m_matches.push_back(block);

	GridPosition pos = block->m_gridPosition;

	Block* leftBlock = (pos.row > 0) ? m_blocks[pos.row - 1][pos.col] : nullptr;
	Block* rightBlock = (pos.row < (int)m_Config.rows - 1) ? m_blocks[pos.row + 1][pos.col] : nullptr;
	Block * topBlock = (pos.col < (int)m_Config.cols - 1) ? m_blocks[pos.row][pos.col + 1] : nullptr;
	Block * bottomBlock = (pos.col > 0) ? m_blocks[pos.row][pos.col - 1] : nullptr;

	// Left
	if (leftBlock && leftBlock->getType() == block->getType())
	{
		if (std::find(m_matches.begin(), m_matches.end(), leftBlock) == m_matches.end())
			findMatches(leftBlock, false);
	}

	// Right
	if (rightBlock && rightBlock->getType() == block->getType())
	{
		if (std::find(m_matches.begin(), m_matches.end(), rightBlock) == m_matches.end())
			findMatches(rightBlock, false);
	}

	// Top
	if (topBlock && topBlock->getType() == block->getType())
	{
		if (std::find(m_matches.begin(), m_matches.end(), topBlock) == m_matches.end())
			findMatches(topBlock, false);
	}

	// Bottom
	if (bottomBlock && bottomBlock->getType() == block->getType())
	{
		if (std::find(m_matches.begin(), m_matches.end(), bottomBlock) == m_matches.end())
			findMatches(bottomBlock, false);
	}

	return m_matches;
}
//--------------------------------------------------------------------------------------------------------
std::vector<GridMove> Grid::findAvailableMoves() 
{
	std::vector<GridMove> availableMoves;
	Block* otherBlock;

	for (size_t row = 0; row < m_Config.rows; ++row)
	{
		for (size_t col = 0; col < m_Config.cols; ++col)
		{
			Block* block = m_blocks[row][col];
			if (block == nullptr)
				continue;

			// Swap right and check if successfull, than swap them back
			if (row < m_Config.rows - 1 && m_blocks[row + 1][col])
			{
				otherBlock = m_blocks[row + 1][col];
				swapBoxes(block, otherBlock);
				if (checkForMatches(block) || checkForMatches(otherBlock))
					availableMoves.push_back({ otherBlock->m_gridPosition, block->m_gridPosition });
				swapBoxes(otherBlock, block);
			}

			// Swap up and check
			if (col < m_Config.cols - 1 && m_blocks[row][col + 1])
			{
				otherBlock = m_blocks[row][col + 1];
				swapBoxes(block, otherBlock);
				if (checkForMatches(block) || checkForMatches(otherBlock))
					availableMoves.push_back({ otherBlock->m_gridPosition, block->m_gridPosition });
				swapBoxes(otherBlock, block);
			}
		}
	}

	return availableMoves;
}
//--------------------------------------------------------------------------------------------------------
void Grid::fillBlanks()
{
	m_bisy = true;
	std::vector<Block*> addedBlocks;
	for (size_t row = 0; row < m_Config.cols; ++row)
	{
		int blanks = 0; // deleted during match
		int numMoves = 0;
		int numConstEmptyBox = 0; // static
		// Find blank spots for the column
		for (size_t col = 0; col < m_Config.rows; ++col)
		{
			std::bitset<16> bitset(m_Config.matrix[col]);
			
			if (!bitset[row])
			{
				if(blanks)
					++numConstEmptyBox;
				continue;
			}
			if (m_blocks[row][col] == nullptr)
			{
				++blanks;
			}
			else if (blanks > 0) // Move blocks above the blank down
			{
				if(numConstEmptyBox)
					++numMoves;
				int new_col = col - blanks - numConstEmptyBox;
				m_blocks[row][col]->runAction(EaseBounceOut::create(
					MoveTo::create(0.7f,
						Vec2(m_blocks[row][col]->getPositionX(),
							new_col * gBlockSize.height + gBlockSize.height * 0.5f)
					)));
				m_blocks[row][new_col] = m_blocks[row][col];
				m_blocks[row][col] = nullptr;
				m_blocks[row][new_col]->m_gridPosition = { (int)row, (int)new_col };

				addedBlocks.push_back(m_blocks[row][new_col]);
				
				if (numMoves == blanks)
					numConstEmptyBox = 0;
			}
		}

		// Add new blocks to fill the missing ones
		while (blanks--) 
		{
			int new_col = m_Config.cols - blanks - 1;

			std::bitset<16> bitset(m_Config.matrix[new_col]);
			if (!bitset[row])
				continue;

			Block* block = Block::createBlock((BlockType)random((int)BlockType::APPLE, (int)BlockType::STAR), { (int)row, (int)new_col });

			Vec2 newPosition = Vec2(row * gBlockSize.width + gBlockSize.width * 0.5f,
				new_col * gBlockSize.height + gBlockSize.height * 0.5f);

			block->setPositionX(newPosition.x);
			block->setPositionY(getContentSize().height + (m_Config.cols - blanks) * gBlockSize.height * 0.5f);
			block->m_gridPosition = { (int)row, (int)new_col };
			block->runAction(EaseBounceOut::create(MoveTo::create(0.7f, newPosition)));

			addChild(block, 1);
			m_blocks[row][new_col] = block;
			addedBlocks.push_back(block);
		}

	}

	runAction(Sequence::create(
		DelayTime::create(0.75f),
		CallFunc::create(CC_CALLBACK_0(Grid::resolveMatchesForBlocks, this, addedBlocks)),
		NULL));
}
//--------------------------------------------------------------------------------------------------------
void Grid::resolveMatchesForBlocks(std::vector<Block*> blocks)
{
	bool hasMatches = false;
	std::vector<Block*> blocksToRemove;

	for (size_t i = 0; i < blocks.size(); ++i)
	{
		Block* block = blocks.at(i);

		if (block && block->getType() != BlockType::NONE && checkForMatches(block))
		{
			hasMatches = true;
			for (auto match : findMatches(block))
				if (std::find(blocksToRemove.begin(), blocksToRemove.end(), match) == blocksToRemove.end())
					blocksToRemove.push_back(match);
		}
	}

	if (hasMatches)
	{
		EventCustom hasMatchesEvent(EVENT_GRID_HAS_MATCHES);
		EventMatchesData em;
		em.matches = (int)blocksToRemove.size();
		hasMatchesEvent.setUserData((void*)& em);
		_eventDispatcher->dispatchEvent(&hasMatchesEvent);

		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("pop.wav");
		for (auto block : blocksToRemove)
			removeBlockAt(block->m_gridPosition);

		fillBlanks();
	}
	else
	{
		EventCustom gridReadyEvent(EVENT_GRID_READY);
		_eventDispatcher->dispatchEvent(&gridReadyEvent);

		m_bisy = false;
	}
}
//--------------------------------------------------------------------------------------------------------