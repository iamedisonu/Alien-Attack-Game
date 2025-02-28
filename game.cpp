#include "gameHeader.h"
/*
*  game.cpp
*     - A simple game loop that allows you to move the ship
*       and eventually fire a missile.
*  AUTHOR: Most of the work in this program was completed by Prof. Andy Harbert
*          Prof. Pat Smith has made modifications (December 2021)
*
*/

int main()
{
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

    Sprite background(starsTexture); // background: The sprite that represents the background
    //background.setTexture(starsTexture); // load the starsTexture object into the sprite.
    // The texture file is 640x480, so scale it up a little to cover 800x600 window
    background.setScale({ 1.5, 1.5 });// must include vector2 types in curly braces. 

    // create sprite and texture it
    Sprite ship(shipTexture); // ship: The main player's ship sprite

    // *** You will have to code to load the  texture for the missile here. 
    // Then create the missile Sprite...  
    Texture missileTexture; // missileTexture: Stores the image data for the missile
    if (!missileTexture.loadFromFile("missile.png"))
    {
        cout << "Unable to load missile texture!" << endl;
        exit(EXIT_FAILURE);
    }

    Sprite missile(missileTexture); // missile: The missile sprite
    missile.setScale({ 2.0, 2.0 }); // Scale missile 2x

    // initial position of the ship will be approx middle of screen
    float shipX = window.getSize().x / 2.0f; // shipX: X-coordinate of the ship (center of window)
    float shipY = window.getSize().y / 2.0f; // shipY: Y-coordinate of the ship (center of window)
    ship.setPosition({ shipX, shipY });
    ship.setScale({ 2.5, 2.5 });

    bool isMissileInFlight = false; // isMissileInFlight: Tracks whether a missile is active on screen

    while (window.isOpen())
    {
        // check all the window's events that were triggered since the last iteration of the loop
        // For now, we just need this so we can click on the window and close it
        // don't declare an event here, it will be declared in the while loop!
        // Event event;

        // This while loop checks to see if anything happened since last time
        // we drew the window and all its graphics. 
        // while (window.pollEvent(event)) // this was SFML 2.x version
        while (const std::optional event = window.pollEvent()) // get a polling event. 
        {
            // if (event.type == Event::Closed) // Did the user kill the window by pressing the "X"?
            if (event->is<sf::Event::Closed>()) // new 3.0.0 check on event "what *is* it?"
                window.close();
            //else if (event.type == Event::KeyPressed) // did the user press a key on the keyboard?
            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
            {
                // if the key that was pressed was a space, fire a missile. 
                if (keyPressed->scancode == sf::Keyboard::Scancode::Space && !isMissileInFlight)
                {
                    isMissileInFlight = true;
                    // Set missile position slightly above the ship
                    Vector2f shipPos = ship.getPosition(); // shipPos: Holds the current ship position
                    missile.setPosition({ shipPos.x + 15, shipPos.y - 10 });

                    // You add the code to initialize missile positino
                    //  You should have created a missile sprite 
                    // above the main loop, maybe around line 60? 
                }
            }
        }

        //===========================================================
        // Everything from here to the end of the loop is where you put your
        // code to produce ONE frame of the animation. The next iteration of the loop will
        // render the next frame, and so on. All this happens ~ 60 times/second.
        //===========================================================

        // draw background first, so everything that's drawn later 
        // will appear on top of background
        window.draw(background);

        //detect key presses to update the position of the ship. 
        //See moveShip() function above.
        moveShip(ship);

        // draw the ship on top of background 
        // (the ship from previous frame was erased when we drew background)
        window.draw(ship);

        // Missile movement logic
        if (isMissileInFlight)
        {
            missile.move({ 0, -5.0f }); // Move missile up 5px

            if (missile.getPosition().y < 0) // Missile off top?
            {
                isMissileInFlight = false; // Hide missile
            }
        }

        // Move alien enemy
        moveEnemy(movingAlien, alienMovingRight);

        // Check if missile hits alien
        if (isMissileInFlight && checkCollision(missile, movingAlien))
        {
            cout << "Hit!" << endl;
            isMissileInFlight = false;
        }

        // end the current frame; this makes everything that we have 
        // already "drawn" actually show up on the screen
        window.clear();
        window.draw(ship);
        window.draw(movingAlien); // Draw alien

        if (isMissileInFlight)
        {
            window.draw(missile);
        }

        window.display();

        // At this point the frame we have built is now visible on screen.
        // Now control will go back to the top of the animation loop
        // to build the next frame. Since we begin by drawing the
        // background, each frame is rebuilt from scratch.

    } // end body of animation loop

    return 0;
}
