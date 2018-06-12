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
	backGroundLayer = tmx->getLayer("backGroundLayer");
	mapSize = tmx->getMapSize();
	tileSize = tmx->getTileSize();
	CCLOG("%f", tileSize.width);
}
/*void Playground::setColor(std::vector<Vec2> inputArea, Color3B inputColor)
{
	if (inputArea.size() <= 0) {
		return;
	}
	CCLOG("tile1x = %f,tile1y = %f", inputArea[0].x, inputArea[0].y);
	//CCLOG("tile1x = %f,tile1y = %f", inputArea.at(1).x, inputArea.at(1).y);
	CCLOG("inputAreaSize = %d", inputArea.size());
	for (int i = 0; i < inputArea.size(); i++) {
		CCLOG("%d,%d", inputArea.at(i).x, inputArea.at(i).y);
		if (inputArea[i].x < 0 || inputArea[i].x > mapSize.width || inputArea[i].y < 0 || inputArea[i].y > mapSize.height) {
			continue;
		}
		auto tile1 = backGroundLayer->getTileAt(Point(inputArea.at(i).x, inputArea.at(i).y));
		tile1->setColor(inputColor);
	}

}


*/
