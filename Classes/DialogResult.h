#pragma once
#include <cocos2d.h>
#include "Global.h"
#include "ui/CocosGUI.h"
#include <string>

class DialogResult : public cocos2d::Layer
{
public:
	static cocos2d::Scene* createScene(std::string message);
	virtual bool init(std::string message);
	virtual void onEnter();

private:
	std::string strMessage;
	void menuYesCallback(Ref* pSender, cocos2d::ui::Widget::TouchEventType a_type);
	void menuNoCallback(Ref* pSender, cocos2d::ui::Widget::TouchEventType a_type);
};

class MyMenu : public cocos2d::Layer
{
public:
	static MyMenu* createMenu();
	virtual bool init();
};