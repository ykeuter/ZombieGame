//
//  Player.h
//  ZombieGame
//
//  Created by Yvo Keuter on 9/14/15.
//  Copyright (c) 2015 blabla. All rights reserved.
//

#ifndef __ZombieGame__Player__
#define __ZombieGame__Player__

#include "Human.h"
#include <GameEngine/InputManager.h>
#include <GameEngine/Camera2D.h>
#include "Bullet.h"

class Gun;

class Player : public Human
{
public:
    Player();
    ~Player();
    
    void init(float speed, glm::vec2 pos, GameEngine::InputManager* inputManager, GameEngine::Camera2D* camera, std::vector<Bullet>* bullets);
    
    void addGun(Gun* gun);
    
    void update(const std::vector<std::string>& levelData,
                std::vector<Human*>& humans,
                std::vector<Zombie*>& zombies, float deltaTime) override;
private:
    GameEngine::InputManager* _inputManager;
    
    std::vector<Gun*> _guns;
    int _currentGunIndex;
    
    GameEngine::Camera2D* _camera;
    std::vector<Bullet>* _bullets;
    
};

#endif /* defined(__ZombieGame__Player__) */
