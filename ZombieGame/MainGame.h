//
//  MainGame.h
//  ZombieGame
//
//  Created by Yvo Keuter on 9/10/15.
//  Copyright (c) 2015 blabla. All rights reserved.
//

#ifndef __ZombieGame__MainGame__
#define __ZombieGame__MainGame__

#include <GameEngine/Window.h>
#include <GameEngine/GLSLProgram.h>
#include <GameEngine/Camera2D.h>
#include <GameEngine/InputManager.h>
#include <GameEngine/SpriteBatch.h>
#include <GameEngine/SpriteFont.h>
#include <GameEngine/AudioEngine.h>

#include <GameEngine/ParticleEngine2D.h>
#include <GameEngine/ParticleBatch2D.h>

#include "Player.h"
#include "Level.h"
#include "Bullet.h"

enum class GameState {
    PLAY,
    EXIT
};

class MainGame
{
public:
    MainGame();
    ~MainGame();
    
    /// Runs the game
    void run();
    
private:
    /// Initializes the core systems
    void initSystems();
    
    /// Initializes the shaders
    void initShaders();
    
    /// Initializes the level and sets up everything
    void initLevel();
    
    /// Main game loop for the program
    void gameLoop();
    
    /// Handles input processing
    void processInput();
    
    /// Renders the game
    void drawGame();
    void drawHud();
    
    /// Adds blood to the particle engine
    void addBlood(const glm::vec2& position, int numParticles);
    
    /// Updates all agents
    void updateAgents(float deltaTime);
    
    /// Updates all bullets
    void updateBullets(float deltaTime);
    
    /// Checks the victory condition
    void checkVictory();
    
    /// Member Variables
    GameEngine::Window _window; ///< The game window
    
    GameEngine::GLSLProgram _textureProgram; ///< The shader program
    
    GameEngine::InputManager _inputManager; ///< Handles input
    
    GameEngine::Camera2D _camera; ///< Main Camera
    GameEngine::Camera2D _hudCamera;
    
    GameEngine::SpriteBatch _agentSpriteBatch; ///< Draws all agents
    GameEngine::SpriteBatch _hudSpriteBatch;
    
    GameEngine::ParticleEngine2D m_particleEngine;
    GameEngine::ParticleBatch2D* m_bloodParticleBatch;
    
    std::vector<Level*> _levels; ///< vector of all levels
    
    int _screenWidth, _screenHeight;
    
    float _fps;
    
    int _currentLevel;
    
    Player* _player;
    std::vector<Human*> _humans; ///< Vector of all humans
    std::vector<Zombie*> _zombies; ///< Vector of all zombies
    std::vector<Bullet> _bullets;
    
    int _numHumansKilled; ///< Humans killed by player
    int _numZombiesKilled; ///< Zombies killed by player
    
    GameEngine::SpriteFont* _spriteFont;
    GameEngine::AudioEngine m_audioEngine;
    
    GameState _gameState;
};

#endif /* defined(__ZombieGame__MainGame__) */
