#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include "Monster.h"

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

	//这里需要特别注意
	/***********************************************************************/
	/*这里差不多花了三个小时的时间
	主要解决，图片没有显示的问题，上网查了很多资料，没有一个有用的。
	有的说png图片不能用，有的说bmp图片不能用，我暂时用的jpg图片。
	据说新建层没有添加物体会奔溃，发现并没有，不知道网上的问题是怎么回事，全部都是错误答案。
	也可能是版本的问题，毕竟那个时候还是CC前缀还在的时候
	至于除去以上问题，画面还是没法显示，怎么解决？
	经过尝试，这种方法是可以的：
	一开始将所有要用到的资源放在Resources，不能直接把生成好的map.tmx放在Resources，
	否则没有显示，但是tmx ！= NULL。也不能将生成好的map.tmx先放在Resources下，再把用到
	的资源放在Resources，亲测不行。只能先把要用到的资源放在Resources下，我用到的资源是
	Test.jpg，然后打开TileMap，新建地图，保存地址也要保存到Resources，最好不要保存到其他位置，然后
	再拷贝到Resources，接下来导入图块，一定要用Resources下的图片，这样生成的图片才可以在Cocos中显示。*/
	/***********************************************************************/
	tmx = TMXTiledMap::create("test2.tmx");
	tmx->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	CCLOG("%f,%f", visibleSize.width / 2, visibleSize.height / 2);
	tmx->setAnchorPoint(Vec2(0.5, 0.5));
	tmx->setScale(Director::getInstance()->getContentScaleFactor());
	addChild(tmx, 0);
	mapSize = tmx->getMapSize();
	tileSize = tmx->getTileSize();
	backGroundLayer = tmx->getLayer("backGroundLayer");


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
	//每两秒生成一个怪物，并且移动
	//schedule(schedule_selector(HelloWorld::createMonster), 2.0f);
	//每0.1秒检测有没有被怪物攻击
	//schedule(schedule_selector(HelloWorld::hitByMonster), 0.1f);
	//每0.1秒检测主角有没有死
	//为什么要这么做，其实也是无奈的办法
	//因为如果死亡就要播放死亡动画，死亡动画结束后，取消所有动作，
	// 将死亡动画和取消所有动作放在一个函数中的时候，因为死亡动画是要时间的，函数会
	//直接执行，所以取消所有动作覆盖了死亡动画，导致没有播放，有人可能会依靠isDead来实现
	//先死亡再停止，但是本质没有改变，用一个序列来实现，序列结束的时候改变isDead的值，再用这个值
	//决定是否停止动画，同样，停止所有动画不会等isDead的值改变再执行。所有直接用调度器了。
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
	auto sequence = Sequence::create(Animate::create(AnimationCache::getInstance()->getAnimation("attack")),
		CCCallFunc::create(([this]() {
		isAnimating = false;
		isAttack = false;
	})), nullptr);
	player->runAction(sequence);
	setColor(skill1(tileCoordForPosition(player->getPosition())),Color3B(255,255,255));
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
		unschedule(schedule_selector(HelloWorld::createMonster));
		unschedule(schedule_selector(HelloWorld::hitByMonster));
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

//参考PPT
void HelloWorld::createMonster(float dt) 
{
	auto fac = Factory::getInstance();
	auto m = fac->createMonster();
	float x = random(origin.x, visibleSize.width);
	float y = random(origin.y, visibleSize.height);
	m->setPosition(x, y);
	addChild(m, 3);
	fac->moveMonster(player->getPosition(), 0.5f);
}

//参考PPT
void HelloWorld::hitByMonster(float dt) 
{
	auto fac = Factory::getInstance();
	Rect playerRect = player->getBoundingBox();
	Rect attackRect = Rect(playerRect.getMinX() - 40, playerRect.getMinY(), playerRect.getMaxX() - playerRect.getMinX() + 80, playerRect.getMaxY() - playerRect.getMinY());
	Sprite *attackCollision = fac->collider(attackRect);
	//这边注意判断的时候，是嵌套关系，不是并列关系
	//就是说不能将attackCollison和playerCollision放在并列关系判断，否则会出错
	if (attackCollision != NULL) {
		if (isAttack) {
			fac->removeMonster(attackCollision);
			float percentage = pT->getPercentage();
			if (percentage < 100) {
				auto to = ProgressFromTo::create(1.0f, percentage, percentage + 20);
				pT->runAction(to);
			}
			string str = score->getString();
			int scoreLength = atoi(str.c_str());
			scoreLength++;
			CCString* ns = CCString::createWithFormat("%d", scoreLength);
			string s = ns->_string;
			score->setString(s);
			//放在本地文件UserDefault.xml中
			database->setIntegerForKey("score",scoreLength);
			//这里又要吐槽一下网上的解答，说UserDefault.xml文件在Debug文件夹里面之类的
			//直接用下面的输出语句就可以看到地址，实现本地数据持久化
			CCLOG("%s", FileUtils::getInstance()->getWritablePath().c_str());
		}
		else {
			Sprite *playerCollision = fac->collider(playerRect);
			if (playerCollision != NULL) {
				fac->removeMonster(playerCollision);
				float percentage = pT->getPercentage();
				if (percentage > 0) {
					auto to = ProgressFromTo::create(1.0f, percentage, percentage - 20);
					pT->runAction(to);
				}
				else {
					deadCallback(NULL);
				}
			}
		}
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
	
	CCLOG("Player == %f,%f", player->getPosition().x, player->getPosition().y);
	CCLOG("OpenGL == %f,%f", tileCoordForPosition(player->getPosition()).x, tileCoordForPosition(player->getPosition()).y);
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

// OpenGL坐标转成格子坐标
Vec2 HelloWorld::tileCoordForPosition(const Vec2& position)
{
	CCLOG("ContentSize == %f",tmx->getContentSize().width);
	CCLOG("ContentSize == %f", tmx->getContentSize().height);

	int x = (position.x) / tileSize.width * 12/11;
	CCLOG("position.x = %f,tileSize.width = %f",position.x,tileSize.width);
	int y = (mapSize.height*tileSize.width - position.y) / tileSize.width * 8/7;
	return Vec2(x, y);
}
// tile坐标转成瓦片格子中心的OpenGL坐标
Vec2 HelloWorld::positionForTileCoord(const Vec2& tileCoord)
{
	int x = tileCoord.x * tileSize.width + tileSize.width / 2;
	int y = (mapSize.height - tileCoord.y)*tileSize.height - tileSize.height / 2;
	return Vec2(x, y);
}

std::vector<Vec2> HelloWorld::skill1(Vec2 input)
{
	std::vector<Vec2> skillArea;
	/*Vec2 wDirection1 = Vec2(input.y - 1, input.x);
	Vec2 wDirection2 = Vec2(input.y - 2, input.x);
	Vec2 sDirection1 = Vec2(input.y + 1, input.x);
	Vec2 sDirection2 = Vec2(input.y + 2, input.x);
	Vec2 aDirection1 = Vec2(input.y, input.x - 1);
	Vec2 aDirection2 = Vec2(input.y, input.x - 2);
	Vec2 dDirection1 = Vec2(input.y, input.x + 1);
	Vec2 dDirection2 = Vec2(input.y, input.x + 2);
	skillArea.push_back(wDirection1);
	skillArea.push_back(wDirection1);
	skillArea.push_back(wDirection1);
	skillArea.push_back(wDirection1);
	skillArea.push_back(wDirection1);
	skillArea.push_back(wDirection1);
	skillArea.push_back(wDirection1);
	skillArea.push_back(wDirection1);*/
	skillArea.push_back(input);
	return skillArea;
}

void HelloWorld::setColor(std::vector<Vec2> inputArea, Color3B inputColor) 
{
	if (inputArea.size() <= 0) {
		return;
	}
	CCLOG("tile1x = %f,tile1y = %f",inputArea.at(0).x,inputArea.at(0).y);
	auto tile1 = backGroundLayer->getTileAt(Point(inputArea.at(0).x,inputArea.at(0).y));
	JumpBy *jump = JumpBy::create(1.0f, Vec2(50, 1), 100, 1);
	tile1->runAction(jump);
}