# Alien Attack

A little arcade shooter: pilot your ship, blast the alien waves, dodge their bombs, and don't let them land.

## ▶ Play it in your browser

**https://iamedisonu.github.io/Alien-Attack-Game/**

No download, no install — it just runs in the page.

| Controls | |
| --- | --- |
| Move | Arrow keys or `W` `A` `S` `D` (on mobile: drag, or use the on-screen buttons) |
| Shoot | `Space` (mobile: the **Fire** button) |
| Pause | `P` or `Esc` |
| Sound on/off | `M` |

Shoot an alien: **100 points**. Clear a wave: **250 point bonus**, and the next wave comes in bigger and faster. You get 3 ships — getting bombed costs one, and if the aliens reach the ground line it's over either way. Your best score is saved in the browser.

## The original desktop version

This started as a C++ / SFML desktop game (a class project — most of the original work is by Prof. Andy Harbert, with modifications by Prof. Pat Smith, Dec 2021):

- `game.cpp` — main loop, window, firing, scoring
- `gameFunctions.cpp` — ship / missile / alien movement and collision detection
- `gameHeader.h` — constants and declarations
- `ship.png`, `alien.bmp`, `missile.png`, `stars.jpg` — the original sprites and background

To build it you need [SFML 3](https://www.sfml-dev.org/) and a C++ compiler. With g++ on Linux/macOS:

```bash
g++ -std=c++17 game.cpp gameFunctions.cpp -o alien-attack $(pkg-config --cflags --libs sfml-graphics sfml-window sfml-system)
./alien-attack
```

On Windows the included Visual Studio project (`game.vcxproj`) works — just point it at your SFML install.

## The web version

`index.html`, `style.css` and `game.js` are a straight port to HTML canvas so it can live on GitHub Pages — same ship, same aliens, plus the stuff the desktop version was missing: waves, a score that shows on screen, lives, sound effects, and touch controls. No build step and no dependencies; open `index.html` and it runs.

## Fixes in this update

- The background never actually showed up in the desktop version: the game drew the background and the ship, then cleared the window, wiping both. Drawing now happens in the right order.
- The ship spawned in the middle of the screen but could only move up from near the bottom, so Up did nothing. It spawns near the bottom now and the movement bounds use the sprite's real size.
- Ship movement was an `else if` chain, so you could only ever move in one direction at a time — no diagonals. Each key is checked on its own now.
- A missile hit only printed "Hit!" to the console. Hits now score points on screen, the alien respawns, and it gets a bit faster every time.
- Cleaned the committed Visual Studio build files (`Debug/`, `.user` file) out of the repo and added a `.gitignore` so they stay out.
