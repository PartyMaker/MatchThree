#include "GameLayer.h"
#include "SimpleAudioEngine.h"
#include "Global.h"
#include "LevelLayer.h"
#include "external/json/writer.h"
#include "external/json/stringbuffer.h"
#include <iostream>
#include <fstream>

USING_NS_CC;
//--------------------------------------------------------------------------------------------------------
Scene* GameLayer::createScene(const std::vector<TLevelConfig>& a_config, size_t index)
{
	auto scene     = Scene::create();
	auto gameScene = new GameLayer();

	gameScene->autorelease();

	if (gameScene->init(a_config,  index))
		scene->addChild(gameScene);
	else
		return nullptr;

	return scene;
}
//--------------------------------------------------------------------------------------------------------
bool GameLayer::init(const std::vector<TLevelConfig>& a_config, size_t index)
{
	if (!Layer::init())
		return false;

	m_isBisy = false;
	srand(utils::gettime());
	m_LevelConfig = a_config;
	m_indexCurConfig = index;
	if (index < m_LevelConfig.size())
		m_Config = m_LevelConfig[index];
	else
		return false;

	// Preload assets
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("blocks.plist");
	CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("pop.wav");
	if (CocosDenshion::SimpleAudioEngine::getInstance()->isBackgroundMusicPlaying())
		CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("match.mp3", true);

	// Register event listeners
	auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->onTouchBegan = CC_CALLBACK_2(Grid::onTouchBegan, this);
	touchListener->onTouchMoved = CC_CALLBACK_2(Grid::onTouchMoved, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);

	auto gridListener = EventListenerCustom::create(EVENT_GRID_READY, CC_CALLBACK_1(GameLayer::onGridReady, this));
	_eventDispatcher->addEventListenerWithSceneGraphPriority(gridListener, this);

	auto matchesListener = EventListenerCustom::create(EVENT_GRID_HAS_MATCHES, CC_CALLBACK_1(GameLayer::onGridMatches, this));
	_eventDispatcher->addEventListenerWithSceneGraphPriority(matchesListener, this);

	Vec2 origin = Director::getInstance()->getVisibleOrigin(); 
	Vec2 visibleSize = Director::getInstance()->getVisibleSize();

	// background
	auto background = Sprite::createWithSpriteFrameName("bg.png");
	background->setPosition(origin.x + visibleSize.x / 2, origin.y + visibleSize.y / 2);
	this->addChild(background);

	m_grid = Grid::createGrid(m_Config);
	m_grid->setPosition(visibleSize.x * 0.5f - m_grid->getContentSize().width * 0.5f,
		visibleSize.y * 0.5f - m_grid->getContentSize().height * 0.5f - 30);
	addChild(m_grid, 1);
	m_mainControl.pushBack(m_grid);

	// buttons
	auto buttonHint = cocos2d::ui::Button::create("button_9slice.png", "button_9slice01.png", "button_9slice.png", cocos2d::ui::TextureResType::PLIST);
	buttonHint->setPosition(cocos2d::Vec2(origin.x + visibleSize.x / 2.7, origin.y + visibleSize.y / 1.05));
	buttonHint->addTouchEventListener(CC_CALLBACK_2(GameLayer::showHint, this));
	buttonHint->setTitleText("HINT");
	buttonHint->setScale(0.8f);
	buttonHint->setTitleFontSize(32.0f);
	m_mainControl.pushBack(buttonHint);
	this->addChild(buttonHint);

	auto buttonRestart = cocos2d::ui::Button::create("button_9slice.png", "button_9slice01.png", "button_9slice.png", cocos2d::ui::TextureResType::PLIST);
	buttonRestart->setPosition(cocos2d::Vec2(origin.x + visibleSize.x / 8, origin.y + visibleSize.y / 1.05));
	buttonRestart->addTouchEventListener(CC_CALLBACK_1(GameLayer::newGame, this));
	buttonRestart->setTitleText("RESTART");
	buttonRestart->setScale(0.8f);
	buttonRestart->setTitleFontSize(32.0f);
	m_mainControl.pushBack(buttonRestart);
	this->addChild(buttonRestart);

	auto buttonClose = cocos2d::ui::Button::create("button_9slice.png", "button_9slice01.png", "button_9slice.png", cocos2d::ui::TextureResType::PLIST);
	buttonClose->setPosition(cocos2d::Vec2(origin.x + visibleSize.x / 1.15, origin.y + visibleSize.y / 1.05));
	buttonClose->addTouchEventListener(CC_CALLBACK_2(GameLayer::menuCloseCallback, this));
	buttonClose->setTitleText("BACK");
	buttonClose->setScale(0.8f);
	buttonClose->setTitleFontSize(32.0f);
	this->addChild(buttonClose);
	m_mainControl.pushBack(buttonClose);

	// labels
	m_scoreLabel = Label::createWithSystemFont("", "Marker Felt", 32);
	m_scoreLabel->setColor(Color3B::WHITE);
	m_scoreLabel->enableShadow();
	m_scoreLabel->setAnchorPoint(Vec2::ANCHOR_TOP_RIGHT);
	m_scoreLabel->setPosition(visibleSize.x - 20, visibleSize.y - 90);
	addChild(m_scoreLabel, 2);
	m_mainControl.pushBack(m_scoreLabel);

	char moves_string[512];
	snprintf(moves_string, 512, "Goal: %d/%d", m_moves_left, m_Config.moves);
	m_movesLabel = Label::createWithSystemFont(moves_string, "Marker Felt", 32);
	m_movesLabel->setColor(Color3B::WHITE);
	m_movesLabel->enableShadow();
	m_movesLabel->setAnchorPoint(Vec2::ANCHOR_MIDDLE_TOP);
	m_movesLabel->setPosition(visibleSize.x - 100, visibleSize.y - 140);
	addChild(m_movesLabel, 2);
	m_mainControl.pushBack(m_movesLabel);


	dialogControlSimple();

	newGame(this);

	return true;
}
//--------------------------------------------------------------------------------------------------------
bool GameLayer::onTouchBegan(Touch * touch, Event * event)
{
	Vec2 point = touch->getLocation();

	if (m_isBisy || m_grid->isBisy() || m_grid->getBoundingBox().containsPoint(point) == false)
		return false;

	auto block = m_grid->getBlockForPosition(point);
	if (block == nullptr || block == m_activeBlock)
		return true;

	if (m_activeBlock != nullptr)
	{
		// If boxes are neighbours try to swap them
		if (m_grid->areNeighbours(m_activeBlock, block))
		{
			swapBoxes(block, m_activeBlock);
			runAction(Sequence::create(
				DelayTime::create(0.3f), // Wait for the animation to end
				CallFunc::create(CC_CALLBACK_0(GameLayer::checkForMatches, this, m_activeBlock, block)),
				nullptr));

			m_activeBlock->setActive(false);
			block->setActive(false);
			m_activeBlock = nullptr;
		}
		else // If not set active the selected one
		{
			m_activeBlock->setActive(false);
			m_activeBlock = block;
			m_activeBlock->setActive(true);
		}
	}
	else
	{
		m_activeBlock = block;
		m_activeBlock->setActive(true);
	}

	return true;
}
//--------------------------------------------------------------------------------------------------------
// Detect swipes
void GameLayer::onTouchMoved(cocos2d::Touch * touch, cocos2d::Event * event)
{
	if (m_isBisy || m_grid->isBisy() || m_activeBlock == nullptr)
		return;

	Vec2 touchDistance = touch->getLocation() - touch->getPreviousLocation();
	Block * blockToSwap = nullptr;

	// Right
	if (touchDistance.x >= SWIPE_TRESHOLD && (m_activeBlock->m_gridPosition.row + 1 < (int)m_Config.rows))
		blockToSwap = m_grid->m_blocks[m_activeBlock->m_gridPosition.row + 1][m_activeBlock->m_gridPosition.col];

	// Left
	if (touchDistance.x <= -SWIPE_TRESHOLD && (m_activeBlock->m_gridPosition.row > 0))
		blockToSwap = m_grid->m_blocks[m_activeBlock->m_gridPosition.row - 1][m_activeBlock->m_gridPosition.col];

	// Up
	if (touchDistance.y >= SWIPE_TRESHOLD && (m_activeBlock->m_gridPosition.col + 1 < (int)m_Config.cols))
		blockToSwap = m_grid->m_blocks[m_activeBlock->m_gridPosition.row][m_activeBlock->m_gridPosition.col + 1];

	// Down
	if (touchDistance.y <= -SWIPE_TRESHOLD && (m_activeBlock->m_gridPosition.col > 0))
		blockToSwap = m_grid->m_blocks[m_activeBlock->m_gridPosition.row][m_activeBlock->m_gridPosition.col - 1];

	if (blockToSwap != nullptr)
	{
		swapBoxes(blockToSwap, m_activeBlock);
		runAction(Sequence::create(
			DelayTime::create(0.3f), // Wait for the animation to end
			CallFunc::create(CC_CALLBACK_0(GameLayer::checkForMatches, this, m_activeBlock, blockToSwap)),
			nullptr));

		m_activeBlock->setActive(false);
		m_activeBlock = nullptr;
	}
}
//--------------------------------------------------------------------------------------------------------
void GameLayer::checkForMatches(Block * first, Block * second)
{
	bool firstMatches = m_grid->checkForMatches(first);
	bool secondMatches = m_grid->checkForMatches(second);

	if (firstMatches == false && secondMatches == false)
	{
		// No matches swap them back and return
		swapBoxes(first, second);
		return;
	}

	if (firstMatches)
	{
		resolveMatchesForBlock(first);
	}

	if (secondMatches)
	{
		resolveMatchesForBlock(second);
	}

	if (firstMatches || secondMatches)
	{
		removeMoves(1);
		lockTouches(0.7f);
		m_grid->fillBlanks();
		m_activeBlock = nullptr;

		if (!m_moves_left)
			gameOver();
	}
}
//--------------------------------------------------------------------------------------------------------
void GameLayer::swapBoxes(Block * first, Block * second)
{
	lockTouches(0.3f);
	m_grid->swapBoxes(first, second);
	first->runAction(MoveTo::create(0.3f, second->getPosition()));
	second->runAction(MoveTo::create(0.3f, first->getPosition()));
}
//--------------------------------------------------------------------------------------------------------
void GameLayer::resolveMatchesForBlock(Block * block)
{
	if (block == nullptr)
		return;

	auto matches = m_grid->findMatches(block);
	addScore((int)matches.size());

	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("pop.wav");

	for (auto match : matches)
		m_grid->removeBlockAt(match->m_gridPosition);

}
//--------------------------------------------------------------------------------------------------------
void GameLayer::newGame(cocos2d::Ref * /*ref*/)
{
	m_grid->removeAllBoxes();
	m_grid->generateRandomBloxes();
	m_availableMoves = m_grid->findAvailableMoves();

	m_activeBlock = nullptr;
	addScore(-m_currentScore);
	removeMoves(m_moves_left - m_Config.moves);
	if (m_availableMoves.size() == 0)
	{
		newGame(this);
		if(m_currentScore)
			dialogResult("No steps available! Go to menu?");
	}

}
//--------------------------------------------------------------------------------------------------------
// Select a random move from available ones and show it
void GameLayer::showHint(cocos2d::Ref * ref, cocos2d::ui::Widget::TouchEventType a_type)
{
	if (a_type == cocos2d::ui::Widget::TouchEventType::ENDED)
	{
		if (!m_grid->isBisy() && m_availableMoves.size() > 0)
		{
			GridMove move = m_availableMoves.at(random(0, (int)m_availableMoves.size() - 1));
			m_grid->m_blocks[move.first.row][move.first.col]->blink();
			m_grid->m_blocks[move.second.row][move.second.col]->blink();
		}
	}
}
//--------------------------------------------------------------------------------------------------------
void GameLayer::lockTouches(float time)
{
	unschedule(CC_SCHEDULE_SELECTOR(GameLayer::unlockTouches));
	scheduleOnce(CC_SCHEDULE_SELECTOR(GameLayer::unlockTouches), 0.5f);
	m_isBisy = true;
}
//--------------------------------------------------------------------------------------------------------
void GameLayer::unlockTouches(float dt)
{
	m_isBisy = false;
}
//--------------------------------------------------------------------------------------------------------
void GameLayer::removeMoves(int moves)
{
	m_moves_left -= moves;
	char moves_string[512];
	snprintf(moves_string, 512, "Goal: %d/%d", m_moves_left, m_Config.moves);
	m_movesLabel->setString(moves_string);
}
//--------------------------------------------------------------------------------------------------------
void GameLayer::addScore(int score)
{
	m_currentScore += score;
	char score_string[512];
	snprintf(score_string, 512, "Score: %d/%d", m_currentScore, m_Config.score);
	m_scoreLabel->setString(score_string);
}
//--------------------------------------------------------------------------------------------------------
void GameLayer::onGridReady(cocos2d::EventCustom * event)
{
	m_availableMoves = m_grid->findAvailableMoves();
	if (m_availableMoves.size() == 0)
	{
		gameOver();
		newGame(this);
	}
}
//--------------------------------------------------------------------------------------------------------
void GameLayer::gameOver()
{
	if (m_currentScore >= (int)m_Config.score)
	{
		m_Config.success = true;
		if ((int)m_Config.record < m_currentScore)
			m_Config.record = m_currentScore;
		if (m_indexCurConfig < m_LevelConfig.size())
		{
			m_LevelConfig[m_indexCurConfig] = m_Config;
			if (m_LevelConfig.size() > (m_indexCurConfig+1))
			{
				m_LevelConfig[m_indexCurConfig+1].lock = false;
			}
		}
		else
			CCLOGERROR("Index [%d] of array_config out range!", m_indexCurConfig);

		//save list config
		writeConfig();
		dialogResult("YOU WIN! Go to menu?");
	}
	else
		dialogResult("YOU LOSE! Go to menu?");
}
//--------------------------------------------------------------------------------------------------------
void GameLayer::onGridMatches(cocos2d::EventCustom * event)
{
	EventMatchesData* em = (EventMatchesData*)event->getUserData();
	addScore(em->matches);
}
//--------------------------------------------------------------------------------------------------------
void GameLayer::menuCloseCallback(Ref* pSender, cocos2d::ui::Widget::TouchEventType a_type)
{
	if (a_type == cocos2d::ui::Widget::TouchEventType::ENDED)
	{
		dialogResult("Go to the level select mode");
	}
}
//--------------------------------------------------------------------------------------------------------
void GameLayer::writeConfig()
{
	rapidjson::Value json_val;
	rapidjson::Document doc;
	auto& allocator = doc.GetAllocator();

	doc.SetObject();

	rapidjson::Value arrayObjects(rapidjson::kArrayType);
	for (auto it : m_LevelConfig)
	{
		rapidjson::Value object(rapidjson::kObjectType);
		object.AddMember("ID", it.id, allocator);
		object.AddMember("LOCK", it.lock, allocator);
		object.AddMember("NAME", rapidjson::Value(it.name.c_str(), it.name.size(), allocator).Move(), allocator);
		object.AddMember("SCORE", it.score, allocator);
		object.AddMember("RECORD", it.record, allocator);
		object.AddMember("MOVES", it.moves, allocator);
		object.AddMember("ROWS", it.rows, allocator);
		object.AddMember("COLS", it.cols, allocator);
		object.AddMember("SUCCESS", it.success, allocator);

		rapidjson::Value _array(rapidjson::kArrayType);
		for(auto _m : it.matrix)
			_array.PushBack(_m, allocator);
		object.AddMember("MATRIX", _array, allocator);
		arrayObjects.PushBack(object, allocator);
	}
	doc.AddMember("LEVELS", arrayObjects, allocator);

	rapidjson::StringBuffer strbuf;
	rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
	doc.Accept(writer);

	std::string json(strbuf.GetString(), strbuf.GetSize());
	std::string patchToRes = "Levels.json";
	auto data = FileUtils::getInstance()->getDefaultResourceRootPath();
	data += "/res/" + patchToRes;
	FileUtils::getInstance()->writeStringToFile(json, data);
}
//--------------------------------------------------------------------------------------------------------
void GameLayer::dialogControlSimple()
{
	Vec2 origin      = Director::getInstance()->getVisibleOrigin();
	Vec2 visibleSize = Director::getInstance()->getVisibleSize();
	// popup base
	auto popup_base = Sprite::createWithSpriteFrameName("popup_base.png");
	popup_base->setPosition(origin.x + visibleSize.x / 2, origin.y + visibleSize.y / 2);
	m_dialogControl.pushBack(popup_base);
	popup_base->setVisible(false);
	this->addChild(popup_base);

	// popup header
	auto popup_header = Sprite::createWithSpriteFrameName("popup_header.png");
	popup_header->setPosition(origin.x + visibleSize.x / 2, origin.y + visibleSize.y / 1.6);
	popup_header->setScale(0.9f);
	m_dialogControl.pushBack(popup_header);
	popup_header->setVisible(false);
	this->addChild(popup_header);

	// text
	auto textLabel = Label::createWithSystemFont("", "Marker Felt", 18);
	textLabel->setColor(Color3B::BLACK);
	textLabel->enableShadow(cocos2d::Color4B::GRAY, cocos2d::Size(1.0f, 1.0f), 1);
	textLabel->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	textLabel->setPosition(origin.x + visibleSize.x / 2, origin.y + visibleSize.y / 2);
	m_dialogControl.pushBack(textLabel);
	textLabel->setVisible(false);
	this->addChild(textLabel);

	//buttons
	auto buttonYes = cocos2d::ui::Button::create("button_9slice.png", "button_9slice01.png", "button_9slice.png", cocos2d::ui::TextureResType::PLIST);
	buttonYes->setPosition(cocos2d::Vec2(origin.x + visibleSize.x / 2.5, origin.y + visibleSize.y / 2.5));
	buttonYes->addTouchEventListener(CC_CALLBACK_2(GameLayer::menuYesCallback, this));
	buttonYes->setTitleText("Yes");
	buttonYes->setTitleFontSize(38.0f);
	buttonYes->setScale(0.5f);
	buttonYes->setVisible(false);
	m_dialogControl.pushBack(buttonYes);
	this->addChild(buttonYes);

	auto buttonNo = cocos2d::ui::Button::create("button_9slice.png", "button_9slice01.png", "button_9slice.png", cocos2d::ui::TextureResType::PLIST);
	buttonNo->setPosition(cocos2d::Vec2(origin.x + visibleSize.x / 1.66, origin.y + visibleSize.y / 2.5));
	buttonNo->addTouchEventListener(CC_CALLBACK_2(GameLayer::menuNoCallback, this));
	buttonNo->setTitleText("No");
	buttonNo->setScale(0.5f);
	buttonNo->setTitleFontSize(38.0f);
	buttonNo->setVisible(false);
	m_dialogControl.pushBack(buttonNo);
	this->addChild(buttonNo);
}
//--------------------------------------------------------------------------------------------------------
void GameLayer::dialogResult(std::string strMessage)
{
	// unVisible background
	for(auto i : m_mainControl)
		i->setVisible(false);
	for (auto i : m_dialogControl)
	{
		i->setVisible(true);
		if (auto label = dynamic_cast<Label*>(i))
			label->setString(strMessage);
	}
}

//--------------------------------------------------------------------------------------------------------
void GameLayer::menuYesCallback(Ref* /*pSender*/, cocos2d::ui::Widget::TouchEventType a_type)
{
	if (a_type == cocos2d::ui::Widget::TouchEventType::BEGAN)
	{
		if (CocosDenshion::SimpleAudioEngine::getInstance()->isBackgroundMusicPlaying())
			CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("main.mp3", true);
		this->stopAllActions();
		Director::getInstance()->popScene();
	}
}
//--------------------------------------------------------------------------------------------------------
void GameLayer::menuNoCallback(Ref* pSender, cocos2d::ui::Widget::TouchEventType a_type)
{
	if (a_type == cocos2d::ui::Widget::TouchEventType::BEGAN)
	{
		for(auto i : m_dialogControl)
			i->setVisible(false);
		for (auto i : m_mainControl)
			i->setVisible(true);
	}
}
//--------------------------------------------------------------------------------------------------------