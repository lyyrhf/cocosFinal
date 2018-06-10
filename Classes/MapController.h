#pragma once
#include "cocos2d.h"
using namespace cocos2d;
/*
*��������������Ƶ�ͼ��Ҫ��
*�������ʵ�ͼ��ÿһ�����ӵĺ������޸ĸ�����ɫ�ĺ���
*
*/
class Playground :public cocos2d::Ref {
public:
	//��ȡ��ͼ
	static Playground* getInstance();

	void initMap();


	void setColor(Vec2 inputPosition, Color3B inputColor) {//��������ı�ָ��λ�ø��ӵ���ɫ
		CCLOG("%f  ######%f", inputPosition.x, -inputPosition.y);
		auto tile1 = backGroundLayer->getTileAt(Point(inputPosition.x, -inputPosition.y));//��ȡָ������
		tile1->setColor(inputColor);//�ı���ɫ
	}
	void setColor(std::vector<Vec2> inputPosition, Color3B inputColor) {//�������������ı�����и��ӵ���ɫ

	}
	// OpenGL����ת�ɸ�������
	Vec2 tileCoordForPosition(const Vec2& position)
	{
		//CCLOG("ContentSize == %f", tmx->getContentSize().width);
		//CCLOG("ContentSize == %f", tmx->getContentSize().height);

		int x = (position.x) / tileSize.width * 12 / 11;
		CCLOG("position.x = %f,tileSize.width = %f", position.x, tileSize.width);
		int y = (mapSize.height*tileSize.width - position.y) / tileSize.width * 8 / 7;
		return Vec2(x, y);
	}
	Vec2 positionForTileCoord(const Vec2& tileCoord)
	{
		int x = tileCoord.x * tileSize.width + tileSize.width / 2;
		int y = (mapSize.height - tileCoord.y)*tileSize.height - tileSize.height / 2;
		return Vec2(x, y);
	}

	TMXTiledMap* tmx;//��ͼ�ļ�
private:
	Playground();
	TMXLayer* backGroundLayer;

	static Playground* playground;
	Size tileSize;
	Size mapSize;
	TMXObjectGroup* objects;//��ͼ����
};