#include "cocos2d.h"
using namespace cocos2d;

class HelloWorld : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
        
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);
private:
	cocos2d::Sprite* player;
	cocos2d::Vector<SpriteFrame*> attack;
	cocos2d::Vector<SpriteFrame*> dead;
	cocos2d::Vector<SpriteFrame*> run;
	cocos2d::Vector<SpriteFrame*> idle;
	cocos2d::Size visibleSize;
	cocos2d::Vec2 origin;
	cocos2d::Label* time;
	int dtime;
	cocos2d::ProgressTimer* pT;

	//�ж��Ƿ��ڲ��Ŷ���
	bool isAnimating;
	//����
	void attackCallback(Ref* pSender);
	//����
	void deadCallback(Ref* pSender);
	//��дupdate��ʵ�ֵ���ʱ
	void update(float time)override;

	//�������¼ӵı���
	//������ʾ�򵹵ĵ�����
	cocos2d::Label* score;
	//�ж��Ƿ��ڹ���
	bool isAttack = false;
	//�ж��Ƿ�������
	bool isDead = false;
	//ֹͣ���ж���
	void stop(float time);


	void addKeyboardListener();
	void onKeyPressed(EventKeyboard::KeyCode code, Event* event);
	void onKeyReleased(EventKeyboard::KeyCode code, Event* event);
	bool isMove;
	char movekey;
	void movePlayer(char c);
	void updateMove(float time);
	void attackA();
	Vec2 tileCoordForPosition(const Vec2& position);
	Vec2 positionForTileCoord(const Vec2& tileCoord);
	cocos2d::TMXTiledMap* tmx;
	Size mapSize;
	Size tileSize;
	std::vector<Vec2> skill1(Vec2 temp);
	void setColor(std::vector<Vec2> inputArea,Color3B inputColor);
	cocos2d::TMXLayer* backGroundLayer;
};
