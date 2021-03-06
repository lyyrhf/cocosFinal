#include "cocos2d.h"
#include "MapController.h"
#include "SimpleAudioEngine.h"
using namespace cocos2d;
USING_NS_CC;
class HelloWorld : public Layer
{
public:
	static PhysicsWorld* world;
    static cocos2d::Scene* createScene();
	void setPhysicsWorld(PhysicsWorld * world);
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
	cocos2d::Vector<SpriteFrame*> down;//上下左右移动的动画帧
	cocos2d::Vector<SpriteFrame*> up;
	cocos2d::Vector<SpriteFrame*> left;
	cocos2d::Vector<SpriteFrame*> right;

	cocos2d::Size visibleSize;
	cocos2d::Vec2 origin;
	cocos2d::Label* time;
	int dtime;
	cocos2d::ProgressTimer* pT;


	void update(float time)override;

	cocos2d::Label* gameText;


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
	std::vector<Vec2> skill4(Vec2 temp);
	Playground * theMap;


	int player1Direction;//0是上，1是下，2是左，3是右
	int player2Direction;

	//Vec2 currentPositionA;
	//Vec2 currentPositionB;
	Color3B currentColor3B=Color3B(255,255,255);
	Color3B playerAColor3B = Color3B(139, 0, 0);
	Color3B playerBColor3B = Color3B(0,139,0);
	bool isAttack1;
	bool isAttack2;

	void loadWindAttack();
	Vector<SpriteFrame*> windAnimation;
	void playWindAttack(cocos2d::Sprite* player);//表示技能是谁发出的，显示在相应的人物身边
	
	void loadFireAttack();
	Vector<SpriteFrame*> fireAnimation;
	void playFireAttack(cocos2d::Sprite* player);
	
	void loadDargonAttack();
	Vector<SpriteFrame*> dargonAnimation;
	void playDargonAttack(cocos2d::Sprite* player);

	void loadDead();
	void playDead(cocos2d::Sprite* player);
	Vector<SpriteFrame*> deadAnimation;

	bool isPlayer1Dead;
	int player1Blood;
	std::stack<Sprite*> player1BloodStack;
	void reducePlayer1Blood();//调用这个函数，进行血条的控制，只可以减少，不可以增加


	bool isPlayer2Dead;
	int player2Blood;
	std::stack<Sprite*> player2BloodStack;
	void reducePlayer2Blood();

	PhysicsWorld* m_world;
	bool beingAttackedA=false;
	bool beingAttackedB=false;
	void beingAttacked(Sprite* attacker, Sprite* beingAttacker);
	enum Tag { PLAYER1, PLAYER2,SKILL1,SKILL2,SKILL3 };
	bool onConcactBegin(PhysicsContact & contact);

	Sprite* item=NULL;
	void itemGenerate(float time);
	int kindOfItem;
	void replayCallback(Ref * pSender);
};
