#pragma once
/*
*  gameHeader.h
*     - Constants and function declarations for the game.
*
*  AUTHOR: Most of the work in this program was completed by Prof. Andy Harbert
*          Prof. Pat Smith has made modifications (December 2021)
*
*/
// includes go below
#include <iostream>
using namespace std;
// these two lines are specific to the SFML graphics library.
#include <SFML/Graphics.hpp>
using namespace sf;

// Constants for the game
const float DISTANCE = 5.0f; // When the ship moves it moves 5 pixels at a time.
const int WINDOW_WIDTH = 800; // window is 800 pixels wide
const int WINDOW_HEIGHT = 600;// window is 600 pixels vertically "high"
const float ENEMY_SPEED = 3.0f; // aliens pick up speed every time one is shot down (see game.cpp)
const float MISSILE_SPEED = 7.0f; // how fast a missile travels up the screen
const int ALIEN_POINTS = 100; // points for shooting down one alien

// Game Functions
void moveShip(Sprite& ship); // check ship movement
void moveMissile(Sprite& missile, bool& isMissileInFlight); // check if missile is in motion
void moveEnemy(Sprite& enemy, bool& enemyMovingRight, float speed); // move and track the direction of the enemy
bool checkCollision(const Sprite& missile, const Sprite& enemy); // collision detector for missile and enemy
