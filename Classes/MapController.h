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
		//CCLOG("%f  ######%f", inputPosition.x, inputPosition.y);
		auto tile1 = backGroundLayer->getTileAt(Point(inputPosition.x, inputPosition.y));//��ȡָ������
		tile1->setColor(inputColor);//�ı���ɫ
	}
	bool setColor(Vec2 otherPlayerPosition,std::vector<Vec2> inputPosition, Color3B inputColor) {//�������������ı�����и��ӵ���ɫ,���Ҹ����Ƿ��ܹ���������truefalse
		//setColor(inputPosition.at(0), inputColor);
		bool temp = false;
		for (int i = 0; i < inputPosition.size(); i++) {
			if (isValid(inputPosition.at(i))) {//����λ�úϷ���ʱ���ɫ
				setColor(inputPosition.at(i), inputColor);
				if (inputPosition.at(i) == tileCoordForPosition(otherPlayerPosition)) {
					temp = true;
				}
			}
		}
		return temp;//�鿴�����Ƿ񱻹�������
	}
	Color3B getColor(Vec2 inputPosition) {//��ȡһ�����ӵ���ɫ
		auto tile1 = backGroundLayer->getTileAt(Point(inputPosition.x, inputPosition.y));//��ȡָ������
		return tile1->getColor();
	}
	// OpenGL����ת�ɸ�������
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
	bool isValid(Vec2 position) {//��������ж�һ�������Ƿ���ս��������
		if (position.x < 2.9 || position.y < 5.9 || position.x>14 || position.y>11) {
			//CCLOG("Refused%f %f", position.x, position.y);
			return false;
		}
		return true;
	}
	Size tileSize;
	Size mapSize;
	TMXTiledMap* tmx;//��ͼ�ļ�
private:
	Playground();
	TMXLayer* backGroundLayer;
	TMXLayer* battleGroundLayer;
	static Playground* playground;

	TMXObjectGroup* objects;//��ͼ����
};