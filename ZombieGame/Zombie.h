//
//  Zombie.h
//  ZombieGame
//
//  Created by Yvo Keuter on 9/14/15.
//  Copyright (c) 2015 blabla. All rights reserved.
//

#ifndef __ZombieGame__Zombie__
#define __ZombieGame__Zombie__

#include "Agent.h"

class Zombie : public Agent
{
public:
    Zombie();
    ~Zombie();
    
    void init(float speed, glm::vec2 pos);
    
    virtual void update(const std::vector<std::string>& levelData,
                        std::vector<Human*>& humans,
                        std::vector<Zombie*>& zombies, float deltaTime) override;
private:
    
    Human* getNearestHuman(std::vector<Human*>& humans);
};

#endif /* defined(__ZombieGame__Zombie__) */
