#include "cocos2d.h"
#include"MapController.h"
using namespace cocos2d;

class HelloWorld : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
        
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);
private:
	cocos2d::Sprite* player1;
	cocos2d::Sprite* player2;
	cocos2d::Vector<SpriteFrame*> attack;
	cocos2d::Vector<SpriteFrame*> dead;
	cocos2d::Vector<SpriteFrame*> run;
	cocos2d::Vector<SpriteFrame*> idle;
	cocos2d::Size visibleSize;
	cocos2d::Vec2 origin;
	cocos2d::Label* time;
	int dtime;
	cocos2d::ProgressTimer* pT;

	//判断是否在播放动作
	bool isAnimating;
	//攻击
	void attackCallback(Ref* pSender);
	//死亡
	void deadCallback(Ref* pSender);
	//重写update，实现倒计时
	void update(float time)override;

	//下面是新加的变量
	//用来显示打倒的敌人数
	cocos2d::Label* score;
	//判断是否在攻击
	bool isAttack = false;
	//判断是否在死亡
	bool isDead = false;
	//停止所有动作
	void stop(float time);


	void addKeyboardListener();
	void onKeyPressed(EventKeyboard::KeyCode code, Event* event);
	void onKeyReleased(EventKeyboard::KeyCode code, Event* event);
	bool isMove1;
	bool isMove2;
	char movekey1;
	char movekey2;
	void movePlayer1(char c);
	void movePlayer2(char c);
	void updateMove(float time);
	void attack1();
	void attack2();

	int attackWay1;//1是第一个技能，2是第二个技能，3是第三个技能
	int attackWay2;

	std::vector<Vec2> skill1(Vec2 temp);
	std::vector<Vec2> skill2(Vec2 temp);
	std::vector<Vec2> skill3(Vec2 temp);
	Playground * theMap;

	int player1Direction;//0是上，1是下，2是左，3是右
	int player2Direction;
};
