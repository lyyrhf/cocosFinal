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
		CCLOG("%f  ######%f", inputPosition.x, inputPosition.y);
		auto tile1 = backGroundLayer->getTileAt(Point(inputPosition.x, inputPosition.y));//获取指定格子
		tile1->setColor(inputColor);//改变颜色
	}
	void setColor(std::vector<Vec2> inputPosition, Color3B inputColor) {//这个函数按次序改变队列中格子的颜色
		setColor(inputPosition.at(0), inputColor);
		for (int i = 0; i < inputPosition.size(); i++) {
			setColor(inputPosition.at(i), inputColor);
		}
	}

	// OpenGL坐标转成格子坐标
	Vec2 tileCoordForPosition(const Vec2& position)
	{
		int x = (position.x) / tileSize.width * 12 / 11 - 4 ;
		CCLOG("position.x = %f,tileSize.width = %f", position.x, tileSize.width);
		int y = (mapSize.height*tileSize.width - position.y) /tileSize.width * 8 / 7 + 2;
		return Vec2(x, y);
	}
	Vec2 positionForTileCoord(const Vec2& tileCoord)
	{
		int x = tileCoord.x * tileSize.width + tileSize.width / 2;
		int y = (mapSize.height - tileCoord.y)*tileSize.height - tileSize.height / 2;
		return Vec2(x, y);
	}

	Size tileSize;
	Size mapSize;
	TMXTiledMap* tmx;//地图文件
private:
	Playground();
	TMXLayer* backGroundLayer;

	static Playground* playground;

	TMXObjectGroup* objects;//地图对象
};