#pragma once
#include <cocos2d.h>
#include "Grid.h"
#include "Global.h"
#include "ui/CocosGUI.h"
#include "external/json/document.h"
#include "LevelLayer.h"

class GameLayer : public cocos2d::Layer
{
public:
	static cocos2d::Scene* createScene(const std::vector<TLevelConfig>& a_config, size_t index);
	virtual bool init(const std::vector<TLevelConfig>& a_config, size_t index);

private:
	cocos2d::Vector<cocos2d::Node*> m_dialogControl;
	cocos2d::Vector<cocos2d::Node*> m_mainControl;
	TLevelConfig                    m_Config;
	std::vector<TLevelConfig>       m_LevelConfig;
	size_t                          m_indexCurConfig;
	Block*                          m_activeBlock;
	Grid*                           m_grid;
	bool                            m_isBisy;
	cocos2d::Label*                 m_scoreLabel;
	cocos2d::Label*                 m_movesLabel;
	std::vector<GridMove>           m_availableMoves;
	int                             m_currentScore;
	size_t                          m_moves_left;

	void dialogControlSimple();
	void swapBoxes(Block* first, Block* second);
	void checkForMatches(Block* first, Block* second);
	void lockTouches(float time);
	void unlockTouches(float dt);
	void addScore(int score);
	void removeMoves(int moves);
	void resolveMatchesForBlock(Block* block);
	void writeConfig();
	void dialogResult(std::string message);
	void gameOver();

	bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
	void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
	void onGridReady  (cocos2d::EventCustom* event);
	void onGridMatches(cocos2d::EventCustom* event);
	void newGame (cocos2d::Ref* ref);
	void showHint(cocos2d::Ref* ref,     cocos2d::ui::Widget::TouchEventType a_type);
	void menuCloseCallback(Ref* pSender, cocos2d::ui::Widget::TouchEventType a_type);
	void menuYesCallback  (Ref* pSender, cocos2d::ui::Widget::TouchEventType a_type);
	void menuNoCallback   (Ref* pSender, cocos2d::ui::Widget::TouchEventType a_type);
};
