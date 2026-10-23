"use strict";
/*
 * Alien Attack — web version
 * Same idea as the original C++/SFML game in this repo: pilot the ship at
 * the bottom of the screen, shoot the aliens, don't get hit. This version
 * runs in a canvas so it can be played straight from GitHub Pages.
 */

const canvas = document.getElementById("game");
const ctx = canvas.getContext("2d");
const W = canvas.width;   // 800
const H = canvas.height;  // 600

// ---------- DOM ----------
const hudScore = document.getElementById("hudScore");
const hudBest  = document.getElementById("hudBest");
const hudWave  = document.getElementById("hudWave");
const hudLives = document.getElementById("hudLives");
const menuOverlay   = document.getElementById("menuOverlay");
const overOverlay   = document.getElementById("overOverlay");
const pausedOverlay = document.getElementById("pausedOverlay");
const finalStats    = document.getElementById("finalStats");
const newBestEl     = document.getElementById("newBest");
const menuBest      = document.getElementById("menuBest");
const soundBtn      = document.getElementById("soundBtn");
const pauseBtn      = document.getElementById("pauseBtn");

// ---------- helpers ----------
const clamp = (v, lo, hi) => Math.max(lo, Math.min(hi, v));
const rand  = (lo, hi) => lo + Math.random() * (hi - lo);
function overlap(a, b) {
  return a.x < b.x + b.w && a.x + a.w > b.x && a.y < b.y + b.h && a.y + a.h > b.y;
}

// ---------- sound (tiny WebAudio bleeps, no audio files needed) ----------
let audioCtx = null;
let soundOn = localStorage.getItem("alienAttackSound") !== "off";
function beep(freq, dur, type, vol, slideTo) {
  if (!soundOn) return;
  try {
    if (!audioCtx) audioCtx = new (window.AudioContext || window.webkitAudioContext)();
    if (audioCtx.state === "suspended") audioCtx.resume();
    const t = audioCtx.currentTime;
    const osc = audioCtx.createOscillator();
    const gain = audioCtx.createGain();
    osc.type = type || "square";
    osc.frequency.setValueAtTime(freq, t);
    if (slideTo) osc.frequency.exponentialRampToValueAtTime(slideTo, t + dur);
    gain.gain.setValueAtTime(vol || 0.05, t);
    gain.gain.exponentialRampToValueAtTime(0.0001, t + dur);
    osc.connect(gain).connect(audioCtx.destination);
    osc.start(t);
    osc.stop(t + dur);
  } catch (e) { /* sound is a nice-to-have; never break the game over it */ }
}
const sfx = {
  shoot:   () => beep(880, 0.12, "square", 0.04, 220),
  boom:    () => beep(160, 0.3, "sawtooth", 0.07, 40),
  hurt:    () => beep(300, 0.4, "sawtooth", 0.08, 60),
  wave:    () => { beep(523, 0.1, "square", 0.05); setTimeout(() => beep(784, 0.14, "square", 0.05), 110); },
  gameover:() => { beep(392, 0.16, "triangle", 0.07); setTimeout(() => beep(311, 0.16, "triangle", 0.07), 170); setTimeout(() => beep(262, 0.3, "triangle", 0.07), 340); }
};
function refreshSoundBtn() {
  soundBtn.textContent = soundOn ? "🔊 Sound on" : "🔇 Sound off";
  soundBtn.setAttribute("aria-pressed", String(soundOn));
}
function toggleSound() {
  soundOn = !soundOn;
  localStorage.setItem("alienAttackSound", soundOn ? "on" : "off");
  refreshSoundBtn();
  if (soundOn) sfx.shoot();
}

// ---------- background starfield ----------
const stars = [];
for (let i = 0; i < 140; i++) {
  stars.push({ x: Math.random() * W, y: Math.random() * H, r: rand(0.5, 1.8), s: rand(12, 55) });
}

// ---------- game state ----------
const STATE = { MENU: 0, PLAYING: 1, PAUSED: 2, OVER: 3 };
let state = STATE.MENU;

const ship = { x: W / 2 - 26, y: H - 84, w: 52, h: 44, speed: 380, cooldown: 0, invincibleUntil: 0 };
let missiles = [];   // player missiles
let bombs = [];      // alien bombs
let aliens = [];     // the current wave
let particles = [];
let keys = {};
let touch = { left: false, right: false, fire: false, dragX: null };

let score = 0;
let lives = 3;
let wave = 1;
let best = parseInt(localStorage.getItem("alienAttackBest") || "0", 10) || 0;
let fleetDir = 1;        // 1 = moving right, -1 = moving left
let fleetSpeed = 40;     // px per second, set per wave
let bombTimer = 1.2;     // seconds until the next alien bomb
let shake = 0;
let elapsed = 0;

function startWave(n) {
  aliens = [];
  const cols = Math.min(5 + n, 9);
  const rows = Math.min(2 + Math.ceil(n / 2), 4);
  const aw = 46, ah = 34, gapX = 22, gapY = 18;
  const totalW = cols * aw + (cols - 1) * gapX;
  const startX = (W - totalW) / 2;
  const startY = 64;
  for (let r = 0; r < rows; r++) {
    for (let c = 0; c < cols; c++) {
      aliens.push({
        x: startX + c * (aw + gapX), y: startY + r * (ah + gapY),
        w: aw, h: ah, row: r, wobble: Math.random() * Math.PI * 2
      });
    }
  }
  fleetDir = 1;
  fleetSpeed = 36 + n * 9;
  bombTimer = Math.max(0.45, 1.25 - n * 0.09);
  bombs = [];
}

function resetGame() {
  score = 0;
  lives = 3;
  wave = 1;
  missiles = [];
  particles = [];
  ship.x = W / 2 - ship.w / 2;
  ship.y = H - 84;
  ship.cooldown = 0;
  ship.invincibleUntil = 0;
  shake = 0;
  startWave(wave);
  updateHud();
}

function updateHud() {
  hudScore.textContent = score;
  hudBest.textContent = best;
  hudWave.textContent = wave;
  hudLives.textContent = lives > 0 ? "❤️".repeat(lives) : "—";
  hudLives.setAttribute("aria-label", lives + (lives === 1 ? " life" : " lives"));
}

function show(overlay) {
  for (const o of [menuOverlay, overOverlay, pausedOverlay]) o.classList.add("hidden");
  if (overlay) overlay.classList.remove("hidden");
}

function startGame() {
  resetGame();
  state = STATE.PLAYING;
  show(null);
  sfx.wave();
}

function endGame() {
  state = STATE.OVER;
  const isNewBest = score > best;
  if (isNewBest) {
    best = score;
    localStorage.setItem("alienAttackBest", String(best));
  }
  updateHud();
  finalStats.textContent =
    "You scored " + score + " and reached wave " + wave + ". Best so far: " + best + ".";
  newBestEl.classList.toggle("hidden", !isNewBest);
  show(overOverlay);
  sfx.gameover();
}

function togglePause() {
  if (state === STATE.PLAYING) { state = STATE.PAUSED; show(pausedOverlay); }
  else if (state === STATE.PAUSED) { state = STATE.PLAYING; show(null); }
}

function explode(x, y, color, count) {
  for (let i = 0; i < (count || 16); i++) {
    const a = Math.random() * Math.PI * 2;
    const sp = rand(40, 190);
    particles.push({ x, y, vx: Math.cos(a) * sp, vy: Math.sin(a) * sp, life: rand(0.3, 0.7), color });
  }
}

function loseLife() {
  lives--;
  shake = 0.35;
  explode(ship.x + ship.w / 2, ship.y + ship.h / 2, "#ffb35c", 22);
  sfx.hurt();
  updateHud();
  if (lives <= 0) { endGame(); return; }
  // brief breather: clear nearby bombs and make the ship blink
  bombs = [];
  ship.invincibleUntil = elapsed + 2;
  ship.x = W / 2 - ship.w / 2;
}

// ---------- update ----------
function update(dt) {
  // starfield always drifts, even on the menu — looks alive behind the panel
  for (const s of stars) {
    s.y += s.s * dt;
    if (s.y > H) { s.y = -2; s.x = Math.random() * W; }
  }
  if (state !== STATE.PLAYING) return;
  elapsed += dt;
  if (shake > 0) shake = Math.max(0, shake - dt);

  // --- ship movement (keyboard + touch buttons + drag) ---
  let dx = 0, dy = 0;
  if (keys.ArrowLeft || keys.KeyA || touch.left) dx -= 1;
  if (keys.ArrowRight || keys.KeyD || touch.right) dx += 1;
  if (keys.ArrowUp || keys.KeyW) dy -= 1;
  if (keys.ArrowDown || keys.KeyS) dy += 1;
  if (dx && dy) { dx *= Math.SQRT1_2; dy *= Math.SQRT1_2; }
  ship.x += dx * ship.speed * dt;
  ship.y += dy * ship.speed * dt;
  if (touch.dragX !== null) ship.x = touch.dragX - ship.w / 2;
  ship.x = clamp(ship.x, 4, W - ship.w - 4);
  ship.y = clamp(ship.y, H * 0.45, H - ship.h - 8);

  // --- firing ---
  ship.cooldown -= dt;
  if ((keys.Space || touch.fire) && ship.cooldown <= 0) {
    missiles.push({ x: ship.x + ship.w / 2 - 3, y: ship.y - 14, w: 6, h: 14, speed: 560 });
    ship.cooldown = 0.22;
    sfx.shoot();
  }

  // --- missiles ---
  for (const m of missiles) m.y -= m.speed * dt;
  missiles = missiles.filter(m => m.y + m.h > 0);

  // --- fleet movement: slide sideways, drop a step at the edges ---
  let drop = 0;
  if (aliens.length) {
    let minX = Infinity, maxX = -Infinity;
    for (const a of aliens) { minX = Math.min(minX, a.x); maxX = Math.max(maxX, a.x + a.w); }
    if ((fleetDir > 0 && maxX >= W - 10) || (fleetDir < 0 && minX <= 10)) { fleetDir *= -1; drop = 20; }
    const vx = fleetDir * fleetSpeed * dt;
    for (const a of aliens) { a.x += vx; a.y += drop; a.wobble += dt * 3; }
  }

  // --- alien bombs ---
  bombTimer -= dt;
  if (bombTimer <= 0 && aliens.length) {
    const shooter = aliens[Math.floor(Math.random() * aliens.length)];
    bombs.push({ x: shooter.x + shooter.w / 2 - 3, y: shooter.y + shooter.h, w: 6, h: 12, speed: rand(170, 230) + wave * 12 });
    bombTimer = Math.max(0.4, (1.25 - wave * 0.09)) * rand(0.6, 1.3);
  }
  for (const b of bombs) b.y += b.speed * dt;
  bombs = bombs.filter(b => b.y < H + 16);

  // --- collisions: missiles vs aliens ---
  for (let i = missiles.length - 1; i >= 0; i--) {
    let hit = false;
    for (let j = aliens.length - 1; j >= 0; j--) {
      if (overlap(missiles[i], aliens[j])) {
        explode(aliens[j].x + aliens[j].w / 2, aliens[j].y + aliens[j].h / 2, "#7ee787", 16);
        aliens.splice(j, 1);
        score += 100;
        hit = true;
        sfx.boom();
        break;
      }
    }
    if (hit) missiles.splice(i, 1);
  }

  // --- collisions: bombs / aliens vs ship ---
  const invincible = elapsed < ship.invincibleUntil;
  if (!invincible) {
    for (let i = 0; i < bombs.length; i++) {
      if (overlap(bombs[i], ship)) { bombs.splice(i, 1); loseLife(); break; }
    }
  }
  if (state !== STATE.PLAYING) return; // loseLife() may have ended the game

  if (!invincible) {
    for (const a of aliens) {
      if (overlap(a, ship)) { loseLife(); break; }
    }
  }
  if (state !== STATE.PLAYING) return;

  // aliens landing = game over, same as losing your last ship
  for (const a of aliens) {
    if (a.y + a.h >= H - 56) { lives = 0; updateHud(); endGame(); return; }
  }

  // --- wave cleared ---
  if (aliens.length === 0) {
    wave++;
    score += 250; // wave bonus
    startWave(wave);
    updateHud();
    sfx.wave();
  }

  // --- particles ---
  for (const p of particles) { p.x += p.vx * dt; p.y += p.vy * dt; p.life -= dt; }
  particles = particles.filter(p => p.life > 0);

  updateHud();
}

// ---------- drawing ----------
function drawShip() {
  const { x, y, w } = ship;
  const cx = x + w / 2;
  // blink while invincible after a hit
  if (elapsed < ship.invincibleUntil && Math.floor(elapsed * 12) % 2 === 0) ctx.globalAlpha = 0.35;

  // engine flame
  ctx.fillStyle = "#ffb35c";
  ctx.beginPath();
  ctx.moveTo(cx - 6, y + 40);
  ctx.lineTo(cx, y + 52 + Math.random() * 7);
  ctx.lineTo(cx + 6, y + 40);
  ctx.closePath();
  ctx.fill();

  // wings
  ctx.fillStyle = "#2f6fd0";
  ctx.beginPath();
  ctx.moveTo(x + 2, y + 40);
  ctx.lineTo(x + 14, y + 18);
  ctx.lineTo(x + 14, y + 40);
  ctx.closePath();
  ctx.moveTo(x + w - 2, y + 40);
  ctx.lineTo(x + w - 14, y + 18);
  ctx.lineTo(x + w - 14, y + 40);
  ctx.closePath();
  ctx.fill();

  // body
  const grad = ctx.createLinearGradient(x, y, x, y + 44);
  grad.addColorStop(0, "#dbe7ff");
  grad.addColorStop(1, "#7ea6e8");
  ctx.fillStyle = grad;
  ctx.beginPath();
  ctx.moveTo(cx, y);
  ctx.lineTo(x + w - 12, y + 40);
  ctx.lineTo(x + 12, y + 40);
  ctx.closePath();
  ctx.fill();

  // cockpit
  ctx.fillStyle = "#173a6b";
  ctx.beginPath();
  ctx.ellipse(cx, y + 18, 6, 9, 0, 0, Math.PI * 2);
  ctx.fill();

  ctx.globalAlpha = 1;
}

function drawAlien(a) {
  const cx = a.x + a.w / 2;
  const bob = Math.sin(a.wobble) * 2;
  const top = a.y + bob;

  // dome
  ctx.fillStyle = "#7ee787";
  ctx.beginPath();
  ctx.ellipse(cx, top + 10, 11, 9, 0, Math.PI, 0);
  ctx.fill();
  // eyes in the dome
  ctx.fillStyle = "#0b1026";
  ctx.beginPath();
  ctx.ellipse(cx - 4, top + 6, 2, 3, 0, 0, Math.PI * 2);
  ctx.ellipse(cx + 4, top + 6, 2, 3, 0, 0, Math.PI * 2);
  ctx.fill();

  // saucer body — top rows are green, deeper rows shift colour a bit
  ctx.fillStyle = a.row % 2 === 0 ? "#39d353" : "#2ea043";
  ctx.beginPath();
  ctx.ellipse(cx, top + 16, a.w / 2, 9, 0, 0, Math.PI * 2);
  ctx.fill();
  ctx.fillStyle = "#1d7a38";
  ctx.beginPath();
  ctx.ellipse(cx, top + 20, a.w / 2 - 7, 6, 0, 0, Math.PI);
  ctx.fill();

  // blinking rim lights
  ctx.fillStyle = Math.floor(elapsed * 4 + a.wobble) % 2 === 0 ? "#ffe08a" : "#c98f1b";
  for (const off of [-15, 0, 15]) {
    ctx.beginPath();
    ctx.arc(cx + off, top + 16, 2.4, 0, Math.PI * 2);
    ctx.fill();
  }
}

function draw() {
  ctx.save();
  if (shake > 0) ctx.translate(rand(-1, 1) * shake * 14, rand(-1, 1) * shake * 14);

  // space
  const bgGrad = ctx.createLinearGradient(0, 0, 0, H);
  bgGrad.addColorStop(0, "#05070f");
  bgGrad.addColorStop(1, "#0b1030");
  ctx.fillStyle = bgGrad;
  ctx.fillRect(-20, -20, W + 40, H + 40);

  // stars
  ctx.fillStyle = "#ffffff";
  for (const s of stars) {
    ctx.globalAlpha = 0.35 + s.r * 0.3;
    ctx.fillRect(s.x, s.y, s.r, s.r);
  }
  ctx.globalAlpha = 1;

  // ground line — aliens reaching it ends the game
  ctx.fillStyle = "rgba(57, 211, 83, 0.25)";
  ctx.fillRect(0, H - 56, W, 2);

  for (const a of aliens) drawAlien(a);

  // missiles
  ctx.fillStyle = "#ffe08a";
  for (const m of missiles) {
    ctx.fillRect(m.x, m.y, m.w, m.h);
    ctx.fillStyle = "#ff9d5c";
    ctx.fillRect(m.x + 1, m.y + m.h - 4, m.w - 2, 4);
    ctx.fillStyle = "#ffe08a";
  }

  // bombs
  ctx.fillStyle = "#ff6b6b";
  for (const b of bombs) {
    ctx.beginPath();
    ctx.ellipse(b.x + b.w / 2, b.y + b.h / 2, b.w / 2, b.h / 2, 0, 0, Math.PI * 2);
    ctx.fill();
  }

  // particles
  for (const p of particles) {
    ctx.globalAlpha = Math.max(0, p.life * 1.6);
    ctx.fillStyle = p.color;
    ctx.fillRect(p.x - 2, p.y - 2, 4, 4);
  }
  ctx.globalAlpha = 1;

  if (state !== STATE.OVER) drawShip();

  ctx.restore();
}

// ---------- main loop ----------
let lastTime = performance.now();
function frame(now) {
  let dt = (now - lastTime) / 1000;
  lastTime = now;
  if (dt > 0.05) dt = 0.05; // tab was away / hiccup — don't teleport everything
  update(dt);
  draw();
  requestAnimationFrame(frame);
}

// ---------- input ----------
window.addEventListener("keydown", (e) => {
  if (["ArrowLeft", "ArrowRight", "ArrowUp", "ArrowDown", "Space"].includes(e.code)) e.preventDefault();
  keys[e.code] = true;
  if (e.code === "KeyP" || e.code === "Escape") togglePause();
  if (e.code === "KeyM") toggleSound();
  if (e.code === "Enter" && state === STATE.MENU) startGame();
  if (e.code === "KeyR" && state === STATE.OVER) startGame();
  if (e.code === "Space" && state === STATE.MENU) startGame();
});
window.addEventListener("keyup", (e) => { keys[e.code] = false; });
window.addEventListener("blur", () => { keys = {}; if (state === STATE.PLAYING) togglePause(); });

// touch buttons
function bindHold(id, prop) {
  const el = document.getElementById(id);
  const on = (e) => { e.preventDefault(); touch[prop] = true; };
  const off = (e) => { e.preventDefault(); touch[prop] = false; };
  el.addEventListener("pointerdown", on);
  el.addEventListener("pointerup", off);
  el.addEventListener("pointerleave", off);
  el.addEventListener("pointercancel", off);
}
bindHold("leftBtn", "left");
bindHold("rightBtn", "right");
bindHold("fireBtn", "fire");

// drag on the canvas to steer
function canvasX(clientX) {
  const r = canvas.getBoundingClientRect();
  return (clientX - r.left) * (W / r.width);
}
canvas.addEventListener("pointerdown", (e) => { touch.dragX = canvasX(e.clientX); });
canvas.addEventListener("pointermove", (e) => { if (touch.dragX !== null) touch.dragX = canvasX(e.clientX); });
canvas.addEventListener("pointerup", () => { touch.dragX = null; });
canvas.addEventListener("pointercancel", () => { touch.dragX = null; });

// buttons
document.getElementById("playBtn").addEventListener("click", startGame);
document.getElementById("againBtn").addEventListener("click", startGame);
document.getElementById("resumeBtn").addEventListener("click", togglePause);
pauseBtn.addEventListener("click", togglePause);
soundBtn.addEventListener("click", toggleSound);

// ---------- boot ----------
// set up a first wave behind the menu so the page doesn't open on a black box
startWave(1);
if (best > 0) menuBest.textContent = "Your best score so far: " + best;
refreshSoundBtn();
updateHud();
requestAnimationFrame(frame);
