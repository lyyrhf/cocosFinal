#include"Monster.h"
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
	tmx = TMXTiledMap::create("map.tmx");//创建地图
	CCLOG("inited");
	tmx->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	tmx->setAnchorPoint(Vec2(0.5, 0.5));
	tmx->setScale(Director::getInstance()->getContentScaleFactor());
	backGroundLayer = tmx->getLayer("backGroundLayer");
	mapSize = tmx->getMapSize();
	tileSize = tmx->getTileSize();

}




