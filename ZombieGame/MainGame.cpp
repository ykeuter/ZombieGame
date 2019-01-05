//
//  MainGame.cpp
//  ZombieGame
//
//  Created by Yvo Keuter on 9/10/15.
//  Copyright (c) 2015 blabla. All rights reserved.
//

#include "MainGame.h"

#include <GameEngine/GameEngine.h>
#include <GameEngine/Timing.h>
#include <GameEngine/ResourceManager.h>
#include <GameEngine/GameEngineErrors.h>
#include <random>
#include <ctime>

#include <SDL2/SDL.h>
#include <iostream>
#include <glm/gtx/rotate_vector.hpp>

#include "Gun.h"
#include "Zombie.h"

const float HUMAN_SPEED = 1.0f;
const float ZOMBIE_SPEED = 1.3f;
const float PLAYER_SPEED = 5.0f;

MainGame::MainGame()  :
_screenWidth(1024),
_screenHeight(768),
_gameState(GameState::PLAY),
_fps(0),
_player(nullptr),
_numHumansKilled(0),
_numZombiesKilled(0) {
    // Empty
    
}

MainGame::~MainGame() {
    // Don't forget to delete the levels!
    for (int i = 0; i < _levels.size(); i++) {
        delete _levels[i];
    }
    // Don't forget to delete the humans and zombies!
    for (int i = 0; i < _humans.size(); i++) {
        delete _humans[i];
    }
    for (int i = 0; i < _zombies.size(); i++) {
        delete _zombies[i];
    }
    
}

void MainGame::run() {
    
    initSystems();
    
    initLevel();
    
    GameEngine::Music music = m_audioEngine.loadMusic("Sound/XYZ.ogg");
    music.play(-1);
    gameLoop();
}

void MainGame::initSystems() {
    // Initialize the game engine
    GameEngine::init();
    
    m_audioEngine.init();
    
    // Create our window
    _window.create("ZombieGame", _screenWidth, _screenHeight, 0);
    
    // Grey background color
    glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
    
    // Set up the shaders
    initShaders();
    
    // Initialize our spritebatch
    _agentSpriteBatch.init();
    _hudSpriteBatch.init();
    
    _spriteFont = new GameEngine::SpriteFont("Fonts/horrendo.ttf", 64);
    
    // Set up the camera
    _camera.init(_screenWidth, _screenHeight);
    _hudCamera.init(_screenWidth, _screenHeight);
    _hudCamera.setPosition(glm::vec2(_screenWidth / 2, _screenHeight / 2));
    
    // Initialize particles
    m_bloodParticleBatch = new GameEngine::ParticleBatch2D;
    // Initialize the particle batch and use a lambda function to define the update
    m_bloodParticleBatch->init(1000, 0.05f,
                               GameEngine::ResourceManager::getTexture("Textures/particle.png"),
                               [](GameEngine::Particle2D& particle, float deltaTime) {
                                   particle.position += particle.velocity * deltaTime;
                                   particle.color.a = (GLubyte)(particle.life * 255.0f);
                               });
    m_particleEngine.addParticleBatch(m_bloodParticleBatch);
}

void MainGame::initLevel() {
    // Level 1
    _levels.push_back(new Level("Levels/level2.txt"));
    _currentLevel = 0;
    
    _player = new Player();
    _player->init(PLAYER_SPEED, _levels[_currentLevel]->getStartPlayerPos(), &_inputManager, &_camera, &_bullets);
    
    _humans.push_back(_player);
    
    std::mt19937 randomEngine;
    randomEngine.seed((unsigned int)time(nullptr));
    
    std::uniform_int_distribution<int> randX(2, (int)_levels[_currentLevel]->getWidth() - 2);
    std::uniform_int_distribution<int> randY(2, (int)_levels[_currentLevel]->getHeight() - 2);
    
    // Add all the random humans
    for (int i = 0; i < _levels[_currentLevel]->getNumHumans(); i++) {
        _humans.push_back(new Human);
        glm::vec2 pos(randX(randomEngine) * TILE_WIDTH, randY(randomEngine) * TILE_WIDTH);
        _humans.back()->init(HUMAN_SPEED, pos);
    }
    
    // Add the zombies
    const std::vector<glm::vec2>& zombiePositions = _levels[_currentLevel]->getZombieStartPositions();
    for (int i = 0; i < zombiePositions.size(); i++) {
        _zombies.push_back(new Zombie);
        _zombies.back()->init(ZOMBIE_SPEED, zombiePositions[i]);
    }
    
    // Set up the players guns
    const float BULLET_SPEED = 20.0f;
    _player->addGun(new Gun("Magnum", 10, 1, 0.1f, 30, BULLET_SPEED, m_audioEngine.loadSoundEffect("Sound/shots/pistol.wav")));
    _player->addGun(new Gun("Shotgun", 30, 12, 0.4f, 4, BULLET_SPEED, m_audioEngine.loadSoundEffect("Sound/shots/shotgun.wav")));
    _player->addGun(new Gun("MP5", 2, 1, 0.2f, 20, BULLET_SPEED, m_audioEngine.loadSoundEffect("Sound/shots/cg1.wav")));
}

void MainGame::initShaders() {
    // Compile our color shader
    _textureProgram.compileShaders("Shaders/textureShading.vert", "Shaders/textureShading.frag");
    _textureProgram.addAttribute("vertexPosition");
    _textureProgram.addAttribute("vertexColor");
    _textureProgram.addAttribute("vertexUV");
    _textureProgram.linkShaders();
}

void MainGame::gameLoop() {
    // Some helpful constants.
    const float DESIRED_FPS = 60.0f; // FPS the game is designed to run at
    const int MAX_PHYSICS_STEPS = 6; // Max number of physics steps per frame
    const float MS_PER_SECOND = 1000; // Number of milliseconds in a second
    const float DESIRED_FRAMETIME = MS_PER_SECOND / DESIRED_FPS; // The desired frame time per frame
    const float MAX_DELTA_TIME = 1.0f; // Maximum size of deltaTime
    
    GameEngine::FpsLimiter fpsLimiter;
    fpsLimiter.setMaxFPS(60.0f);
    
    const float CAMERA_SCALE = 1.0f / 3.0f;
    _camera.setScale(CAMERA_SCALE);
    
    // Start our previousTicks variable
    float previousTicks = SDL_GetTicks();
    
    // Main loop
    while (_gameState == GameState::PLAY) {
        fpsLimiter.begin();
        
        // Calculate the frameTime in milliseconds
        float newTicks = SDL_GetTicks();
        float frameTime = newTicks - previousTicks;
        previousTicks = newTicks; // Store newTicks in previousTicks so we can use it next frame
        // Get the total delta time
        float totalDeltaTime = frameTime / DESIRED_FRAMETIME;
        
        checkVictory();
        
        _inputManager.update();
        
        processInput();
        
        int i = 0; // This counter makes sure we don't spiral to death!
        // Loop while we still have steps to process.
        while (totalDeltaTime > 0.0f && i < MAX_PHYSICS_STEPS) {
            // The deltaTime should be the the smaller of the totalDeltaTime and MAX_DELTA_TIME
            float deltaTime = std::min(totalDeltaTime, MAX_DELTA_TIME);
            // Update all physics here and pass in deltaTime
            updateAgents(deltaTime);
            updateBullets(deltaTime);
            m_particleEngine.update(deltaTime);
            // Since we just took a step that is length deltaTime, subtract from totalDeltaTime
            totalDeltaTime -= deltaTime;
            // Increment our frame counter so we can limit steps to MAX_PHYSICS_STEPS
            i++;
        }
        
        _camera.setPosition(_player->getPosition());
        
        _camera.update();
        _hudCamera.update();
        
        drawGame();
        
        _fps = fpsLimiter.end();
        std::cout << _fps << std::endl;
    }
}

void MainGame::updateAgents(float deltaTime) {
    // Update all humans
    for (int i = 0; i < _humans.size(); i++) {
        _humans[i]->update(_levels[_currentLevel]->getLevelData(),
                           _humans,
                           _zombies, deltaTime);
    }
    
    // Update all zombies
    for (int i = 0; i < _zombies.size(); i++) {
        _zombies[i]->update(_levels[_currentLevel]->getLevelData(),
                            _humans,
                            _zombies, deltaTime);
    }
    
    // Update Zombie collisions
    for (int i = 0; i < _zombies.size(); i++) {
        // Collide with other zombies
        for (int j = i + 1; j < _zombies.size(); j++) {
            _zombies[i]->collideWithAgent(_zombies[j]);
        }
        // Collide with humans
        for (int j = 1; j < _humans.size(); j++) {
            if (_zombies[i]->collideWithAgent(_humans[j])) {
                // Add the new zombie
                _zombies.push_back(new Zombie);
                _zombies.back()->init(ZOMBIE_SPEED, _humans[j]->getPosition());
                // Delete the human
                delete _humans[j];
                _humans[j] = _humans.back();
                _humans.pop_back();
            }
        }
        
        // Collide with player
        if (_zombies[i]->collideWithAgent(_player)) {
            GameEngine::fatalError("YOU LOSE");
        }
    }
    
    // Update Human collisions
    for (int i = 0; i < _humans.size(); i++) {
        // Collide with other humans
        for (int j = i + 1; j < _humans.size(); j++) {
            _humans[i]->collideWithAgent(_humans[j]);
        }
    }
    
    // Dont forget to update zombies
}

void MainGame::updateBullets(float deltaTime) {
    // Update and collide with world
    for (int i = 0; i < _bullets.size(); ) {
        // If update returns true, the bullet collided with a wall
        if (_bullets[i].update(_levels[_currentLevel]->getLevelData(), deltaTime)) {
            _bullets[i] = _bullets.back();
            _bullets.pop_back();
        } else {
            i++;
        }
    }
    
    bool wasBulletRemoved;
    
    // Collide with humans and zombies
    for (int i = 0; i < _bullets.size(); i++) {
        wasBulletRemoved = false;
        // Loop through zombies
        for (int j = 0; j < _zombies.size(); ) {
            // Check collision
            if (_bullets[i].collideWithAgent(_zombies[j])) {
                // Add blood
                addBlood(_bullets[i].getPosition(), 5);
                
                // Damage zombie, and kill it if its out of health
                if (_zombies[j]->applyDamage(_bullets[i].getDamage())) {
                    // If the zombie died, remove him
                    delete _zombies[j];
                    _zombies[j] = _zombies.back();
                    _zombies.pop_back();
                    _numZombiesKilled++;
                } else {
                    j++;
                }
                
                // Remove the bullet
                _bullets[i] = _bullets.back();
                _bullets.pop_back();
                wasBulletRemoved = true;
                i--; // Make sure we don't skip a bullet
                // Since the bullet died, no need to loop through any more zombies
                break;
            } else {
                j++;
            }
        }
        // Loop through humans
        if (wasBulletRemoved == false) {
            for (int j = 1; j < _humans.size(); ) {
                // Check collision
                if (_bullets[i].collideWithAgent(_humans[j])) {
                    // Add blood
                    addBlood(_bullets[i].getPosition(), 5);
                    // Damage human, and kill it if its out of health
                    if (_humans[j]->applyDamage(_bullets[i].getDamage())) {
                        // If the human died, remove him
                        delete _humans[j];
                        _humans[j] = _humans.back();
                        _humans.pop_back();
                    } else {
                        j++;
                    }
                    
                    // Remove the bullet
                    _bullets[i] = _bullets.back();
                    _bullets.pop_back();
                    _numHumansKilled++;
                    i--; // Make sure we don't skip a bullet
                    // Since the bullet died, no need to loop through any more zombies
                    break;
                } else {
                    j++;
                }
            }
        }
    }
}

void MainGame::checkVictory() {
    // TODO: Support for multiple levels!
    // _currentLevel++; initLevel(...);
    
    // If all zombies are dead we win!
    if (_zombies.empty()) {
        // Print victory message
        std::printf("*** You win! ***\n You killed %d humans and %d zombies. There are %d/%d civilians remaining",
                    _numHumansKilled, _numZombiesKilled, (int)_humans.size() - 1, _levels[_currentLevel]->getNumHumans());
        
        // Easy way to end the game :P
        GameEngine::fatalError("");
    }
}

void MainGame::processInput() {
    SDL_Event evnt;
    //Will keep looping until there are no more events to process
    while (SDL_PollEvent(&evnt)) {
        switch (evnt.type) {
            case SDL_QUIT:
                _gameState = GameState::EXIT;
                break;
            case SDL_MOUSEMOTION:
                _inputManager.setMouseCoords(evnt.motion.x, evnt.motion.y);
                break;
            case SDL_KEYDOWN:
                _inputManager.pressKey(evnt.key.keysym.sym);
                break;
            case SDL_KEYUP:
                _inputManager.releaseKey(evnt.key.keysym.sym);
                break;
            case SDL_MOUSEBUTTONDOWN:
                _inputManager.pressKey(evnt.button.button);
                break;
            case SDL_MOUSEBUTTONUP:
                _inputManager.releaseKey(evnt.button.button);
                break;
        }
    }
}

void MainGame::drawGame() {
    // Set the base depth to 1.0
    glClearDepth(1.0);
    // Clear the color and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    _textureProgram.use();
    
    // Draw code goes here
    glActiveTexture(GL_TEXTURE0);
    
    // Make sure the shader uses texture 0
    GLint textureUniform = _textureProgram.getUniformLocation("mySampler");
    glUniform1i(textureUniform, 0);
    
    // Grab the camera matrix
    glm::mat4 projectionMatrix = _camera.getCameraMatrix();
    GLint pUniform = _textureProgram.getUniformLocation("P");
    glUniformMatrix4fv(pUniform, 1, GL_FALSE, &projectionMatrix[0][0]);
    
    // Draw the level
    _levels[_currentLevel]->draw();
    
    // Begin drawing agents
    _agentSpriteBatch.begin();
    
    const glm::vec2 agentDims(AGENT_RADIUS * 2.0f);
    
    // Draw the humans
    for (int i = 0; i < _humans.size(); i++) {
        if (_camera.isBoxInView(_humans[i]->getPosition(), agentDims)) {
            _humans[i]->draw(_agentSpriteBatch);
        }
    }
    
    // Draw the zombies
    for (int i = 0; i < _zombies.size(); i++) {
        if (_camera.isBoxInView(_zombies[i]->getPosition(), agentDims)) {
            _zombies[i]->draw(_agentSpriteBatch);
        }
    }
    
    // Draw the bullets
    for (int i = 0; i < _bullets.size(); i++) {
        _bullets[i].draw(_agentSpriteBatch);
    }
    
    // End spritebatch creation
    _agentSpriteBatch.end();
    
    // Render to the screen
    _agentSpriteBatch.renderBatch();
    
    // Render the particles
    m_particleEngine.draw(&_agentSpriteBatch);
    
    drawHud();
    
    // Unbind the program
    _textureProgram.unuse();
    
    // Swap our buffer and draw everything to the screen!
    
    _window.swapBuffer();
}

void MainGame::drawHud() {
    char buffer[256];
    
    // Grab the camera matrix
    glm::mat4 projectionMatrix = _hudCamera.getCameraMatrix();
    GLint pUniform = _textureProgram.getUniformLocation("P");
    glUniformMatrix4fv(pUniform, 1, GL_FALSE, &projectionMatrix[0][0]);
    
    _hudSpriteBatch.begin();
    sprintf(buffer, "Num humans: %d", (int)_humans.size());
    _spriteFont->draw(_hudSpriteBatch, buffer, glm::vec2(0.0f, 0.0f), glm::vec2(0.5f), 0.0f,
                      GameEngine::ColorRGBA8(255, 255, 255, 255));
    sprintf(buffer, "Num zombies: %d", (int)_zombies.size());
    _spriteFont->draw(_hudSpriteBatch, buffer, glm::vec2(0.0f, 36.0f), glm::vec2(0.5f), 0.0f,
                      GameEngine::ColorRGBA8(255, 255, 255, 255));
    _hudSpriteBatch.end();
    _hudSpriteBatch.renderBatch();
}

void MainGame::addBlood(const glm::vec2& position, int numParticles) {
    
    static std::mt19937 randEngine((int)time(nullptr));
    static std::uniform_real_distribution<float> randAngle(0.0f, 2.0f * M_PI);
    
    glm::vec2 vel(2.0f, 0.0f);
    GameEngine::ColorRGBA8 col(255, 0, 0, 255);
    
    for (int i = 0; i < numParticles; i++) {
        m_bloodParticleBatch->addParticle(position, glm::rotate(vel, randAngle(randEngine)), col, 30.0f);
    }
}
