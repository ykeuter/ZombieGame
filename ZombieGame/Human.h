//
//  Human.h
//  ZombieGame
//
//  Created by Yvo Keuter on 9/14/15.
//  Copyright (c) 2015 blabla. All rights reserved.
//

#ifndef __ZombieGame__Human__
#define __ZombieGame__Human__

#include "Agent.h"

class Human : public Agent
{
public:
    Human();
    virtual ~Human();
    
    void init(float speed, glm::vec2 pos);
    
    virtual void update(const std::vector<std::string>& levelData,
                        std::vector<Human*>& humans,
                        std::vector<Zombie*>& zombies, float deltaTime) override;
    
private:
    int _frames;
};

#endif /* defined(__ZombieGame__Human__) */
