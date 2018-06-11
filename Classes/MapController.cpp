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
void Playground::initMap() {//��ʼ��tmx��ʹ��tmx���Ա�����������layer
	cocos2d::Size visibleSize = Director::getInstance()->getVisibleSize();
	tmx = TMXTiledMap::create("map.tmx");//������ͼ
	CCLOG("inited");
	tmx->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	tmx->setAnchorPoint(Vec2(0.5, 0.5));
	tmx->setScale(Director::getInstance()->getContentScaleFactor());
	backGroundLayer = tmx->getLayer("backGroundLayer");
	mapSize = tmx->getMapSize();
	tileSize = tmx->getTileSize();

}




