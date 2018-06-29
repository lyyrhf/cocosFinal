#include "cocos2d.h"
#include "MapController.h"
#include "SimpleAudioEngine.h"
using namespace cocos2d;

class HelloWorld : public cocos2d::Scene
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
	cocos2d::Vector<SpriteFrame*> down;//���������ƶ��Ķ���֡
	cocos2d::Vector<SpriteFrame*> up;
	cocos2d::Vector<SpriteFrame*> left;
	cocos2d::Vector<SpriteFrame*> right;

	cocos2d::Size visibleSize;
	cocos2d::Vec2 origin;
	cocos2d::Label* time;
	int dtime;
	cocos2d::ProgressTimer* pT;


	void update(float time)override;

	//�������¼ӵı���
	//������ʾ�򵹵ĵ�����
	cocos2d::Label* score;
	//�ж��Ƿ��ڹ���
	//bool isAttack = false;
	//�ж��Ƿ�������
	//bool isDead = false;
	//ֹͣ���ж���
	//void stop(float time);


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

	int attackWay1;//1�ǵ�һ�����ܣ�2�ǵڶ������ܣ�3�ǵ���������
	int attackWay2;

	std::vector<Vec2> skill1(Vec2 temp);
	std::vector<Vec2> skill2(Vec2 temp);
	std::vector<Vec2> skill3(Vec2 temp);
	Playground * theMap;


	int player1Direction;//0���ϣ�1���£�2����3����
	int player2Direction;

	Vec2 currentPosition;
	Color3B currentColor3B=Color3B(255,255,255);

	bool isAttack1;
	bool isAttack2;

	void loadWindAttack();
	Vector<SpriteFrame*> windAnimation;
	void playWindAttack(cocos2d::Sprite* player);//��ʾ������˭�����ģ���ʾ����Ӧ���������
	
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
	void reducePlayer1Blood();//�����������������Ѫ���Ŀ��ƣ�ֻ���Լ��٣�����������


	bool isPlayer2Dead;
	int player2Blood;
	std::stack<Sprite*> player2BloodStack;
	void reducePlayer2Blood();

	PhysicsWorld* m_world;

	enum Tag { PLAYER1, PLAYER2 };
};
