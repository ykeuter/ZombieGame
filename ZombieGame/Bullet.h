//
//  Bullet.h
//  ZombieGame
//
//  Created by Yvo Keuter on 9/14/15.
//  Copyright (c) 2015 blabla. All rights reserved.
//

#ifndef __ZombieGame__Bullet__
#define __ZombieGame__Bullet__

#include <glm/glm.hpp>
#include <vector>
#include <GameEngine/SpriteBatch.h>

class Human;
class Zombie;
class Agent;

const int BULLET_RADIUS = 5;

class Bullet
{
public:
    Bullet(glm::vec2 position, glm::vec2 direction, float damage, float speed);
    ~Bullet();
    
    // When update returns true, delete bullet
    bool update(const std::vector<std::string>& levelData, float deltaTime);
    
    void draw(GameEngine::SpriteBatch& spriteBatch);
    
    bool collideWithAgent(Agent* agent);
    
    float getDamage() const { return _damage; }
    
    glm::vec2 getPosition() const { return _position; }
    
private:
    
    bool collideWithWorld(const std::vector<std::string>& levelData);
    
    glm::vec2 _position;
    glm::vec2 _direction;
    float _damage;
    float _speed;
};

#endif /* defined(__ZombieGame__Bullet__) */
