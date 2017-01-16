//
//  MainScene.cpp
//  50Numbers
//
//  Created by Bui Tho on 10/4/16.
//
//

#include "MainScene.h"

Scene* MainScene::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = MainScene::create();
    
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

MainScene::MainScene()
{
    
}

MainScene::~MainScene()
{
    
}

bool MainScene::init()
{
    if(!LayerEffect::init())
    {
        return false;
    }

    /*
    x: Amplitude, y: Refraction , z: width (length of shockwave)
    */
    auto center = Director::getInstance()->getVisibleSize();
    
    //this->setContentSize(Size(center.width / 2, center.height / 2));    

    sprite = Sprite::create("test.jpg");
    sprite->setPosition(center.width / 2, center.height / 2);
    sprite->setScaleX(center.width / sprite->getContentSize().width);
    sprite->setScaleY(center.height / sprite->getContentSize().height);
    this->addChild(sprite);

    this->setScale(0.5);

    auto listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(MainScene::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(MainScene::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(MainScene::onTouchEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    
    scheduleUpdate();
    
    return true;
}

void MainScene::update(float dt)
{
    LayerEffect::update(dt);
}

bool MainScene::onTouchBegan(Touch *touch, Event *uevent)
{
    EffectShockWave *wave = new EffectShockWave();
    wave->init();
    wave->setShockParam(Vec3(20.0, 1.2, 70.0));
    wave->setMaxRadius(250);
    wave->setSpeed(130);
    wave->setCenter(touch->getLocation());
    wave->setRepeat(-1);
    this->addChildEffect(wave);
    
    CCLOG("size: %d", this->childrenEffect.size());

    return true;
}

void MainScene::onTouchMoved(Touch *touch, Event *uevent)
{

}

void MainScene::onTouchEnded(Touch *touch, Event *uevent)
{

}
