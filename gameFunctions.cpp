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
    // Use the sprite's real size instead of guessing at the width/height.
    FloatRect bounds = ship.getGlobalBounds();
    Vector2f shipPos = ship.getPosition(); // shipPos: Ship's current x, y

    // Each direction is checked on its own (not else-if) so the ship can
    // move diagonally, and so one key doesn't block the others.
    if (Keyboard::isKeyPressed(Keyboard::Key::Left)) // Left key pressed?
    {
        if (shipPos.x - DISTANCE >= 0) { // Check left edge
            ship.move({ -DISTANCE, 0 }); // Move left
        }
    }
    if (Keyboard::isKeyPressed(Keyboard::Key::Right)) // Right key pressed?
    {
        if (shipPos.x + DISTANCE <= WINDOW_WIDTH - bounds.size.x) { // Check right edge
            ship.move({ DISTANCE, 0 }); // Move right
        }
    }
    if (Keyboard::isKeyPressed(Keyboard::Key::Up)) // Up key pressed?
    {
        // The ship patrols the lower part of the screen. It used to be
        // locked below y = 480, but the ship spawns higher than that, so
        // it could never move up at all. Top limit is now above the spawn.
        if (shipPos.y - DISTANCE >= WINDOW_HEIGHT * 0.5f) { // Check upper boundary
            ship.move({ 0.0, -DISTANCE }); // Move up
        }
    }
    if (Keyboard::isKeyPressed(Keyboard::Key::Down)) // Down key pressed?
    {
        if (shipPos.y + DISTANCE <= WINDOW_HEIGHT - bounds.size.y) { // Check lower boundary
            ship.move({ 0.0, DISTANCE }); // Move down
        }
    }
}

void moveMissile(Sprite& missile, bool& isMissileInFlight) { // Moves missile
    if (isMissileInFlight) { // Missile active?
        missile.move({ 0, -MISSILE_SPEED }); // Move up
        if (missile.getPosition().y < 0) { // Off top?
            isMissileInFlight = false; // Deactivate missile
        }
    }
}

void moveEnemy(Sprite& enemy, bool& enemyMovingRight, float speed) { // Moves enemy
    if (enemyMovingRight) { // Moving right?
        enemy.move({ speed, 0 }); // Move right
    }
    else { // Moving left?
        enemy.move({ -speed, 0 }); // Move left
    }

    // Check the position AFTER moving (checking the old position first
    // let it overshoot the edge for a frame) and keep the whole sprite
    // on screen using its real width.
    FloatRect bounds = enemy.getGlobalBounds();
    Vector2f enemyPos = enemy.getPosition(); // enemyPos: Enemy's current x, y

    if (enemyPos.x <= 0) { // At left edge?
        enemy.setPosition({ 0, enemyPos.y });
        enemyMovingRight = true; // Go right
    }
    else if (enemyPos.x >= WINDOW_WIDTH - bounds.size.x) { // At right edge?
        enemy.setPosition({ WINDOW_WIDTH - bounds.size.x, enemyPos.y });
        enemyMovingRight = false; // Go left
    }
}

bool checkCollision(const Sprite& missile, const Sprite& enemy) { // Checks collision
    FloatRect missileBounds = missile.getGlobalBounds(); // missileBounds: Missile's hitbox
    FloatRect enemyBounds = enemy.getGlobalBounds(); // enemyBounds: Enemy's hitbox

    if (const auto intersection = missileBounds.findIntersection(enemyBounds)) { // Overlap?
        return true; // Collision detected
    }
    return false; // No collision
}
