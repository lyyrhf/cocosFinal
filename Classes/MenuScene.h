#include <stdio.h>
#include "cocos2d.h"

class MenuScene : public cocos2d::Scene
{
public:
	static cocos2d::Scene* createScene();

	virtual bool init();

	void menuStartCallBack(Ref* pSender);

	// implement the "static create()" method manually
	CREATE_FUNC(MenuScene);
};