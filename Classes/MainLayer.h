#pragma once
#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "SimpleAudioEngine.h"
#include "LevelLayer.h"

class MainLayer : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();
    virtual bool init();
    CREATE_FUNC(MainLayer);

private:
	void menuCloseCallback (cocos2d::Ref* pSender);  // selector close callback
	void menuPlayCallback  (cocos2d::Ref* pSender, cocos2d::ui::Widget::TouchEventType a_type); 
	void menuAwardsCallback(cocos2d::Ref* pSender, cocos2d::ui::Widget::TouchEventType a_type); 
	void menuInfoCallback  (cocos2d::Ref* pSender, cocos2d::ui::Widget::TouchEventType a_type); 
	void menuSoundCallback (cocos2d::Ref* pSender, cocos2d::ui::Widget::TouchEventType a_type); 
};