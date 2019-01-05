//
//  Human.cpp
//  ZombieGame
//
//  Created by Yvo Keuter on 9/14/15.
//  Copyright (c) 2015 blabla. All rights reserved.
//

#include "Human.h"
#include <ctime>
#include <random>
#include <glm/gtx/rotate_vector.hpp>
#include <GameEngine/ResourceManager.h>

Human::Human() :
_frames(0)
{
}


Human::~Human()
{
}

void Human::init(float speed, glm::vec2 pos) {
    
    static std::mt19937 randomEngine((unsigned int)time(nullptr));
    static std::uniform_real_distribution<float> randDir(-1.0f, 1.0f);
    
    _health = 20;
    
    _color.r = 255;
    _color.g = 255;
    _color.b = 255;
    _color.a = 255;
    m_textureID = GameEngine::ResourceManager::getTexture("Textures/human.png").id;
    
    _speed = speed;
    _position = pos;
    // Get random direction
    m_direction = glm::vec2(randDir(randomEngine), randDir(randomEngine));
    // Make sure direction isn't zero
    if (m_direction.length() == 0) m_direction = glm::vec2(1.0f, 0.0f);
    
    m_direction = glm::normalize(m_direction);
}

void Human::update(const std::vector<std::string>& levelData,
                   std::vector<Human*>& humans,
                   std::vector<Zombie*>& zombies, float deltaTime) {
    
    static std::mt19937 randomEngine((unsigned int)time(nullptr));
    static std::uniform_real_distribution<float> randRotate(-0.8f, 0.8f);
    
    _position += m_direction * _speed * deltaTime;
    
    // Randomly change direction every 20 frames
    if (_frames == 20) {
        m_direction = glm::rotate(m_direction, randRotate(randomEngine));
        _frames = 0;
    } else {
        _frames++;
    }
    
    if (collideWithLevel(levelData)) {
        m_direction = glm::rotate(m_direction, randRotate(randomEngine));
    }
}
