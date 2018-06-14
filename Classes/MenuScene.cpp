#include "MenuScene.h"
#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

Scene* MenuScene::createScene()
{
	return MenuScene::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
	printf("Error while loading: %s\n", filename);
	printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool MenuScene::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Scene::init())
	{
		return false;
	}

	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto backGround = Sprite::create("menuSceneBackground.jpeg");
	backGround->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
	this->addChild(backGround, 0);

	

	/*auto bg = Sprite::create("menu-background.png");
	bg->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y - 60));
	this->addChild(bg, 0);

	auto miner = Sprite::create("menu-miner.png");
	miner->setPosition(Vec2(150 + origin.x, visibleSize.height / 2 + origin.y - 60));
	this->addChild(miner, 1);

	auto leg = Sprite::createWithSpriteFrameName("miner-leg-0.png");
	Animate* legAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("legAnimation"));
	leg->runAction(RepeatForever::create(legAnimate));
	leg->setPosition(110 + origin.x, origin.y + 102);
	this->addChild(leg, 1);
	//从这里开始，基本操作
	auto goldMinerText = Sprite::create("gold-miner-text.png");
	goldMinerText->setPosition(Vect(origin.x + visibleSize.width / 2, origin.y + visibleSize.height - goldMinerText->getContentSize().height));
	this->addChild(goldMinerText);

	auto menuStartGold = Sprite::create("menu-start-gold.png");
	menuStartGold->setPosition(Vec2(origin.x + visibleSize.width - menuStartGold->getContentSize().width / 2, origin.y + menuStartGold->getContentSize().width / 2));
	this->addChild(menuStartGold, 0);
	//菜单项，有一个点击调用函数，replaceScene，后续的init会直接调用*/

	auto startItem = MenuItemImage::create(
		"startGame01.png",
		"startGame02.png",
		CC_CALLBACK_1(MenuScene::menuStartCallBack, this));
	//startItem->setPosition(Vec2(visibleSize.width/2 + origin.x, origin.y + visibleSize.height/2));
	startItem->setPosition(Vec2::ZERO);
	auto menu = Menu::create(startItem, NULL);
	menu->setPosition(Vec2(visibleSize.width / 2 + origin.x, origin.y + visibleSize.height / 2));
	addChild(menu, 1);

	return true;
}

void MenuScene::menuStartCallBack(Ref* pSender) {
	Director::getInstance()->replaceScene(HelloWorld::createScene());
}
