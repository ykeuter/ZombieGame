//
//  Level.h
//  ZombieGame
//
//  Created by Yvo Keuter on 9/14/15.
//  Copyright (c) 2015 blabla. All rights reserved.
//

#ifndef __ZombieGame__Level__
#define __ZombieGame__Level__

#include <string>
#include <vector>

#include <GameEngine/SpriteBatch.h>

const int TILE_WIDTH = 64;

class Level
{
public:
    // Load the level
    Level(const std::string& fileName);
    ~Level();
    
    void draw();
    
    // Getters
    unsigned long getWidth() const { return _levelData[0].size(); }
    unsigned long getHeight() const { return _levelData.size(); }
    int getNumHumans() const { return _numHumans; }
    const std::vector<std::string>& getLevelData() const { return _levelData; }
    glm::vec2 getStartPlayerPos() const { return _startPlayerPos; }
    const std::vector<glm::vec2>& getZombieStartPositions() const { return _zombieStartPositions; }
    
private:
    std::vector<std::string> _levelData;
    int _numHumans;
    GameEngine::SpriteBatch _spriteBatch;
    
    glm::vec2 _startPlayerPos;
    std::vector<glm::vec2> _zombieStartPositions;
};



#endif /* defined(__ZombieGame__Level__) */
