#pragma once
/*
*  gameFunctions.cpp
*     - The functions for the game program are in this file.
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
const float ENEMY_SPEED = 9.0f;
// Game Functions
void moveShip(Sprite& ship); //check ship movenment
void moveMissile(Sprite& missile, bool& isMissileInFlight); // check if missile is in motion
void moveEnemy(Sprite& enemy, bool& enemyMovingRight); // move and track the direction of the enemy
bool checkCollision(const Sprite& missile, const Sprite& enemy); // collisision detector for missile and enemy