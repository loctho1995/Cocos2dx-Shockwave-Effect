//
//  EffectShockWave.h
//  50Numbers
//
//  Created by Bui Tho on 9/29/16.
//
//

#ifndef EffectShockWave_hpp
#define EffectShockWave_hpp

#include "cocos2d.h"
#include "EffectNode.h"

USING_NS_CC;

class EffectShockWave : public EffectNode
{
public:
    static EffectShockWave* createEffectShockWave();

    //center of shockwave, maximum radius of shockwave and speed of shockwave (pixel / sec)
    EffectShockWave(Vec2 center = Vec2::ZERO, float maxRadius = 1.0, float speed = 100.0);
    
    ~EffectShockWave();
    
    virtual const char* getVertName();
    
    virtual const char* getFragName();
    
    virtual void setupUniforms();
    
    virtual bool init();
    
    virtual void update(float dt);
    
    /*
     set the repeat time of shockwave
     < 0: forever
     > 0 will be repeat with the time you set
     */
    void setRepeat(int time);
    
    //set the center of shockwave, it will reset the shockwave after set center
    void setCenter(Vec2 center) {this->center = center; this->currentRadius = 0; }
    
    Vec2 getCenter() { return center; }
    
    //set shockwave's speed (pixel / s)
    void setSpeed(float speed) { this->speed = speed; this->speedTemp = speed; }
    
    float getSpeed() { return speed; }
    
    //set maxRadius of shockwave
    void setMaxRadius(float maxRadius){this->maxRadius = maxRadius;}
    
    float getCurrentRadius() {return this->currentRadius; }
    
    float getMaxRadius() {return this->maxRadius; }
    
    //x: Amplitude (Bien do), y: Refraction (do khuc xa), z: width (length of shockwave)
    void setShockParam(Vec3 shockParam);
    
    Vec3 getShockParam(){return this->shockParam; }
    
    bool isFinished() { return this->finished; }
    
    //do ripple effect with slow down every veriable
    void setSlowdown(bool flag) { this->isSlowDown = flag; }
    
protected:
    
    void updateShockWave(float dt);
    
    Vec2 center;
    
    float   speed,
            speedTemp,
            maxRadius,
            currentRadius;
    
    int     repeatTime,
            totalStep; //total step to done
    
    bool    isRepeatForever,
            finished,
            isSlowDown;
    
    Vec3 shockParam, shockParamTemp;
    
    std::vector<Vec2> listCenters;
};

#endif /* EffectShockWave_hpp */
