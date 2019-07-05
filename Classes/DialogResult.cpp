#include "DialogResult.h"
#include <cocos2d.h>
#include "Global.h"
#include "ui/CocosGUI.h"
#include "LevelLayer.h"
#include "MainLayer.h"

USING_NS_CC;
//--------------------------------------------------------------------------------------------------------
cocos2d::Scene* DialogResult::createScene(std::string message)
{
	auto scene = Scene::create();
	auto gameScene = new DialogResult();

	gameScene->autorelease();

	if (gameScene->init(message))
		scene->addChild(gameScene);
	else
		return nullptr;

	return scene;
}
//--------------------------------------------------------------------------------------------------------
bool DialogResult::init(std::string message)
{
	if (!Layer::init())
		return false;

	strMessage = message;
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("cityscene.plist");

	return true;
}
//--------------------------------------------------------------------------------------------------------
void DialogResult::menuYesCallback(Ref* pSender, cocos2d::ui::Widget::TouchEventType a_type)
{
	if (a_type == cocos2d::ui::Widget::TouchEventType::BEGAN)
	{
		CocosDenshion::SimpleAudioEngine::getInstance()->stopBackgroundMusic();
		auto scene = LevelLayer::createScene();
		Director::getInstance()->replaceScene(scene);
	}
}
//--------------------------------------------------------------------------------------------------------
void DialogResult::menuNoCallback(Ref* pSender, cocos2d::ui::Widget::TouchEventType a_type)
{
	if (a_type == cocos2d::ui::Widget::TouchEventType::BEGAN)
	{
		TLevelConfig _config;
		Director::getInstance()->popScene();
	}
}
//--------------------------------------------------------------------------------------------------------
void DialogResult::onEnter()
{
	Layer::onEnter();

	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	Vec2 visibleSize = Director::getInstance()->getVisibleSize();

	// background
	auto background = Sprite::createWithSpriteFrameName("bg.png");
	background->setPosition(origin.x + visibleSize.x / 2, origin.y + visibleSize.y / 2);
	background->setColor(cocos2d::Color3B::GRAY);
	this->addChild(background);

	// popup base
	auto popup_base = Sprite::createWithSpriteFrameName("popup_base.png");
	popup_base->setPosition(origin.x + visibleSize.x / 2, origin.y + visibleSize.y / 2);
	this->addChild(popup_base);

	// popup header
	auto popup_header = Sprite::createWithSpriteFrameName("popup_header.png");
	popup_header->setPosition(origin.x + visibleSize.x / 2, origin.y + visibleSize.y / 1.6);
	popup_header->setScale(0.9f);
	this->addChild(popup_header);

	// text
	auto textLabel = Label::createWithSystemFont(strMessage, "Marker Felt", 18);
	textLabel->setColor(Color3B::BLACK);
	textLabel->enableShadow(cocos2d::Color4B::GRAY, cocos2d::Size(1.0f, 1.0f), 1);
	textLabel->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	textLabel->setPosition(origin.x + visibleSize.x / 2, origin.y + visibleSize.y / 2);
	this->addChild(textLabel);

	auto buttonYes = cocos2d::ui::Button::create("button_9slice.png", "button_9slice01.png", "button_9slice.png", cocos2d::ui::TextureResType::PLIST);
	buttonYes->setPosition(cocos2d::Vec2(origin.x + visibleSize.x / 2.5, origin.y + visibleSize.y / 2.5));
	buttonYes->addTouchEventListener(CC_CALLBACK_2(DialogResult::menuYesCallback, this));
	buttonYes->setTitleText("Yes");
	buttonYes->setTitleFontSize(38.0f);
	buttonYes->setScale(0.5f);
	this->addChild(buttonYes);

	auto buttonNo = cocos2d::ui::Button::create("button_9slice.png", "button_9slice01.png", "button_9slice.png", cocos2d::ui::TextureResType::PLIST);
	buttonNo->setPosition(cocos2d::Vec2(origin.x + visibleSize.x / 1.66, origin.y + visibleSize.y / 2.5));
	buttonNo->addTouchEventListener(CC_CALLBACK_2(DialogResult::menuNoCallback, this));
	buttonNo->setTitleText("No");
	buttonNo->setScale(0.5f);
	buttonNo->setTitleFontSize(38.0f);
	this->addChild(buttonNo);
}
//--------------------------------------------------------------------------------------------------------

MyMenu* MyMenu::createMenu()
{
	MyMenu* menu = new MyMenu();

	if (menu->init())
	{
		menu->autorelease();
		menu->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		return menu;
	}
	return nullptr;
}

bool MyMenu::init()
{
	return true;
}
