#include "gameHeader.h" // Includes game constants and functions
/*
*  gameFunctions.cpp
*     - The functions for the game program are in this file.
*
*  AUTHOR: Most of the work in this program was completed by Prof. Andy Harbert
*          Prof. Pat Smith has made modifications (December 2021)
*
*/

// note: a Sprite represents an image on screen. A sprite knows and remembers its own position
// ship.move(offsetX, offsetY) adds offsetX, offsetY to 
// the current position of the ship. 
// x is horizontal, y is vertical. 
// 0,0 is in the UPPER LEFT of the screen, y increases DOWN the screen

void moveShip(Sprite& ship) // Moves ship based on keys
{
    Vector2f shipPos = ship.getPosition(); // shipPos: Ship’s current x, y
    const int ship_width = 53; // ship_width: Width of the ship sprite

    if (Keyboard::isKeyPressed(Keyboard::Key::Left)) // Left key pressed?
    {
        if (shipPos.x - DISTANCE >= 0) { // Check left edge
            ship.move({ -DISTANCE, 0 }); // Move left
        }
    }
    else if (Keyboard::isKeyPressed(Keyboard::Key::Right)) // Right key pressed?
    {
        if (shipPos.x + DISTANCE <= WINDOW_WIDTH - ship_width) { // Check right edge
            ship.move({ DISTANCE, 0 }); // Move right
        }
    }
    else if (Keyboard::isKeyPressed(Keyboard::Key::Up)) // Up key pressed?
    {
        if (shipPos.y > WINDOW_HEIGHT * 0.8f) { // Check upper boundary
            ship.move({ 0.0, -DISTANCE }); // Move up
        }
    }
    else if (Keyboard::isKeyPressed(Keyboard::Key::Down)) // Down key pressed?
    {
        if (shipPos.y + DISTANCE <= WINDOW_HEIGHT - 32) { // Check lower boundary
            ship.move({ 0.0, DISTANCE }); // Move down
        }
    }

    // What do you want to do when you reach the edge of the screen? 
    //   You might just move the ship back, simulate hitting a "wall"
    //   You might also wrap around (change from some value greater than
    //   the width of the window back to zero, or from some x value less
    //   than zero to something like 795, near the far right edge? 
    //  Or you could do nothing? 
    // This statement gets the ship's x location. If you decide you want 
    //   to do something with the problem of the ship moving off screen
    //   you can use this:
    //
    //        ship.setPosition(float x, float y); 
    // 
    //   You pass the setPosition method the x and y value you want 
    //     to set the ship at. 

    // Debugging: Uncomment to display ship's position
    // cout << ship.getPosition().x << ", " << ship.getPosition().y << endl;
}

void moveMissile(Sprite& missile, bool& isMissileInFlight) { // Moves missile
    if (isMissileInFlight) { // Missile active?
        missile.move({ 0, -5.0f }); // Move up
        if (missile.getPosition().y < 0) { // Off top?
            isMissileInFlight = false; // Deactivate missile
        }
    }
}

void moveEnemy(Sprite& enemy, bool& enemyMovingRight) { // Moves enemy
    Vector2f enemyPos = enemy.getPosition(); // enemyPos: Enemy’s x, y

    if (enemyMovingRight) { // Moving right?
        enemy.move({ ENEMY_SPEED, 0 }); // Move right
    }
    else { // Moving left?
        enemy.move({ -ENEMY_SPEED, 0 }); // Move left
    }

    if (enemyPos.x <= 50) { // At left limit?
        enemyMovingRight = true; // Go right
    }
    else if (enemyPos.x >= WINDOW_WIDTH - 114) { // At right limit?
        enemyMovingRight = false; // Go left
    }
}

bool checkCollision(const Sprite& missile, const Sprite& enemy) { // Checks collision
    FloatRect missileBounds = missile.getGlobalBounds(); // missileBounds: Missile’s hitbox
    FloatRect enemyBounds = enemy.getGlobalBounds(); // enemyBounds: Enemy’s hitbox

    if (const auto intersection = missileBounds.findIntersection(enemyBounds)) { // Overlap?
        return true; // Collision detected
    }
    return false; // No collision
}
