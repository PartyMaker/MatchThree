#include "MainLayer.h"

USING_NS_CC;
//--------------------------------------------------------------------------------------------------------
Scene* MainLayer::createScene()
{
	auto scene = Scene::create();
	auto layer = MainLayer::create();
	scene->addChild(layer);
	return scene;
}
//--------------------------------------------------------------------------------------------------------
bool MainLayer::init()
{
    if (!Layer::init())
        return false;

	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("cityscene.plist");
	
	if(CocosDenshion::SimpleAudioEngine::getInstance()->isBackgroundMusicPlaying())
		CocosDenshion::SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
	else 
		CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("main.mp3", true);

	Vec2 origin      = Director::getInstance()->getVisibleOrigin();
	Vec2 visibleSize = Director::getInstance()->getVisibleSize();

	// background
	auto background = Sprite::createWithSpriteFrameName("bg.png");
	background->setPosition(origin.x + visibleSize.x / 2, origin.y + visibleSize.y / 2);
	this->addChild(background);

	auto bg_effect_flare = Sprite::createWithSpriteFrameName("bg_effect_flare.png");
	bg_effect_flare->setPosition(origin.x + visibleSize.x / 2, origin.y + visibleSize.y / 1.1);
	this->addChild(bg_effect_flare);

	auto game_title = Sprite::createWithSpriteFrameName("game_title.png");
	game_title->setPosition(origin.x + visibleSize.x / 2, origin.y + visibleSize.y / 1.5);
	this->addChild(game_title);

	auto popup_base = Sprite::createWithSpriteFrameName("popup_base.png");
	popup_base->setScale(0.92f, 0.6f);
	popup_base->setPosition(origin.x + visibleSize.x / 2, origin.y + visibleSize.y / 4);
	this->addChild(popup_base);

	auto popup_header = Sprite::createWithSpriteFrameName("popup_header.png");
	popup_header->setScale(0.8f);
	popup_header->setPosition(origin.x + visibleSize.x / 2, origin.y + visibleSize.y / 3.1);
	this->addChild(popup_header);

	//buttons
	auto buttonClose = cocos2d::ui::Button::create("button_close01.png", "button_close02.png", "button_close01.png", cocos2d::ui::TextureResType::PLIST);
	buttonClose->setPosition(cocos2d::Vec2(origin.x + visibleSize.x / 1.07, origin.y + visibleSize.y / 1.05));
	buttonClose->addTouchEventListener(CC_CALLBACK_1(MainLayer::menuCloseCallback, this));
	this->addChild(buttonClose);
	
	auto buttonStart = cocos2d::ui::Button::create("button_9slice.png", "button_9slice01.png", "button_9slice01.png", cocos2d::ui::TextureResType::PLIST);
	buttonStart->setTitleText("PLAY");
	buttonStart->setTitleFontName("Marker Felt"); 
	buttonStart->setTitleFontSize(29.0f);
	buttonStart->setPosition(cocos2d::Vec2(origin.x + visibleSize.x / 2, origin.y + visibleSize.y /7));
	buttonStart->addTouchEventListener(CC_CALLBACK_2(MainLayer::menuPlayCallback, this));
	this->addChild(buttonStart);

	auto buttonInfo = cocos2d::ui::Button::create("button_info01.png", "button_info01.png", "button_info02.png", cocos2d::ui::TextureResType::PLIST);
	buttonInfo->setPosition(cocos2d::Vec2(origin.x + visibleSize.x / 2, origin.y + visibleSize.y / 4));
	buttonInfo->addTouchEventListener(CC_CALLBACK_2(MainLayer::menuInfoCallback, this));
	this->addChild(buttonInfo);

	auto buttonAudio = cocos2d::ui::Button::create("button_audioUnMute.png", "button_audioUnMute.png", "button_audioMute.png", cocos2d::ui::TextureResType::PLIST);
	buttonAudio->setPosition(cocos2d::Vec2(origin.x + visibleSize.x / 1.6, origin.y + visibleSize.y / 4));
	buttonAudio->addTouchEventListener(CC_CALLBACK_2(MainLayer::menuSoundCallback, this));
	this->addChild(buttonAudio);

	auto buttonAwards = cocos2d::ui::Button::create("button_awards.png", "button_awards01.png", "button_awards.png", cocos2d::ui::TextureResType::PLIST);
	buttonAwards->setPosition(cocos2d::Vec2(origin.x + visibleSize.x / 2.7, origin.y + visibleSize.y / 4));
	buttonAwards->addTouchEventListener(CC_CALLBACK_2(MainLayer::menuAwardsCallback, this));
	this->addChild(buttonAwards);

	return true;
}
//--------------------------------------------------------------------------------------------------------
void MainLayer::menuCloseCallback(Ref* pSender)
{
    //Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();
}
//--------------------------------------------------------------------------------------------------------
void MainLayer::menuPlayCallback(cocos2d::Ref* /*a_pSender*/, ui::Widget::TouchEventType a_type)
{
	if (a_type == ui::Widget::TouchEventType::BEGAN)
	{
		auto scene = LevelLayer::createScene();
		if (!scene)
		{
			CCLOGERROR("Cannot initialize scene!");
			return;
		}
		Director::getInstance()->pushScene(scene);
	}
}
//--------------------------------------------------------------------------------------------------------
void MainLayer::menuSoundCallback(cocos2d::Ref* pSender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == cocos2d::ui::Widget::TouchEventType::BEGAN)
	{
		auto bt = reinterpret_cast<cocos2d::ui::Button*>(pSender);
		if (bt == nullptr) return;
		if (bt->getNormalFile().file == "button_audioUnMute.png")
		{
			CocosDenshion::SimpleAudioEngine::getInstance()->stopAllEffects();
			CocosDenshion::SimpleAudioEngine::getInstance()->stopBackgroundMusic();
			bt->loadTextures("button_audioMute.png", "button_audioMute.png", "button_audioUnMute.png", cocos2d::ui::TextureResType::PLIST);
		}
		else
		{
			CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("main.mp3", true);
			bt->loadTextures("button_audioUnMute.png", "button_audioMute.png", "button_audioMute.png", cocos2d::ui::TextureResType::PLIST);
		}
	}
}
//--------------------------------------------------------------------------------------------------------

void MainLayer::menuAwardsCallback(cocos2d::Ref* /*pSender*/, cocos2d::ui::Widget::TouchEventType /*type*/)
{
	//TODO: do something
}
//--------------------------------------------------------------------------------------------------------
void MainLayer::menuInfoCallback(cocos2d::Ref* /*pSender*/, cocos2d::ui::Widget::TouchEventType /*type*/)
{

	//TODO: do something
}
//--------------------------------------------------------------------------------------------------------