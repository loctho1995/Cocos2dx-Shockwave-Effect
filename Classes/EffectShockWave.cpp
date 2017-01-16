//
//  EffectShockWave.cpp
//  50Numbers
//
//  Created by Bui Tho on 9/29/16.
//
//

#include "EffectShockWave.h"

EffectShockWave* EffectShockWave::createEffectShockWave()
{
    EffectShockWave* effect = new EffectShockWave();

    if (effect && effect->init())
    {
        return effect;
    }

    return nullptr;
}

EffectShockWave::EffectShockWave(Vec2 center, float maxRadius , float speed)
{
    setCenter(center);
    setSpeed(speed);
    setMaxRadius(maxRadius);
    
    currentRadius = 0;
    
    finished = false;
    
    isSlowDown = true;
    
    setRepeat(-1);
}

EffectShockWave::~EffectShockWave()
{
    
}

void EffectShockWave::update(float dt)
{
    if(isRepeatForever)
    {
        updateShockWave(dt);
    }
    else
    {
        if(this->repeatTime > 0)
        {
            updateShockWave(dt);
        }
    }
}

void EffectShockWave::updateShockWave(float dt)
{
    currentRadius += (dt * speed);
    
    if(isSlowDown)
    {
        if(true) //currentRadius / maxRadius >= 0.5f
        {
            float singleStep = std::abs(1.0 - ((dt * speed) / maxRadius));
            
            speed = speed * singleStep;
            
            shockParam = shockParam * singleStep;
        }
    }
    
    if(currentRadius >= maxRadius)
    {
        currentRadius = 0;
        
        if(isRepeatForever == false)
        {
            if(this->repeatTime > 0)
                this->repeatTime--;
            
            if(repeatTime == 0)
            {
                this->finished = true;
                this->setEffectEnable(false);
                
                if(isSlowDown)
                {
                    this->speed = speedTemp;
                    this->shockParam = shockParamTemp;
                }
            }
        }
        else
        {
            this->speed = speedTemp;
            this->shockParam = shockParamTemp;
        }
    }
}

const char* EffectShockWave::getVertName()
{
    return "Shader/shockwave.vert";
}

const char* EffectShockWave::getFragName()
{
    return "Shader/shockwave.frag";
}

void EffectShockWave::setupUniforms()
{
    auto p = this->getGLProgramState();
    
    Vec2 resolution = Director::getInstance()->getVisibleSize();

    p->setUniformVec2("center", center);
    p->setUniformVec2("resolution", resolution);
    p->setUniformVec3("shockParams", shockParam);
    p->setUniformFloat("currentRadius", currentRadius);
    p->applyUniforms();
}

bool EffectShockWave::init()
{
    if(!EffectNode::init())
    {
        return false;
    }
    
    this->scheduleUpdate();
    
    return true;
}

void EffectShockWave::setRepeat(int time)
{
    finished = false;
    
    this->setEffectEnable(true);
    
    this->repeatTime = time;
    
    this->speed = speedTemp;
    
    this->shockParam = shockParamTemp;
    
    this->currentRadius = 0;
    
    isRepeatForever = false;
    
    if(time < 0)
    {
        isRepeatForever = true;
    }
    else
    {
        if(time == 0)
        {
            this->setEffectEnable(false);
        }
        
        this->repeatTime = time;
    }
}

void EffectShockWave::setShockParam(Vec3 shockParam)
{
    this->shockParam = shockParam;
    this->shockParamTemp = shockParam;
}