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
};
