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

    shockwave = new EffectShockWave();
    shockwave->init();
    shockwave->setShockParam(Vec3(25.0, 1.5, 80.0));
    shockwave->setMaxRadius(250);
    shockwave->setSpeed(340);
    this->addChildEffect(shockwave);

    sprite = Sprite::create("test.jpg");
    sprite->setPosition(center.width / 2, center.height / 2);
    this->addChild(sprite);

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
    shockwave->setCenter(touch->getLocation());
    shockwave->setRepeat(1);

    return true;
}

void MainScene::onTouchMoved(Touch *touch, Event *uevent)
{

}

void MainScene::onTouchEnded(Touch *touch, Event *uevent)
{

}
