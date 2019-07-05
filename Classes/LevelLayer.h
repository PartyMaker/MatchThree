#pragma once
#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "Global.h"

class LevelLayer : public cocos2d::Layer
{
public:
	static cocos2d::Scene* createScene();
	virtual bool init();	
	virtual void onEnter();
	CREATE_FUNC(LevelLayer);

private:
	cocos2d::Vector<Node*>    m_MemuLevels;
	std::vector<TLevelConfig> m_levels;

	void menuCloseCallback    (cocos2d::Ref* pSender, cocos2d::ui::Widget::TouchEventType a_type);
	void menuStartGameCallback(cocos2d::Ref* pSender, cocos2d::ui::Widget::TouchEventType a_type);
	std::vector<TLevelConfig> parseLevelConfig();
};