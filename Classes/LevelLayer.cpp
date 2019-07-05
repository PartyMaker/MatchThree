#include "LevelLayer.h"
#include "external/json/document.h"
#include "MainLayer.h"
#include "GameLayer.h"

USING_NS_CC;
//--------------------------------------------------------------------------------------------------------
Scene* LevelLayer::createScene()
{
	auto scene = Scene::create();
	auto layer = LevelLayer::create();
	scene->addChild(layer);
	return scene;
}
//--------------------------------------------------------------------------------------------------------
bool LevelLayer::init()
{
	if (!Layer::init())
		return false;

	Vec2 origin      = Director::getInstance()->getVisibleOrigin();
	Vec2 visibleSize = Director::getInstance()->getVisibleSize();

	// background
	auto background = Sprite::createWithSpriteFrameName("bg.png");
	background->setPosition(origin.x + visibleSize.x / 2, origin.y + visibleSize.y / 2);
	this->addChild(background);
	
	auto shop_title = Sprite::createWithSpriteFrameName("shop_title.png");
	shop_title->setPosition(origin.x + visibleSize.x / 6, origin.y + visibleSize.y / 1.1);
	this->addChild(shop_title);

	auto bg_effect_flare = Sprite::createWithSpriteFrameName("bg_effect_flare.png");
	bg_effect_flare->setPosition(origin.x + visibleSize.x / 6, origin.y + visibleSize.y);
	bg_effect_flare->setScale(0.7f);
	this->addChild(bg_effect_flare);

	//buttons
	auto buttonClose = cocos2d::ui::Button::create("button_replay01.png", "button_replay01.png", "button_replay01.png", cocos2d::ui::TextureResType::PLIST);
	buttonClose->setPosition(cocos2d::Vec2(origin.x + visibleSize.x / 1.07, origin.y + visibleSize.y / 1.05));
	buttonClose->addTouchEventListener(CC_CALLBACK_2(LevelLayer::menuCloseCallback, this));
	this->addChild(buttonClose);
	
	return true;
}
//--------------------------------------------------------------------------------------------------------
void LevelLayer::onEnter()
{
	Layer::onEnter();

	for (auto i : m_MemuLevels)
		i->removeFromParent();

	TLevelConfig config = TLevelConfig(); // for leveles
	config.rows = 5; // TODO: magic grid
	config.cols = 6; // TODO: magic grid
	auto _LevelConfig = std::move(parseLevelConfig());
	m_levels = _LevelConfig;
	size_t count = 0;
	for (size_t col = config.cols; col > 0; --col)
	{
		for (size_t row = 0; row < config.rows; ++row)
		{
			if (count >= m_levels.size())
				break;
			auto shop_title = Sprite::createWithSpriteFrameName("shop_title.png");
			const Size sizeShop = shop_title->getContentSize() / 2;

			// main image
			auto mainImage = Sprite::createWithSpriteFrameName("characters_0006.png");
			mainImage->setPosition(row * mainImage->getContentSize().width / 2 + mainImage->getContentSize().width / 2 * 0.5f,
				col * mainImage->getContentSize().height / 2 + mainImage->getContentSize().height / 2 * 0.5f - sizeShop.height);
			mainImage->setScale(0.5f);
			this->addChild(mainImage, 1);
			m_MemuLevels.pushBack(mainImage);

			// image success
			auto levelSuccess = Sprite::createWithSpriteFrameName("iconLevel.png");
			levelSuccess->setPosition(row * mainImage->getContentSize().width / 2 + mainImage->getContentSize().width / 2 * 0.5f - 30,
				col * mainImage->getContentSize().height / 2 + mainImage->getContentSize().height / 2 * 0.5f + 30 - sizeShop.height);
			levelSuccess->setScale(0.5f);
			this->addChild(levelSuccess, 1);
			m_MemuLevels.pushBack(levelSuccess);
			// button - start level
			auto bt = cocos2d::ui::Button::create("button_9slice.png", "button_9slice01.png", "button_9slice.png", cocos2d::ui::TextureResType::PLIST);
			bt->setScale(0.4f);
			bt->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE);
			bt->setPosition(cocos2d::Vec2(row * mainImage->getContentSize().width / 2 + mainImage->getContentSize().width / 2 * 0.5f,
				col * mainImage->getContentSize().height / 2 + mainImage->getContentSize().height / 2 * 0.5f - 50 - sizeShop.height));
			bt->setTitleText(m_levels[count].name);
			bt->setTitleFontSize(30);
			bt->addTouchEventListener(CC_CALLBACK_2(LevelLayer::menuStartGameCallback, this));
			this->addChild(bt, 1);
			m_MemuLevels.pushBack(bt);

			if (m_levels[count].lock)
			{
				bt->setEnabled(false);
				bt->setColor(cocos2d::Color3B::GRAY);
				mainImage->setColor(cocos2d::Color3B::GRAY);
			}
			if (!m_levels[count].success)
				levelSuccess->setVisible(false);

			++count;
		}
	}
}
//--------------------------------------------------------------------------------------------------------
std::vector<TLevelConfig> LevelLayer::parseLevelConfig()
{
	rapidjson::Document document;
	std::vector<TLevelConfig> _levels;
	std::string patchToRes = "Levels.json";
	auto data = FileUtils::getInstance()->getDefaultResourceRootPath();
	data += "/res/" + patchToRes;
	std::string content = FileUtils::getInstance()->getStringFromFile(data.c_str());
	log("file content: %s", content.c_str());
	document.Parse<0>(content.c_str());
	if (document.IsObject())
	{
		if (document.HasMember("LEVELS"))
		{
			const rapidjson::Value& _array = document["LEVELS"]; 
			assert(_array.IsArray());
			for (rapidjson::SizeType i = 0; i < _array.Size(); ++i)
			{
				const rapidjson::Value& atomicObject = _array[i];
				TLevelConfig level;
				if (atomicObject.HasMember("ID"))
				{
					const rapidjson::Value& name = atomicObject["ID"];
					level.id = cocos2d::Value(name.GetUint()).asUnsignedInt();
				}
				if (atomicObject.HasMember("LOCK"))
				{
					const rapidjson::Value& name = atomicObject["LOCK"];
					level.lock = cocos2d::Value(name.GetBool()).asBool();
				}
				if (atomicObject.HasMember("NAME"))
				{
					const rapidjson::Value& name = atomicObject["NAME"];
					level.name = cocos2d::Value(name.GetString()).asString();
				}
				if (atomicObject.HasMember("SCORE"))
				{
					const rapidjson::Value& name = atomicObject["SCORE"];
					level.score = cocos2d::Value(name.GetUint()).asUnsignedInt();
				}
				if (atomicObject.HasMember("RECORD"))
				{
					const rapidjson::Value& name = atomicObject["RECORD"];
					level.record = cocos2d::Value(name.GetUint()).asUnsignedInt();
				}
				if (atomicObject.HasMember("MOVES"))
				{
					const rapidjson::Value& name = atomicObject["MOVES"];
					level.moves = cocos2d::Value(name.GetUint()).asUnsignedInt();
				}
				if (atomicObject.HasMember("ROWS"))
				{
					const rapidjson::Value& name = atomicObject["ROWS"];
					level.rows = cocos2d::Value(name.GetUint()).asUnsignedInt();
				}
				if (atomicObject.HasMember("COLS"))
				{
					const rapidjson::Value& name = atomicObject["COLS"];
					level.cols = cocos2d::Value(name.GetUint()).asUnsignedInt();
				}
				if (atomicObject.HasMember("SUCCESS"))
				{
					const rapidjson::Value& name = atomicObject["SUCCESS"];
					level.success = cocos2d::Value(name.GetBool()).asBool();
				}
				if (atomicObject.HasMember("MATRIX"))
				{
					const rapidjson::Value& _matrixArray = atomicObject["MATRIX"];
					assert(_matrixArray.IsArray());
					for (rapidjson::SizeType j = 0; j < _matrixArray.Size(); ++j)
						level.matrix.push_back(cocos2d::Value(_matrixArray[j].GetUint()).asUnsignedInt());
				}
				_levels.push_back(level);
			}
		}
	}
	return _levels;
}
//--------------------------------------------------------------------------------------------------------
void LevelLayer::menuCloseCallback(Ref* /*pSender*/, cocos2d::ui::Widget::TouchEventType a_type)
{
	if (a_type == ui::Widget::TouchEventType::BEGAN)
	{
		this->stopAllActions();
		Director::getInstance()->popScene();
	}
}
//--------------------------------------------------------------------------------------------------------
void LevelLayer::menuStartGameCallback(cocos2d::Ref* pSender, cocos2d::ui::Widget::TouchEventType a_type)
{
	auto bt = (cocos2d::ui::Button*)(pSender);
	if (a_type == ui::Widget::TouchEventType::BEGAN)
	{
		auto buttonText = bt->getTitleText();
		size_t index = 0;
		for (size_t i = 0; m_levels.size(); ++i)
			if (m_levels[i].name == buttonText)
			{
				index = i;
				break;
			}
		auto scene = GameLayer::createScene(m_levels, index);
		if (!scene)
		{
			CCLOGERROR("Cannot initialize scene!");
			return;
		}
		Director::getInstance()->pushScene(scene);
	}
}
//--------------------------------------------------------------------------------------------------------