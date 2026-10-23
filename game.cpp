#include "gameHeader.h"
#include <cstdlib>
#include <ctime>
/*
*  game.cpp
*     - A simple game loop that allows you to move the ship
*       and fire missiles at the alien. Shoot the alien down to
*       score points - it comes back faster every time.
*  AUTHOR: Most of the work in this program was completed by Prof. Andy Harbert
*          Prof. Pat Smith has made modifications (December 2021)
*
*/

int main()
{
    srand(static_cast<unsigned>(time(nullptr)));

    // Create the window for graphics.
    //  The "aliens" is the text in the title bar on the window.
    RenderWindow window(VideoMode({ WINDOW_WIDTH, WINDOW_HEIGHT }), "Aliens!"); // window: The main game window

    // Limit the framerate to 60 frames per second
    window.setFramerateLimit(60);

    // load textures from file into memory. This doesn't display anything yet.
    // Notice we do this *before* going into animation loop.

    // a Texture is an image of pixels. You can load a .png file
    //   or a bitmap file.  These files are in the "Resource files" section of
    //   the solution explorer.
    Texture shipTexture; // shipTexture: Stores the image data for the ship
    if (!shipTexture.loadFromFile("ship.png"))
    {
        cout << "Unable to load ship texture!" << endl;
        exit(EXIT_FAILURE);
    }

    // We use the star texture as a background for space in the window.
    //  it makes it more interesting. :)
    Texture starsTexture; // starsTexture: Stores the background image data
    if (!starsTexture.loadFromFile("stars.jpg"))
    {
        cout << "Unable to load stars texture!" << endl;
        exit(EXIT_FAILURE);
    }

    // An alien bitmap is provided in the Resource Files...
    //  Perhaps you'll want to make your own alien though...
    Texture alienTexture; // alienTexture: Stores the image data for the alien
    if (!alienTexture.loadFromFile("alien.bmp"))
    {
        cout << "Unable to load Alien texture!" << endl;
        exit(EXIT_FAILURE);
    }

    Sprite movingAlien(alienTexture); // movingAlien: Alien sprite used in the game
    movingAlien.setPosition({ 5.0f, 20.0f }); // Set alien near top-left
    movingAlien.setScale({ 2.0, 2.0 }); // Scale alien 2x
    bool alienMovingRight = true; // alienMovingRight: Boolean flag to track alien movement direction
    float alienSpeed = ENEMY_SPEED; // alienSpeed: Current alien speed, goes up after every hit

    Sprite background(starsTexture); // background: The sprite that represents the background
    // The texture file is 640x480, so scale it up a little to cover 800x600 window
    background.setScale({ 1.5, 1.5 });// must include vector2 types in curly braces.

    // create sprite and texture it
    Sprite ship(shipTexture); // ship: The main player's ship sprite
    ship.setScale({ 2.5, 2.5 });

    // The ship starts near the bottom of the screen, in the area it can
    // actually move around in (it used to spawn mid-screen, above the
    // boundary that moveShip() enforced, so it couldn't move up).
    float shipX = window.getSize().x / 2.0f; // shipX: X-coordinate of the ship (center of window)
    float shipY = WINDOW_HEIGHT - 90.0f; // shipY: Y-coordinate of the ship (near the bottom)
    ship.setPosition({ shipX, shipY });

    Texture missileTexture; // missileTexture: Stores the image data for the missile
    if (!missileTexture.loadFromFile("missile.png"))
    {
        cout << "Unable to load missile texture!" << endl;
        exit(EXIT_FAILURE);
    }

    Sprite missile(missileTexture); // missile: The missile sprite
    missile.setScale({ 2.0, 2.0 }); // Scale missile 2x

    bool isMissileInFlight = false; // isMissileInFlight: Tracks whether a missile is active on screen

    // Score display. If the font can't be loaded the game still runs,
    // the score just gets printed to the console like the hits used to be.
    int score = 0; // score: Player's score
    Font font; // font: Font for the on-screen score
    bool fontLoaded = font.openFromFile("C:/Windows/Fonts/arial.ttf");
    Text scoreText(font, "Score: 0", 24); // scoreText: On-screen score display
    if (fontLoaded)
    {
        scoreText.setPosition({ 10.0f, 10.0f });
        scoreText.setFillColor(Color::White);
    }

    while (window.isOpen())
    {
        // check all the window's events that were triggered since the last iteration of the loop
        while (const std::optional event = window.pollEvent()) // get a polling event.
        {
            if (event->is<sf::Event::Closed>()) // Did the user kill the window by pressing the "X"?
                window.close();
            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
            {
                // if the key that was pressed was a space, fire a missile.
                if (keyPressed->scancode == sf::Keyboard::Scancode::Space && !isMissileInFlight)
                {
                    isMissileInFlight = true;
                    // Set missile position slightly above the ship
                    Vector2f shipPos = ship.getPosition(); // shipPos: Holds the current ship position
                    missile.setPosition({ shipPos.x + 15, shipPos.y - 10 });
                }
            }
        }

        //===========================================================
        // Everything from here to the end of the loop is where you put your
        // code to produce ONE frame of the animation. The next iteration of the loop will
        // render the next frame, and so on. All this happens ~ 60 times/second.
        //===========================================================

        // detect key presses to update the position of the ship.
        moveShip(ship);

        // Missile movement logic
        moveMissile(missile, isMissileInFlight);

        // Move alien enemy
        moveEnemy(movingAlien, alienMovingRight, alienSpeed);

        // Check if missile hits alien
        if (isMissileInFlight && checkCollision(missile, movingAlien))
        {
            score += ALIEN_POINTS;
            cout << "Hit! Score: " << score << endl;
            isMissileInFlight = false;

            // Bring the alien back at a random spot along the top,
            // a little faster than before.
            float alienWidth = movingAlien.getGlobalBounds().size.x;
            float maxX = WINDOW_WIDTH - alienWidth;
            float newX = static_cast<float>(rand() % static_cast<int>(maxX));
            movingAlien.setPosition({ newX, 20.0f });
            alienSpeed += 0.5f;

            scoreText.setString("Score: " + to_string(score));
        }

        // Draw the frame. clear() has to come FIRST - this used to draw
        // the background and the ship and then clear the window, which
        // erased them, so the background never showed up at all.
        window.clear();
        window.draw(background);
        window.draw(ship);
        window.draw(movingAlien); // Draw alien

        if (isMissileInFlight)
        {
            window.draw(missile);
        }

        if (fontLoaded)
        {
            window.draw(scoreText);
        }

        window.display();

        // At this point the frame we have built is now visible on screen.
        // Now control will go back to the top of the animation loop
        // to build the next frame.
    } // end body of animation loop

    return 0;
}
