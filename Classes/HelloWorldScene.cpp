#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include "MapController.h"

//宏定义用于本地数据持久化
#define database UserDefault::getInstance()

//因为string要用，或者std::string也行
using namespace std;
#pragma execution_character_set("utf-8")

USING_NS_CC;

Scene* HelloWorld::createScene()
{
    return HelloWorld::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }

    visibleSize = Director::getInstance()->getVisibleSize();
    origin = Director::getInstance()->getVisibleOrigin();


	auto theMap =Playground::getInstance();

	addChild(theMap->tmx, 0);

	//创建一张贴图
	auto texture = Director::getInstance()->getTextureCache()->addImage("$lucia_2.png");
	//从贴图中以像素单位切割，创建关键帧
	auto frame0 = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(0, 0, 113, 113)));
	//使用第一帧创建精灵
	player1 = Sprite::createWithSpriteFrame(frame0);
	player1->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height / 2));
	addChild(player1, 3);
	player1->setAnchorPoint(Point(0.5,0.5));

	player2 = Sprite::createWithSpriteFrame(frame0);
	player2->setPosition(Vec2(origin.x + visibleSize.width / 2 + 200,
		origin.y + visibleSize.height / 2 + 50));
	addChild(player2, 3);
	player2->setAnchorPoint(Point(0.5, 0.5));

	//hp条
	Sprite* sp0 = Sprite::create("hp.png", CC_RECT_PIXELS_TO_POINTS(Rect(0, 320, 420, 47)));
	Sprite* sp = Sprite::create("hp.png", CC_RECT_PIXELS_TO_POINTS(Rect(610, 362, 4, 16)));

	//使用hp条设置progressBar
	pT = ProgressTimer::create(sp);
	pT->setScaleX(90);
	pT->setAnchorPoint(Vec2(0, 0));
	pT->setType(ProgressTimerType::BAR);
	pT->setBarChangeRate(Point(1, 0));
	pT->setMidpoint(Point(0, 1));
	pT->setPercentage(100);
	pT->setPosition(Vec2(origin.x + 14 * pT->getContentSize().width, origin.y + visibleSize.height - 2 * pT->getContentSize().height));
	addChild(pT, 1);
	sp0->setAnchorPoint(Vec2(0, 0));
	sp0->setPosition(Vec2(origin.x + pT->getContentSize().width, origin.y + visibleSize.height - sp0->getContentSize().height));
	addChild(sp0, 0);

	// 静态动画
	idle.reserve(1);
	idle.pushBack(frame0);

	// 攻击动画
	attack.reserve(17);
	for (int i = 0; i < 17; i++) {
		auto frame = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(113 * i, 0, 113, 113)));
		attack.pushBack(frame);
	}
	//这里又加入了frame0，原因是，动作结束之后要处理静止状态，而不是动作结束之后的状态
	attack.pushBack(frame0);
	auto attackAnimation = Animation::createWithSpriteFrames(attack, 0.1f);
	AnimationCache::getInstance()->addAnimation(attackAnimation, "attack");

	// 可以仿照攻击动画
	// 死亡动画(帧数：22帧，高：90，宽：79）
	auto texture2 = Director::getInstance()->getTextureCache()->addImage("$lucia_dead.png");
	// Todo
	dead.reserve(22);
	for (int i = 0; i < 22; i++) {
		auto frame = SpriteFrame::createWithTexture(texture2, CC_RECT_PIXELS_TO_POINTS(Rect(79 * i, 0, 79, 90)));
		dead.pushBack(frame);
	}
	//dead.pushBack(frame0);
	auto deadAnimation = Animation::createWithSpriteFrames(dead, 0.1f);
	AnimationCache::getInstance()->addAnimation(deadAnimation, "dead");

	// 运动动画(帧数：8帧，高：101，宽：68）
	auto texture3 = Director::getInstance()->getTextureCache()->addImage("$lucia_forward.png");
	// Todo
	run.reserve(8);
	for (int i = 0; i < 2; i++) {
		auto frame = SpriteFrame::createWithTexture(texture3, CC_RECT_PIXELS_TO_POINTS(Rect(68 * i, 0, 68, 101)));
		run.pushBack(frame);
	}
	run.pushBack(frame0);
	auto runAnimation = Animation::createWithSpriteFrames(run, 0.1f);
	AnimationCache::getInstance()->addAnimation(runAnimation, "run");

	auto menu = Menu::create();
	menu->setPosition(80, 50);
	addChild(menu);

	/*auto player3 = Sprite::create("07.png");
	player3->setPosition(360,240);
	addChild(player3);*/

	windAnimation = new Animation();
	char frameName[100] = { 0 };
	for (int i = 1; i<=12; i++) {
		sprintf(frameName, "%02d.png", i);
		windAnimation->addSpriteFrameWithFileName(frameName);
	}

	//CCActionInterval*  action = CCAnimate::actionWithDuration(1.0f, animation, false);

	//倒计时
	time = Label::createWithTTF("180", "fonts/arial.ttf", 36);
	time->setPosition(origin.x + visibleSize.width / 2, origin.y + visibleSize.height -50);
	addChild(time);

	score = Label::createWithTTF("0", "fonts/arial.ttf", 36);
	score->setPosition(origin.x + visibleSize.width / 2, origin.y + visibleSize.height - 100);
	addChild(score);

	addKeyboardListener();
	schedule(schedule_selector(HelloWorld::updateMove), 0.04f);
	//每一秒时间减少一
	schedule(schedule_selector(HelloWorld::update), 1.0f);
	schedule(schedule_selector(HelloWorld::stop), 0.1f);
    return true;
}

void HelloWorld::attack1() {
	if (isMove1 == false) {
		auto wind = Sprite::create("01.png");
		wind->setPosition(player1->getPosition());
		theMap = Playground::getInstance();
		isAttack1 = true;
		auto windAttack = Animate::create(windAnimation);
		windAttack->setDuration(1.0f);
		auto sequence = Sequence::create(/*Animate::create(AnimationCache::getInstance()->getAnimation("attack"))*/windAttack,
			CCCallFunc::create(([this]() {
			isAttack1 = false;
		})), nullptr);
		player1->runAction(sequence);
		player1->addChild(wind);
		wind->runAction(windAttack);
		attackWay1 = 3;

		if (attackWay1 == 1) {
			theMap->setColor(skill1(theMap->tileCoordForPosition(player1->getPosition())), Color3B(139, 0, 0));
		}
		else if (attackWay1 == 2) {
			theMap->setColor(skill2(theMap->tileCoordForPosition(player1->getPosition())), Color3B(139, 0, 0));
		}
		else if (attackWay1 == 3) {
			theMap->setColor(skill3(theMap->tileCoordForPosition(player1->getPosition())), Color3B(139, 0, 0));
		}
	}
}

void HelloWorld::attack2() {
	if (isMove2 == false) {
		isAttack2 = true;
		theMap = Playground::getInstance();
		auto sequence = Sequence::create(Animate::create(AnimationCache::getInstance()->getAnimation("attack")),
			CCCallFunc::create(([this]() {
			isAttack2 = false;
		})), nullptr);
		player2->runAction(sequence);
		attackWay2 = 3;
		//theMap->setColor(skill1(theMap->tileCoordForPosition(player->getPosition())),Color3B(139,0,0));
		if (attackWay2 == 1) {
			theMap->setColor(skill1(theMap->tileCoordForPosition(player2->getPosition())), Color3B(139, 0, 0));
		}
		else if (attackWay2 == 2) {
			theMap->setColor(skill2(theMap->tileCoordForPosition(player2->getPosition())), Color3B(139, 0, 0));
		}
		else if (attackWay2 == 3) {
			theMap->setColor(skill3(theMap->tileCoordForPosition(player2->getPosition())), Color3B(139, 0, 0));
		}
	}
}


/*
void HelloWorld::deadCallback(Ref * pSender)
{
	if (isDead == false) {
		isAnimating = true;
		auto sequence = Sequence::create(Animate::create(AnimationCache::getInstance()->getAnimation("dead")),
			CCCallFunc::create(([this]() {
			isAnimating = false;
			isDead = true;
		})), nullptr);
		player1->runAction(sequence);
		unschedule(schedule_selector(HelloWorld::update));
	}
}
*/

/*
update实现倒计时的功能，其中开始的值是180
需要每秒减一，所以需要拿到当前的值，并且减一再赋值回去
其中需要int和string的相互转换
由于是cocos2d，还有一个新的类型是CCString
这三者之间的转换参考下面链接
https://www.cnblogs.com/leehongee/p/3642308.html
*/

void HelloWorld::update(float dt)
{
	string str = time->getString();
	int timeLength = atoi(str.c_str());
	if (timeLength > 0) {
		timeLength--;
		CCString* ns = CCString::createWithFormat("%d", timeLength);
		string s = ns->_string;
		time->setString(s);
	}
	else {
		unschedule(schedule_selector(HelloWorld::update));
	}
}
void HelloWorld::updateMove(float dt) {
	if (isMove1) {
		movePlayer1(movekey1);
	}
	if (isMove2) {
		movePlayer2(movekey2);
	}
}


//停止所有动作
void HelloWorld::stop(float dt) 
{
	if (isDead == true) {
		player1->stopAllActions();
	}
}


void HelloWorld::onKeyPressed(EventKeyboard::KeyCode code, Event* event) {
	switch (code) {
	case EventKeyboard::KeyCode::KEY_CAPITAL_A:
	case EventKeyboard::KeyCode::KEY_A:
		movekey1 = 'A';
		isMove1 = true;
		break;
	case EventKeyboard::KeyCode::KEY_CAPITAL_D:
	case EventKeyboard::KeyCode::KEY_D:
		movekey1 = 'D';
		isMove1 = true;
		break;
	case EventKeyboard::KeyCode::KEY_CAPITAL_S:
	case EventKeyboard::KeyCode::KEY_S:
		movekey1 = 'S';
		isMove1 = true;
		break;
	case EventKeyboard::KeyCode::KEY_CAPITAL_W:
	case EventKeyboard::KeyCode::KEY_W:
		movekey1 = 'W';
		isMove1 = true;
		break;
	case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
		movekey2 = 'A';
		isMove2 = true;
		break;
	case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
		movekey2 = 'D';
		isMove2 = true;
		break;
	case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
		movekey2 = 'S';
		isMove2 = true;
		break;
	case EventKeyboard::KeyCode::KEY_UP_ARROW:
		movekey2 = 'W';
		isMove2 = true;
		break;
	case EventKeyboard::KeyCode::KEY_SPACE:
		attack1();
		break;
	case EventKeyboard::KeyCode::KEY_ENTER:
		attack2();
		break;
	}
}

void HelloWorld::onKeyReleased(EventKeyboard::KeyCode code, Event* event) {
	switch (code) {
	case EventKeyboard::KeyCode::KEY_A:
	case EventKeyboard::KeyCode::KEY_CAPITAL_A:
	case EventKeyboard::KeyCode::KEY_D:
	case EventKeyboard::KeyCode::KEY_CAPITAL_D:
	case EventKeyboard::KeyCode::KEY_S:
	case EventKeyboard::KeyCode::KEY_CAPITAL_S:
	case EventKeyboard::KeyCode::KEY_W:
	case EventKeyboard::KeyCode::KEY_CAPITAL_W:
		isMove1 = false;
		break;

	case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
	case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
	case EventKeyboard::KeyCode::KEY_UP_ARROW:
	case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
		isMove2 = false;
		break;
	}
}

void HelloWorld::addKeyboardListener() {
	// Todo
	auto keyBoardListener = EventListenerKeyboard::create();
	keyBoardListener->onKeyPressed = CC_CALLBACK_2(HelloWorld::onKeyPressed, this);
	keyBoardListener->onKeyReleased = CC_CALLBACK_2(HelloWorld::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(keyBoardListener, player1);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(keyBoardListener->clone(), player2);
}
//0是上，1是下，2是左，3是右
void HelloWorld::movePlayer1(char c) {
	if (isAttack1 == false) {
		theMap = Playground::getInstance();
		CCLOG("Player == %f,%f", player1->getPosition().x, player1->getPosition().y);
		CCLOG("OpenGL == %f,%f", theMap->tileCoordForPosition(player1->getPosition()).x, theMap->tileCoordForPosition(player1->getPosition()).y);
		auto test = player1->getPosition();
		if (c == 'A') {//测试这么走是否会出界
			test += Vec2(-20, 0);
		}
		else if (c == 'D') {
			test += Vec2(20, 0); 
		}
		else if (c == 'W') {
			test += Vec2(0, 20);
		}
		else if (c == 'S') {
			test += Vec2(0, -40);
		}
		if (!theMap->isValid(theMap->tileCoordForPosition(test))) {//如果会出界，则将不执行这个指令
			return;
		}

		if (c == 'A') {
			player1Direction = 2;
			player1->setFlippedX(true);
			if (player1->getPosition().x > 0) {
				player1->runAction(MoveBy::create(0.1f, Vec2(-10, 0)));
			}
		}
		else if (c == 'D') {
			player1Direction = 3;
			player1->setFlippedX(false);
			if (player1->getPosition().x < visibleSize.width) {
				player1->runAction(MoveBy::create(0.1f, Vec2(10, 0)));
			}
		}
		else if (c == 'W') {
			player1Direction = 0;
			if (player1->getPosition().y < visibleSize.height) {
				player1->runAction(MoveBy::create(0.1f, Vec2(0, 10)));
			}
		}
		else if (c == 'S') {
			player1Direction = 1;
			if (player1->getPosition().y > 0) {
				player1->runAction(MoveBy::create(0.1f, Vec2(0, -10)));
			}
		}
	}
	if (!currentPosition.equals(theMap->tileCoordForPosition(player1->getPosition()))){//当格子变动时
		CCLOG("tileChange %f %f", theMap->tileCoordForPosition(player1->getPosition()).x, theMap->tileCoordForPosition(player1->getPosition()).y);
		theMap->setColor(currentPosition, currentColor3B);//上一个格子恢复先前颜色
		currentPosition = theMap->tileCoordForPosition(player1->getPosition());//获得新的当前地址
		currentColor3B = theMap->getColor(currentPosition);//保存当前位置的颜色
		theMap->setColor(currentPosition, Color3B(100, 100, 100));//当前位置变成灰色
	}

}

void HelloWorld::movePlayer2(char c) {
	if (isAttack2 == false) {
		theMap = Playground::getInstance();
		CCLOG("Player == %f,%f", player1->getPosition().x, player1->getPosition().y);
		CCLOG("OpenGL == %f,%f", theMap->tileCoordForPosition(player1->getPosition()).x, theMap->tileCoordForPosition(player1->getPosition()).y + 2);
		if (c == 'A') {
			player2->setFlippedX(true);
			if (player2->getPosition().x > 0) {
				player2->runAction(MoveBy::create(0.1f, Vec2(-10, 0)));
			}
		}
		else if (c == 'D') {
			player2->setFlippedX(false);
			if (player2->getPosition().x < visibleSize.width) {
				player2->runAction(MoveBy::create(0.1f, Vec2(10, 0)));
			}
		}
		else if (c == 'W') {
			if (player2->getPosition().y < visibleSize.height) {
				player2->runAction(MoveBy::create(0.1f, Vec2(0, 10)));
			}
		}
		else if (c == 'S') {
			if (player2->getPosition().y > 0) {
				player2->runAction(MoveBy::create(0.1f, Vec2(0, -10)));
			}
		}
	}
}

std::vector<Vec2> HelloWorld::skill1(Vec2 input)//十字形攻击
{
	std::vector<Vec2> skillArea;
	Vec2 wDirection1 = Vec2(input.x, input.y - 1);
	Vec2 wDirection2 = Vec2(input.x, input.y - 2);
	Vec2 sDirection1 = Vec2(input.x, input.y + 1);
	Vec2 sDirection2 = Vec2(input.x, input.y + 2);
	Vec2 aDirection1 = Vec2(input.x - 1, input.y);
	Vec2 aDirection2 = Vec2(input.x - 2, input.y);
	Vec2 dDirection1 = Vec2(input.x + 1, input.y);
	Vec2 dDirection2 = Vec2(input.x + 2, input.y);
	skillArea.push_back(wDirection1);
	skillArea.push_back(wDirection2);
	skillArea.push_back(sDirection1);
	skillArea.push_back(sDirection2);
	skillArea.push_back(aDirection1);
	skillArea.push_back(aDirection2);
	skillArea.push_back(dDirection1);
	skillArea.push_back(dDirection2);
	skillArea.push_back(input);
	return skillArea;
}

std::vector<Vec2> HelloWorld::skill2(Vec2 input) //九宫格形攻击  
{
	std::vector<Vec2> skillArea;
	Vec2 nDirection = Vec2(input.x, input.y - 1);
	Vec2 wnDirection = Vec2(input.x-1, input.y - 1);
	Vec2 enDirection = Vec2(input.x+1, input.y - 1);
	Vec2 eDirection = Vec2(input.x+1, input.y);
	Vec2 esDirection = Vec2(input.x+1, input.y + 1);
	Vec2 wDirection = Vec2(input.x-1, input.y);
	Vec2 wsDirection = Vec2(input.x-1, input.y + 1);
	Vec2 sDirection = Vec2(input.x, input.y + 1);
	skillArea.push_back(nDirection);
	skillArea.push_back(wnDirection);
	skillArea.push_back(enDirection);
	skillArea.push_back(eDirection);
	skillArea.push_back(esDirection);
	skillArea.push_back(wDirection);
	skillArea.push_back(wsDirection);
	skillArea.push_back(sDirection);
	skillArea.push_back(input);
	return skillArea;
}

std::vector<Vec2> HelloWorld::skill3(Vec2 input)
{
	std::vector<Vec2> skillArea;
	Vec2 Direction1;
	Vec2 Direction2;
	Vec2 Direction3;
	Vec2 Direction4;
	if (player1Direction == 3) {
		Direction1 = Vec2(input.x + 1, input.y);
		Direction2 = Vec2(input.x + 2, input.y);
		Direction3 = Vec2(input.x + 3, input.y);
		Direction4 = Vec2(input.x + 4, input.y);
	}
	else if (player1Direction == 2) {
		Direction1 = Vec2(input.x - 1, input.y);
		Direction2 = Vec2(input.x - 2, input.y);
		Direction3 = Vec2(input.x - 3, input.y);
		Direction4 = Vec2(input.x - 4, input.y);
	}
	else if (player1Direction == 1) {
		Direction1 = Vec2(input.x, input.y + 1);
		Direction2 = Vec2(input.x, input.y + 2);
		Direction3 = Vec2(input.x, input.y + 3);
		Direction4 = Vec2(input.x, input.y + 4);
	}
	else if (player1Direction == 0) {
		Direction1 = Vec2(input.x, input.y - 1);
		Direction2 = Vec2(input.x, input.y - 2);
		Direction3 = Vec2(input.x, input.y - 3);
		Direction4 = Vec2(input.x, input.y - 4);
	}
	skillArea.push_back(Direction1);
	skillArea.push_back(Direction2);
	skillArea.push_back(Direction3);
	skillArea.push_back(Direction4);
	skillArea.push_back(input);
	return skillArea;
}
