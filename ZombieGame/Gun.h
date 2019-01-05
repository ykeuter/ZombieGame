//
//  Gun.h
//  ZombieGame
//
//  Created by Yvo Keuter on 9/14/15.
//  Copyright (c) 2015 blabla. All rights reserved.
//

#ifndef __ZombieGame__Gun__
#define __ZombieGame__Gun__

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <GameEngine/AudioEngine.h>

#include "Bullet.h"

class Gun
{
public:
    Gun(std::string name, int fireRate, int bulletsPerShot, float spread,
        float bulletDamage, float bulletSpeed, GameEngine::SoundEffect fireEffect);
    ~Gun();
    
    void update(bool isMouseDown, const glm::vec2& position, const glm::vec2& direction, std::vector<Bullet>& bullets, float deltaTime);
    
private:
    
    GameEngine::SoundEffect m_fireEffect;
    void fire(const glm::vec2& direction, const glm::vec2& position, std::vector<Bullet>& bullets);
    
    std::string _name;
    
    int _fireRate; ///< Fire rate in terms of frames
    
    int _bulletsPerShot; ///< How many bullets are fired at at time
    
    float _spread; ///< Accuracy
    
    float _bulletSpeed;
    
    int _bulletDamage;
    
    float _frameCounter; ///< Counts frames so we know when to shoot bullets
    
};

#endif /* defined(__ZombieGame__Gun__) */
