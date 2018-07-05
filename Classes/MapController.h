#pragma once
#include "cocos2d.h"
using namespace cocos2d;
/*
*这个类是用来控制地图的要素
*包含访问地图上每一个格子的函数、修改格子颜色的函数
*
*/
class Playground :public cocos2d::Ref {
public:
	//获取地图
	static Playground* getInstance();

	void initMap();


	void setColor(Vec2 inputPosition, Color3B inputColor) {//这个函数改变指定位置格子的颜色
		//CCLOG("%f  ######%f", inputPosition.x, inputPosition.y);
		auto tile1 = backGroundLayer->getTileAt(Point(inputPosition.x, inputPosition.y));//获取指定格子
		tile1->setColor(inputColor);//改变颜色
	}
	bool setColor(Vec2 otherPlayerPosition,std::vector<Vec2> inputPosition, Color3B inputColor) {//这个函数按次序改变队列中格子的颜色,并且根据是否能攻击到返回truefalse
		//setColor(inputPosition.at(0), inputColor);
		bool temp = false;
		for (int i = 0; i < inputPosition.size(); i++) {
			if (isValid(inputPosition.at(i))) {//仅当位置合法的时候变色
				setColor(inputPosition.at(i), inputColor);
				if (inputPosition.at(i) == tileCoordForPosition(otherPlayerPosition)) {
					temp = true;
				}
			}
		}
		return temp;//查看敌人是否被攻击到了
	}
	Color3B getColor(Vec2 inputPosition) {//获取一个格子的颜色
		auto tile1 = backGroundLayer->getTileAt(Point(inputPosition.x, inputPosition.y));//获取指定格子
		return tile1->getColor();
	}
	// OpenGL坐标转成格子坐标
	Vec2 tileCoordForPosition(const Vec2& position)
	{
		int x = (position.x) / tileSize.width;// *12 / 11 - 4;
		//CCLOG("position.x = %f,tileSize.width = %f", position.x, tileSize.width);
		int y = (mapSize.height*tileSize.width - position.y) / tileSize.width;// *8 / 7 + 2;
		return Vec2(x, y);
	}
	Vec2 positionForTileCoord(const Vec2 tileCoord)
	{
		int x = tileCoord.x * tileSize.width + tileSize.width / 2;
		int y = (mapSize.height - tileCoord.y)*tileSize.height - tileSize.height / 2;
		return Vec2(x, y);
	}
	bool isValid(Vec2 position) {//这个函数判断一个格子是否在战斗区域内
		if (position.x < 2.9 || position.y < 5.9 || position.x>14 || position.y>11) {
			//CCLOG("Refused%f %f", position.x, position.y);
			return false;
		}
		return true;
	}
	Size tileSize;
	Size mapSize;
	TMXTiledMap* tmx;//地图文件
private:
	Playground();
	TMXLayer* backGroundLayer;
	TMXLayer* battleGroundLayer;
	static Playground* playground;

	TMXObjectGroup* objects;//地图对象
};