#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include "MapController.h"
#include "cocos2d.h"
//宏定义用于本地数据持久化
#define database UserDefault::getInstance()
#pragma execution_character_set("utf-8")

USING_NS_CC;
using namespace CocosDenshion;

void HelloWorld::setPhysicsWorld(PhysicsWorld* world) {
	m_world = world;
}


Scene* HelloWorld::createScene()
{
	auto scene = Scene::createWithPhysics();

	scene->getPhysicsWorld()->setAutoStep(true);

	scene->getPhysicsWorld()->setGravity(Vec2(0, -300.0f));
	auto layer = HelloWorld::create();
	layer->setPhysicsWorld(scene->getPhysicsWorld());
	scene->addChild(layer);
	return scene;
    //return HelloWorld::create();
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

	//CCLOG("visibleSize.width = %f",visibleSize.width);

	auto edgeSp = Sprite::create();  //创建一个精灵
	cocos2d::Size boundSize = Size(visibleSize.width - 160, visibleSize.height - 150);
	auto boundBody = PhysicsBody::createEdgeBox(boundSize, PhysicsMaterial(0.0f, 0.0f, 0.0f), 50);  //edgebox是不受刚体碰撞影响的一种刚体，我们用它来设置物理世界的边界
	edgeSp->setPosition(visibleSize.width / 2, visibleSize.height / 2 - 100);  //位置设置在屏幕中央
	edgeSp->setPhysicsBody(boundBody);
	addChild(edgeSp);

	CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("helloWorldSceneBGM.mp3");
	CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("walk.mp3");
	CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("walk.wav");


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


	player1Blood = 3;//设置初始血条
	player2Blood = 3;

	kindOfItem = 1;
	attackWay1 = 1;
	attackWay2 = 1;

	for (int i = 1; i <= player1Blood; i++) {
		auto blood = Sprite::create("blood.png");
		blood->setPosition(origin.x + 50 * i, origin.y + visibleSize.height * 9 / 10);
		blood->setScale(0.2);
		player1BloodStack.push(blood);
		addChild(blood, 1);
	}

	for (int i = 1; i <= player2Blood; i++) {
		auto blood = Sprite::create("blood.png");
		blood->setPosition(visibleSize.width - 50 * i, origin.y + visibleSize.height * 9 / 10);
		blood->setScale(0.2);
		player2BloodStack.push(blood);
		addChild(blood, 1);
	}

	//使用第一帧创建精灵
	player1 = Sprite::createWithSpriteFrame(frame[0]);

	auto playerBody1 = PhysicsBody::createBox(Size(50,50), PhysicsMaterial(100.0f, 0.0f, 0.0f));
	playerBody1->setCategoryBitmask(0xFFFFFFFF);
	playerBody1->setCollisionBitmask(0xFFFFFFFF);
	playerBody1->setContactTestBitmask(0xFFFFFFFF);
	playerBody1->setGravityEnable(false);
	playerBody1->setTag(Tag::PLAYER1);
	//playerBody1->setDynamic(false);
	player1->setPhysicsBody(playerBody1);
	player1->getPhysicsBody()->setRotationEnable(false);

	player1->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height / 2));
	addChild(player1, 3);
	player1->setAnchorPoint(Point(0.5,0.1));

	player2 = Sprite::createWithSpriteFrame(frame[0]);
	auto playerBody2 = PhysicsBody::createBox(Size(50,50), PhysicsMaterial(100.0f, 0.0f, 0.0f));
	playerBody2->setCategoryBitmask(0xFFFFFFFF);
	playerBody2->setCollisionBitmask(0xFFFFFFFF);
	playerBody2->setContactTestBitmask(0xFFFFFFFF);
	playerBody2->setGravityEnable(false);
	playerBody2->setTag(Tag::PLAYER2);
	//playerBody2->setDynamic(false);
	player2->setPhysicsBody(playerBody2);
	player2->getPhysicsBody()->setRotationEnable(false);

	player2->setPosition(Vec2(origin.x + visibleSize.width / 2 + 200,
		origin.y + visibleSize.height / 2 - 150));
	addChild(player2, 3);
	player2->setAnchorPoint(Point(0.5, 0.1));

	// 静态动画
	idle.reserve(1);
	idle.pushBack(frame[0]);//静止状态是第一帧

	//hp条
	/*
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
	*/



	auto menu = Menu::create();
	menu->setPosition(80, 50);
	addChild(menu);

	/*auto player3 = Sprite::create("07.png");
	player3->setPosition(360,240);
	addChild(player3);*/


	loadWindAttack();
	loadFireAttack();
	loadDargonAttack();
	loadDead();

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
//	schedule(schedule_selector(HelloWorld::stop), 0.1f);
	schedule(schedule_selector(HelloWorld::itemGenerate), 5.0f);

	auto contactListener = EventListenerPhysicsContact::create();
	contactListener->onContactBegin = CC_CALLBACK_1(HelloWorld::onConcactBegin, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);

    return true;
}
void HelloWorld::itemGenerate(float time){
	if (item != NULL) {
		return;
	}
	CCLOG("Generated2");
	theMap = Playground::getInstance();
	if (kindOfItem%3 == 1) {
		item = Sprite::create("dargonPic.png");
	}
	else if (kindOfItem%3 == 2) {
		item = Sprite::create("firePic.png");
	}
	else if (kindOfItem%3 == 0) {
		item = Sprite::create("windPic.png");
	}
	kindOfItem++;
	item->setScale(0.4f);

	auto itemBody = PhysicsBody::createBox(Size(50, 50), PhysicsMaterial(100.0f, 0.0f, 0.0f));
	itemBody->setCategoryBitmask(0xFFFFFFFF);
	itemBody->setCollisionBitmask(0xFFFFFFFF);
	itemBody->setContactTestBitmask(0xFFFFFFFF);
	itemBody->setGravityEnable(false);
	if (kindOfItem % 3 == 1) {
		itemBody->setTag(Tag::SKILL1);
	}
	else if (kindOfItem % 3 == 2) {
		itemBody->setTag(Tag::SKILL2);
	}
	else if (kindOfItem % 3 == 0) {
		itemBody->setTag(Tag::SKILL3);
	}
	item->setPhysicsBody(itemBody);
	item->getPhysicsBody()->setRotationEnable(false);

	CCLOG("Generated");
	Vec2* t = new Vec2(8, 8);
	Vec2 temp = theMap->positionForTileCoord(*t);
	item->setPosition(temp.x, temp.y);
	addChild(item, 3);
}
void HelloWorld::attack1() {//player1的攻击
	if (isMove1 == false && isAttack1 == false) {
		theMap = Playground::getInstance();
		isAttack1 = true;

		currentColor3B = Color3B(139,0,0);// theMap->getColor(currentPosition);//更新脚底下的Color!!!!!!!!!!!!!!!!!!!!!!!!!!!

		if (attackWay1 == 1) {
			playWindAttack(player1);
			beingAttackedB=theMap->setColor(player2->getPosition(),skill1(theMap->tileCoordForPosition(player1->getPosition())), playerAColor3B);
		}
		else if (attackWay1 == 2) {
			playDargonAttack(player1);
			beingAttackedB = theMap->setColor(player2->getPosition(),skill2(theMap->tileCoordForPosition(player1->getPosition())), playerAColor3B);
		}
		else if (attackWay1 == 3) {
			playFireAttack(player1);
			beingAttackedB = theMap->setColor(player2->getPosition(),skill3(theMap->tileCoordForPosition(player1->getPosition())), playerAColor3B);
		}
		//此处要触发后退、损血
		//player2->setPosition(534, 234);
		if(beingAttackedB)beingAttacked(player1, player2);
		//CCLOG("%d", beingAttackedB);
	}
}
void HelloWorld::beingAttacked(Sprite* attacker,Sprite* beingAttacker) {
	theMap = Playground::getInstance();
	Vec2 deltatemp= theMap->tileCoordForPosition(beingAttacker->getPosition()) - theMap->tileCoordForPosition(attacker->getPosition());//格坐标
	Vec2 delta;
	if (deltatemp.x > 0.5) {
		delta.x = 1;
	}else
	if (deltatemp.x < -0.5) {
		delta.x = -1;
	}
	else {
		delta.x = 0;
	}

	if (deltatemp.y > 0.5) {
		delta.y = 1;
	}
	else
	if (deltatemp.y < -0.5) {
			delta.y = -1;
		}
	else {
		delta.y = 0;
	}
	//Vec2 delta( deltatemp.x == 0 ? 0: 1, deltatemp.y == 0? 0: 1);
	//Vec2 delta.y = deltatemp.y == ? : 1;
	Vec2 toGo =theMap->tileCoordForPosition(beingAttacker->getPosition()) + delta;//格坐标
	if (theMap->isValid(toGo)) {
		beingAttacker->setPosition(theMap->positionForTileCoord(toGo));//前往真实坐标

		//CCLOG("AttackChenggong %f %f",toGo.x,toGo.y);
	}
	else {
		//CCLOG("Attackshibai%f %f", toGo.x, toGo.y);
	}
	if (beingAttacker == player2) {
		reducePlayer2Blood();
	}
	if (beingAttacker == player1) {
		reducePlayer1Blood();
	}
	//此处还要扣血模块！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！
}
void HelloWorld::attack2() {//player2的攻击
	if (isMove2 == false && isAttack2 == false) {
		isAttack2 = true;
		theMap = Playground::getInstance();
		//reducePlayer2Blood();
		//theMap->setColor(skill1(theMap->tileCoordForPosition(player->getPosition())),Color3B(139,0,0));
		if (attackWay2 == 1) {
			playWindAttack(player2);
			beingAttackedA=theMap->setColor(player1->getPosition(),skill1(theMap->tileCoordForPosition(player2->getPosition())), playerBColor3B);
		}
		else if (attackWay2 == 2) {
			playDargonAttack(player2);
			beingAttackedA = theMap->setColor(player1->getPosition(),skill2(theMap->tileCoordForPosition(player2->getPosition())), playerBColor3B);
		}
		else if (attackWay2 == 3) {
			playFireAttack(player2);
			beingAttackedA = theMap->setColor(player1->getPosition(),skill4(theMap->tileCoordForPosition(player2->getPosition())), playerBColor3B);
		}
		if (beingAttackedA)beingAttacked(player2, player1);
	}
}





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
	/*
	for (int i = 1; i <= player1Blood; i++) {
		auto blood = Sprite::create("blood.png");
		blood->setPosition(origin.x + 50 * i, origin.y + visibleSize.height * 9 / 10);
		blood->setScale(0.2);
		addChild(blood, 1);
	}

	for (int i = 1; i <= player2Blood; i++) {
		auto blood = Sprite::create("blood.png");
		blood->setPosition(visibleSize.width - 50 * i, origin.y + visibleSize.height * 9 / 10);
		blood->setScale(0.2);
		addChild(blood, 1);
	}
	*/
	std::string str = time->getString();
	int timeLength = atoi(str.c_str());
	if (timeLength > 0) {
		timeLength--;
		CCString* ns = CCString::createWithFormat("%d", timeLength);
		std::string s = ns->_string;
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
/*
void HelloWorld::stop(float dt) 
{
	if (isDead == true) {
		player1->stopAllActions();
	}
}*/


void HelloWorld::onKeyPressed(EventKeyboard::KeyCode code, Event* event) {
	switch (code) {
	case EventKeyboard::KeyCode::KEY_CAPITAL_A:
	case EventKeyboard::KeyCode::KEY_A:
		movekey1 = 'A';
		isMove1 = true;
		/*if (isAttack1 == false) {
			player1->getPhysicsBody()->setVelocity(Vec2(-500, 0));
		}*/
		break;
	case EventKeyboard::KeyCode::KEY_CAPITAL_D:
	case EventKeyboard::KeyCode::KEY_D:
		movekey1 = 'D';
		isMove1 = true;
		/*if (isAttack1 == false) {
			player1->getPhysicsBody()->setVelocity(Vec2(500, 0));
		}*/
		break;
	case EventKeyboard::KeyCode::KEY_CAPITAL_S:
	case EventKeyboard::KeyCode::KEY_S:
		movekey1 = 'S';
		isMove1 = true;
		/*if (isAttack1 == false) {
			player1->getPhysicsBody()->setVelocity(Vec2(0, -500));
		}*/
		break;
	case EventKeyboard::KeyCode::KEY_CAPITAL_W:
	case EventKeyboard::KeyCode::KEY_W:
		movekey1 = 'W';
		isMove1 = true;
		/*if (isAttack1 == false) {
			player1->getPhysicsBody()->setVelocity(Vec2(0, 500));
		}*/
		break;
	case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
		movekey2 = 'A';
		isMove2 = true;
		/*if (isAttack2 == false) {
			player2->getPhysicsBody()->setVelocity(Vec2(-500, 0));
		}*/
		break;
	case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
		movekey2 = 'D';
		isMove2 = true;
		/*if (isAttack2 == false) {
			player2->getPhysicsBody()->setVelocity(Vec2(500, 0));
		}*/
		break;
	case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
		movekey2 = 'S';
		isMove2 = true;
		/*if (isAttack2 == false) {
			player2->getPhysicsBody()->setVelocity(Vec2(0, -500));
		}*/
		break;
	case EventKeyboard::KeyCode::KEY_UP_ARROW:
		movekey2 = 'W';
		isMove2 = true;
		/*if (isAttack2 == false) {
			player2->getPhysicsBody()->setVelocity(Vec2(0, 500));
		}*/
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
		player1->getPhysicsBody()->setVelocity(Vec2(0, 0));
		break;

	case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
	case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
	case EventKeyboard::KeyCode::KEY_UP_ARROW:
	case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
		isMove2 = false;
		player2->getPhysicsBody()->setVelocity(Vec2(0, 0));
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
		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("walk.wav");
		theMap = Playground::getInstance();
		//CCLOG("Player == %f,%f", player1->getPosition().x, player1->getPosition().y);
		//CCLOG("OpenGL == %f,%f", theMap->tileCoordForPosition(player1->getPosition()).x, theMap->tileCoordForPosition(player1->getPosition()).y);
		Color3B tempColor=theMap->getColor(theMap->tileCoordForPosition(player1->getPosition()));
		float factor = 0.8;
		if (tempColor == playerAColor3B) {
			factor =1.2;
		}
		else if (tempColor == playerBColor3B) {
			factor = 0.5;
		}
		if (c == 'A') {
			player1Direction = 2;

			auto left = Animate::create(AnimationCache::getInstance()->getAnimation("left"));
			player1->runAction(left);
			player1->getPhysicsBody()->setVelocity(Vec2(-120*factor, 0));
		}
		else if (c == 'D') {
			player1Direction = 3;

			auto right = Animate::create(AnimationCache::getInstance()->getAnimation("right"));
			player1->runAction(right);
			player1->getPhysicsBody()->setVelocity(Vec2(120 * factor, 0));
		}
		else if (c == 'W') {
			player1Direction = 0;

			auto up = Animate::create(AnimationCache::getInstance()->getAnimation("up"));
			player1->runAction(up);
			player1->getPhysicsBody()->setVelocity(Vec2(0, 120 * factor));
		}
		else if (c == 'S') {
			player1Direction = 1;

			auto down = Animate::create(AnimationCache::getInstance()->getAnimation("down"));
			player1->runAction(down);
			player1->getPhysicsBody()->setVelocity(Vec2(0,-120 * factor));
		}
	}
	/*if (!currentPositionA.equals(theMap->tileCoordForPosition(player1->getPosition()))){//当格子变动时
		CCLOG("tileChange %f %f", theMap->tileCoordForPosition(player1->getPosition()).x, theMap->tileCoordForPosition(player1->getPosition()).y);
		theMap->setColor(currentPositionA, currentColor3B);//上一个格子恢复先前颜色
		currentPositionA = theMap->tileCoordForPosition(player1->getPosition());//获得新的当前地址
		currentColor3B = theMap->getColor(currentPositionA);//保存当前位置的颜色
		theMap->setColor(currentPositionA, Color3B(100, 100, 100));//当前位置变成灰色
	}*/

}
void HelloWorld::movePlayer2(char c) {
	if (isAttack2 == false) {
		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("walk.wav");
		theMap = Playground::getInstance();
		//CCLOG("Player == %f,%f", player2->getPosition().x, player2->getPosition().y);
		//CCLOG("OpenGL == %f,%f", theMap->tileCoordForPosition(player2->getPosition()).x, theMap->tileCoordForPosition(player2->getPosition()).y);
		Color3B tempColor = theMap->getColor(theMap->tileCoordForPosition(player2->getPosition()));
		float factor = 0.8;
		if (tempColor == playerBColor3B) {
			factor = 1.2;
		}
		else if (tempColor == playerAColor3B) {
			factor = 0.5;
		}
		if (c == 'A') {
			player2Direction = 2;

			auto left = Animate::create(AnimationCache::getInstance()->getAnimation("left"));
			player2->runAction(left);
			player2->getPhysicsBody()->setVelocity(Vec2(-120 * factor, 0));
		}
		else if (c == 'D') {
			player2Direction = 3;

			auto right = Animate::create(AnimationCache::getInstance()->getAnimation("right"));
			player2->runAction(right);
			player2->getPhysicsBody()->setVelocity(Vec2(120 * factor, 0));
		}
		else if (c == 'W') {
			player2Direction = 0;

			auto up = Animate::create(AnimationCache::getInstance()->getAnimation("up"));
			player2->runAction(up);
			player2->getPhysicsBody()->setVelocity(Vec2(0, 120 * factor));
		}
		else if (c == 'S') {
			player2Direction = 1;

			auto down = Animate::create(AnimationCache::getInstance()->getAnimation("down"));
			player2->runAction(down);
			player2->getPhysicsBody()->setVelocity(Vec2(0, -120 * factor));
		}
	}
/*	if (!currentPositionB.equals(theMap->tileCoordForPosition(player2->getPosition()))) {//当格子变动时
		CCLOG("tileChange %f %f", theMap->tileCoordForPosition(player2->getPosition()).x, theMap->tileCoordForPosition(player2->getPosition()).y);
		theMap->setColor(currentPositionB, currentColor3B);//上一个格子恢复先前颜色
		currentPositionB = theMap->tileCoordForPosition(player2->getPosition());//获得新的当前地址
		currentColor3B = theMap->getColor(currentPositionB);//保存当前位置的颜色
		theMap->setColor(currentPositionB, Color3B(100, 100, 100));//当前位置变成灰色
	}*/

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
std::vector<Vec2> HelloWorld::skill4(Vec2 input)
{
	std::vector<Vec2> skillArea;
	Vec2 Direction1;
	Vec2 Direction2;
	Vec2 Direction3;
	Vec2 Direction4;
	if (player2Direction == 3) {
		Direction1 = Vec2(input.x + 1, input.y);
		Direction2 = Vec2(input.x + 2, input.y);
		Direction3 = Vec2(input.x + 3, input.y);
		Direction4 = Vec2(input.x + 4, input.y);
	}
	else if (player2Direction == 2) {
		Direction1 = Vec2(input.x - 1, input.y);
		Direction2 = Vec2(input.x - 2, input.y);
		Direction3 = Vec2(input.x - 3, input.y);
		Direction4 = Vec2(input.x - 4, input.y);
	}
	else if (player2Direction == 1) {
		Direction1 = Vec2(input.x, input.y + 1);
		Direction2 = Vec2(input.x, input.y + 2);
		Direction3 = Vec2(input.x, input.y + 3);
		Direction4 = Vec2(input.x, input.y + 4);
	}
	else if (player2Direction == 0) {
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
	CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("wind.wav");
	char frameName[100] = { 0 };
	for (int i = 1; i <= 12; i++) {
		sprintf(frameName, "%02d.png", i);
		SpriteFrame* pngNameSF = SpriteFrame::create(frameName, Rect(0, 0, 300, 300));
		windAnimation.pushBack(pngNameSF);
	}
}

void HelloWorld::playWindAttack(cocos2d::Sprite* player) {
	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("wind.wav");
	auto skill = Sprite::create("01.png");
	skill->setScale(3);
	skill->setAnchorPoint(Point(0.23, 0.5));
	skill->setPosition(player->getPosition());
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
		isAttack2 = false;
	})), nullptr);
	addChild(skill);
	skill->runAction(sequence);
	/*auto jumpBy1 = JumpBy::create(0.7f, Point(0, 50), 50, 1);
	auto jumpBy2 = JumpBy::create(0.3f, Point(0, -50), -50, 1);
	auto sequence1 = Sequence::create(
		jumpBy1,
		jumpBy2,
		nullptr
	);
	player1->runAction(sequence1);*/
}

void HelloWorld::loadFireAttack()
{
	CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("fire.mp3");
	char frameName[100] = { 0 };
	for (int i = 1; i <= 12; i++) {
		sprintf(frameName, "fire%02d.png", i);
		SpriteFrame* pngNameSF = SpriteFrame::create(frameName, Rect(0, 0, 300, 300));
		fireAnimation.pushBack(pngNameSF);
	}
}

void HelloWorld::playFireAttack(cocos2d::Sprite* player) {
	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("fire.mp3");
	auto skill = Sprite::create("fire01.png");
	skill->setScale(4);
	skill->setAnchorPoint(Point(0.1, 0.8));
	skill->setPosition(player->getPosition());
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
		isAttack2 = false;
	})), nullptr);
	addChild(skill);
	skill->runAction(sequence);
}

void HelloWorld::loadDargonAttack()
{
	CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("dargon.wav");
	char frameName[100] = { 0 };
	for (int i = 1; i <= 12; i++) {
		sprintf(frameName, "dargon%02d.png", i);
		SpriteFrame* pngNameSF = SpriteFrame::create(frameName, Rect(0, 0, 300, 300));
		dargonAnimation.pushBack(pngNameSF);
	}
}

void HelloWorld::playDargonAttack(cocos2d::Sprite* player) {
	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("dargon.wav");
	auto skill = Sprite::create("dargon01.png");
	skill->setAnchorPoint(Point(0.2, 0.2));
	skill->setPosition(player->getPosition());
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
		isAttack2 = false;
	})), nullptr);
	addChild(skill);
	skill->runAction(sequence);
}


void HelloWorld::loadDead() {
	CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("dead.wav");
	char frameName[100] = { 0 };
	for (int i = 1; i <= 5; i++) {
		sprintf(frameName, "dead%02d.png", i);
		SpriteFrame* pngNameSF = SpriteFrame::create(frameName, Rect(0, 0, 300, 300));
		deadAnimation.pushBack(pngNameSF);
	}
}

void HelloWorld::playDead(cocos2d::Sprite* player) 
{
	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("dead.wav");
	auto skillFadeOut = FadeOut::create(0.5);
	auto callBackRemove = CallFunc::create([this, &player]() {
		this->removeChild(player);
	});
	auto dead = Animation::createWithSpriteFrames(deadAnimation, 0.1f);
	auto action = Animate::create(dead);
	auto sequence = Sequence::create(
		action,
		skillFadeOut,
		callBackRemove,
		CCCallFunc::create(([this]() {
		isAttack1 = false;
		isAttack2 = false;
	})), nullptr);
	//addChild(skill);
	player->runAction(sequence);
	_eventDispatcher->removeAllEventListeners();
	CocosDenshion::SimpleAudioEngine::getInstance()->stopBackgroundMusic();
}

void HelloWorld::reducePlayer1Blood()
{
	if (player1BloodStack.empty()) {
		return;
	}
	auto temp = player1BloodStack.top();
	temp->removeFromParentAndCleanup(true);
	player1BloodStack.pop();
}

void HelloWorld::reducePlayer2Blood() 
{
	if (player2BloodStack.empty()) {
		return;
	}
	auto temp = player2BloodStack.top();
	temp->removeFromParentAndCleanup(true);
	player2BloodStack.pop();
}

bool HelloWorld::onConcactBegin(PhysicsContact & contact) {
	//CCLOG("yes");
	auto c1 = contact.getShapeA()->getBody(), c2 = contact.getShapeB()->getBody();
	auto s1 = (Sprite*)c1->getNode(), s2 = (Sprite*)c2->getNode();
	
	if (c1->getTag() == Tag::PLAYER1 && c2->getTag() == Tag::PLAYER2) {
		s1->getPhysicsBody()->setVelocity(Vec2(0, 0));
		s2->getPhysicsBody()->setVelocity(Vec2(0, 0));
	}
	if (c1->getTag() == Tag::PLAYER1 && c2->getTag() == Tag::SKILL1) {
		s2->removeFromParentAndCleanup(true);
		attackWay1 = 1;
		item = NULL;
	}
	else if (c1->getTag() == Tag::PLAYER1 && c2->getTag() == Tag::SKILL1) {
		s2->removeFromParentAndCleanup(true);
		attackWay1 = 1;
		item = NULL;
	}
	else if (c1->getTag() == Tag::PLAYER1 && c2->getTag() == Tag::SKILL2) {
		s2->removeFromParentAndCleanup(true);
		attackWay1 = 2;
		item = NULL;
	}
	else if (c1->getTag() == Tag::PLAYER1 && c2->getTag() == Tag::SKILL3) {
		s2->removeFromParentAndCleanup(true);
		attackWay1 = 3;
		item = NULL;
	}
	else if (c1->getTag() == Tag::PLAYER2 && c2->getTag() == Tag::SKILL1) {
		s2->removeFromParentAndCleanup(true);
		attackWay2 = 1;
		item = NULL;
	}
	else if (c1->getTag() == Tag::PLAYER2 && c2->getTag() == Tag::SKILL2) {
		s2->removeFromParentAndCleanup(true);
		attackWay2 = 2;
		item = NULL;
	}
	else if (c1->getTag() == Tag::PLAYER2 && c2->getTag() == Tag::SKILL3) {
		s2->removeFromParentAndCleanup(true);
		attackWay2 = 3;
		item = NULL;
	}
	if (item != NULL) {
		CCLOG("item != NULL");
	}
	return true;
}
