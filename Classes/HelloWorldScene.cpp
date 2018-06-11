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
	/*tmx = TMXTiledMap::create("test2.tmx");
	tmx->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	CCLOG("%f,%f", visibleSize.width / 2, visibleSize.height / 2);
	tmx->setAnchorPoint(Vec2(0.5, 0.5));
	tmx->setScale(Director::getInstance()->getContentScaleFactor());

	mapSize = tmx->getMapSize();
	tileSize = tmx->getTileSize();
	backGroundLayer = tmx->getLayer("backGroundLayer");*/
	addChild(theMap->tmx, 0);

	//创建一张贴图
	auto texture = Director::getInstance()->getTextureCache()->addImage("$lucia_2.png");
	//从贴图中以像素单位切割，创建关键帧
	auto frame0 = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(0, 0, 113, 113)));
	//使用第一帧创建精灵
	player = Sprite::createWithSpriteFrame(frame0);
	player->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height / 2));
	addChild(player, 3);

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

/*
实现攻击动作，攻击工作结束将isAnimating置为false
加分项是实现血条的变化
*/
/*
这次作业将血条和攻击分离了
*/
void HelloWorld::attackCallback(Ref * pSender)
{
	if (isDead == true) {
		return;
	}
	if (isAnimating == false) {
		isAnimating = true;
		isAttack = true;
		auto sequence = Sequence::create(Animate::create(AnimationCache::getInstance()->getAnimation("attack")),
			CCCallFunc::create(([this]() {
			isAnimating = false;
			isAttack = false;
		})), nullptr);
		player->runAction(sequence);
	}
}

void HelloWorld::attackA() {
	theMap = Playground::getInstance();
	auto sequence = Sequence::create(Animate::create(AnimationCache::getInstance()->getAnimation("attack")),
		CCCallFunc::create(([this]() {
		isAnimating = false;
		isAttack = false;
	})), nullptr);
	player->runAction(sequence);
	theMap->setColor(skill1(theMap->tileCoordForPosition(player->getPosition())),Color3B(139,0,0));
}
/*
死亡动作的实现
*/
/*
死亡之后，处理调度器
*/
void HelloWorld::deadCallback(Ref * pSender)
{
	if (isDead == false) {
		isAnimating = true;
		auto sequence = Sequence::create(Animate::create(AnimationCache::getInstance()->getAnimation("dead")),
			CCCallFunc::create(([this]() {
			isAnimating = false;
			isDead = true;
		})), nullptr);
		player->runAction(sequence);
		unschedule(schedule_selector(HelloWorld::update));
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
	if (isMove) {
		movePlayer(movekey);
	}
}


//停止所有动作
void HelloWorld::stop(float dt) 
{
	if (isDead == true) {
		player->stopAllActions();
	}
}


void HelloWorld::onKeyPressed(EventKeyboard::KeyCode code, Event* event) {
	switch (code) {
	case EventKeyboard::KeyCode::KEY_CAPITAL_A:
	case EventKeyboard::KeyCode::KEY_A:
		movekey = 'A';
		isMove = true;
		break;
	case EventKeyboard::KeyCode::KEY_CAPITAL_D:
	case EventKeyboard::KeyCode::KEY_D:
		movekey = 'D';
		isMove = true;
		break;
	case EventKeyboard::KeyCode::KEY_CAPITAL_S:
	case EventKeyboard::KeyCode::KEY_S:
		movekey = 'S';
		isMove = true;
		break;
	case EventKeyboard::KeyCode::KEY_CAPITAL_W:
	case EventKeyboard::KeyCode::KEY_W:
		movekey = 'W';
		isMove = true;
		break;
	case EventKeyboard::KeyCode::KEY_SPACE:
		attackA();
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
		isMove = false;
		break;
	}
}

void HelloWorld::addKeyboardListener() {
	// Todo
	auto keyBoardListener = EventListenerKeyboard::create();
	keyBoardListener->onKeyPressed = CC_CALLBACK_2(HelloWorld::onKeyPressed, this);
	keyBoardListener->onKeyReleased = CC_CALLBACK_2(HelloWorld::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(keyBoardListener, player);
}

void HelloWorld::movePlayer(char c) {
	theMap = Playground::getInstance();
	CCLOG("Player == %f,%f", player->getPosition().x, player->getPosition().y);
	CCLOG("OpenGL == %f,%f", theMap->tileCoordForPosition(player->getPosition()).x, theMap->tileCoordForPosition(player->getPosition()).y);
	if (c == 'A') {
		player->setFlippedX(true);
		if (player->getPosition().x > 0) {
			player->runAction(MoveBy::create(0.1f, Vec2(-10, 0)));
		}
	}
	else if (c == 'D') {
		player->setFlippedX(false);
		if (player->getPosition().x < visibleSize.width) {
			player->runAction(MoveBy::create(0.1f, Vec2(10, 0)));
		}
	}
	else if (c == 'W') {
		if (player->getPosition().y < visibleSize.height) {
			player->runAction(MoveBy::create(0.1f, Vec2(0, 10)));
		}
	}
	else if (c == 'S') {
		if (player->getPosition().y > 0) {
			player->runAction(MoveBy::create(0.1f, Vec2(0, -10)));
		}
	}
}



std::vector<Vec2> HelloWorld::skill1(Vec2 input)
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

