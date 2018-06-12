#include"MapController.h"
USING_NS_CC;

Playground* Playground::playground = NULL;
Playground::Playground() {
	initMap();
}
Playground* Playground::getInstance() {
	if (playground == NULL) {
		playground = new Playground();
		
	}
	return playground;
}
void Playground::initMap() {//初始化tmx，使得tmx可以被主函数加入layer
	cocos2d::Size visibleSize = Director::getInstance()->getVisibleSize();
	tmx = TMXTiledMap::create("finalMap.tmx");//创建地图
	CCLOG("inited");
	tmx->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	tmx->setAnchorPoint(Vec2(0.5, 0.5));
	tmx->setScale(Director::getInstance()->getContentScaleFactor());
	backGroundLayer = tmx->getLayer("backGround");
	battleGroundLayer = tmx->getLayer("battleGround");
	mapSize = tmx->getMapSize();
	tileSize = tmx->getTileSize();
	CCLOG("%f", tileSize.width);
}
/*void Playground::setColor(std::vector<Vec2> inputArea, Color3B inputColor)
{


}*/



