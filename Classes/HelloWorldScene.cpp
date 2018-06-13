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

	//创建一张贴图(player1的)
	auto texture = Director::getInstance()->getTextureCache()->addImage("player1.png");
	//从贴图中以像素单位切割，创建关键帧
	SpriteFrame* frame[16];// = new frame[15];
	for (int i = 0; i < 4; ++i) {
		frame[4 * i] = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(0, i * 100, 100, 100)));
		frame[4 * i+1] = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(100, i*100, 100, 100)));
		frame[4 * i+2] = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(200, i*100, 100, 100)));
		frame[4 * i+3] = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(300, i*100, 100, 100)));
		if (i == 0) {//向下走
			down.reserve(4);
			down.pushBack(frame[4 * i]);
			down.pushBack(frame[4 * i + 1]);
			down.pushBack(frame[4 * i + 2]);
			down.pushBack(frame[4 * i + 3]);

			auto downAnimation = Animation::createWithSpriteFrames(down, 0.1f);
			AnimationCache::getInstance()->addAnimation(downAnimation, "down");
		}else
		if (i == 1) {//向左走
			left.reserve(4);
			left.pushBack(frame[4 * i]);
			left.pushBack(frame[4 * i + 1]);
			left.pushBack(frame[4 * i + 2]);
			left.pushBack(frame[4 * i + 3]);

			auto leftAnimation = Animation::createWithSpriteFrames(left, 0.1f);
			AnimationCache::getInstance()->addAnimation(leftAnimation, "left");
		}else
		if (i == 2) {//向右走
			right.reserve(4);
			right.pushBack(frame[4 * i]);
			right.pushBack(frame[4 * i + 1]);
			right.pushBack(frame[4 * i + 2]);
			right.pushBack(frame[4 * i + 3]);

			auto rightAnimation = Animation::createWithSpriteFrames(right, 0.1f);
			AnimationCache::getInstance()->addAnimation(rightAnimation, "right");
		}else
		if (i == 3) {//向上走
			up.reserve(4);
			up.pushBack(frame[4 * i]);
			up.pushBack(frame[4 * i + 1]);
			up.pushBack(frame[4 * i + 2]);
			up.pushBack(frame[4 * i + 3]);

			auto upAnimation = Animation::createWithSpriteFrames(up, 0.1f);
			AnimationCache::getInstance()->addAnimation(upAnimation, "up");
		}
	}// 行走动画创建完成
	//auto frame0 = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(0, 0, 100, 100)));


	//使用第一帧创建精灵
	player1 = Sprite::createWithSpriteFrame(frame[0]);
	player1->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height / 2));
	addChild(player1, 3);
	player1->setAnchorPoint(Point(0.5,0.5));

	player2 = Sprite::createWithSpriteFrame(frame[0]);
	player2->setPosition(Vec2(origin.x + visibleSize.width / 2 + 200,
		origin.y + visibleSize.height / 2 + 50));
	addChild(player2, 3);
	player2->setAnchorPoint(Point(0.5, 0.5));

	// 静态动画
	idle.reserve(1);
	idle.pushBack(frame[0]);//静止状态是第一帧

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




	auto menu = Menu::create();
	menu->setPosition(80, 50);
	addChild(menu);

	/*auto player3 = Sprite::create("07.png");
	player3->setPosition(360,240);
	addChild(player3);*/


	loadWindAttack();
	loadFireAttack();
	loadDargonAttack();

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
		theMap = Playground::getInstance();
		isAttack1 = true;
		
		//playWindAttack();
		//playFireAttack();
		playDargonAttack();
		currentColor3B = theMap->getColor(currentPosition);//更新脚底下的Color
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


			auto spawn = Spawn::createWithTwoActions(Animate::create(
				AnimationCache::getInstance()->getAnimation("left")),
				MoveBy::create(0.1f, Vec2(-10, 0)));
			auto sequence = Sequence::create(spawn, CCCallFunc::create(([this]() {
				
			})), nullptr);
			player1->runAction(sequence);
		}
		else if (c == 'D') {
			player1Direction = 3;

			
			auto spawn = Spawn::createWithTwoActions(Animate::create(
				AnimationCache::getInstance()->getAnimation("right")),
				MoveBy::create(0.1f, Vec2(10, 0)));
			auto sequence = Sequence::create(spawn, CCCallFunc::create(([this]() {

			})), nullptr);
			player1->runAction(sequence);
		}
		else if (c == 'W') {
			player1Direction = 0;

			auto spawn = Spawn::createWithTwoActions(Animate::create(
				AnimationCache::getInstance()->getAnimation("up")),
				MoveBy::create(0.1f, Vec2(0, 10)));
			auto sequence = Sequence::create(spawn, CCCallFunc::create(([this]() {

			})), nullptr);
			player1->runAction(sequence);
		}
		else if (c == 'S') {
			player1Direction = 1;

			auto spawn = Spawn::createWithTwoActions(Animate::create(
				AnimationCache::getInstance()->getAnimation("down")),
				MoveBy::create(0.1f, Vec2(0, -10)));
			auto sequence = Sequence::create(spawn, CCCallFunc::create(([this]() {

			})), nullptr);
			player1->runAction(sequence);
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


void HelloWorld::loadWindAttack() 
{
	char frameName[100] = { 0 };
	for (int i = 1; i <= 12; i++) {
		sprintf(frameName, "%02d.png", i);
		SpriteFrame* pngNameSF = SpriteFrame::create(frameName, Rect(0, 0, 300, 300));
		windAnimation.pushBack(pngNameSF);
	}
}

void HelloWorld::playWindAttack() {
	auto skill = Sprite::create("01.png");
	skill->setAnchorPoint(Point(0.4, 0.5));
	skill->setPosition(player1->getPosition());
	auto skillFadeOut = FadeOut::create(0.5);
	auto callBackRemove = CallFunc::create([this, &skill]() {
		this->removeChild(skill);
	});
	auto wind = Animation::createWithSpriteFrames(windAnimation, 0.1f);
	auto action = Animate::create(wind);
	auto sequence = Sequence::create(
		action,
		skillFadeOut,
		callBackRemove,
		CCCallFunc::create(([this]() {
		isAttack1 = false;
	})), nullptr);
	addChild(skill);
	skill->runAction(sequence);
	auto jumpBy1 = JumpBy::create(0.7f, Point(0, 50), 50, 1);
	auto jumpBy2 = JumpBy::create(0.3f, Point(0, -50), -50, 1);
	auto sequence1 = Sequence::create(
		jumpBy1,
		jumpBy2,
		nullptr
	);
	player1->runAction(sequence1);
}

void HelloWorld::loadFireAttack()
{
	char frameName[100] = { 0 };
	for (int i = 0; i < 8; i++) {
		sprintf(frameName, "fire%d.png", i);
		SpriteFrame* pngNameSF = SpriteFrame::create(frameName, Rect(0, 0, 300, 300));
		fireAnimation.pushBack(pngNameSF);
	}
}

void HelloWorld::playFireAttack() {
	auto skill = Sprite::create("fire1.png");
	skill->setAnchorPoint(Point(0, 0.8));
	skill->setPosition(player1->getPosition());
	auto skillFadeOut = FadeOut::create(0.5);
	auto callBackRemove = CallFunc::create([this, &skill]() {
		this->removeChild(skill);
	});
	auto fire = Animation::createWithSpriteFrames(fireAnimation, 0.1f);
	auto action = Animate::create(fire);
	auto sequence = Sequence::create(
		action,
		skillFadeOut,
		callBackRemove,
		CCCallFunc::create(([this]() {
		isAttack1 = false;
	})), nullptr);
	addChild(skill);
	skill->runAction(sequence);
}

void HelloWorld::loadDargonAttack()
{
	char frameName[100] = { 0 };
	for (int i = 1; i <= 19; i++) {
		sprintf(frameName, "dargon%02d.png", i);
		SpriteFrame* pngNameSF = SpriteFrame::create(frameName, Rect(0, 0, 300, 300));
		dargonAnimation.pushBack(pngNameSF);
	}
}

void HelloWorld::playDargonAttack() {
	auto skill = Sprite::create("dargon01.png");
	skill->setAnchorPoint(Point(0.2, 0.2));
	skill->setPosition(player1->getPosition());
	auto skillFadeOut = FadeOut::create(0.5);
	auto callBackRemove = CallFunc::create([this, &skill]() {
		this->removeChild(skill);
	});
	auto dargon = Animation::createWithSpriteFrames(dargonAnimation, 0.1f);
	auto action = Animate::create(dargon);
	auto sequence = Sequence::create(
		action,
		skillFadeOut,
		callBackRemove,
		CCCallFunc::create(([this]() {
		isAttack1 = false;
	})), nullptr);
	addChild(skill);
	skill->runAction(sequence);
}