# MVP Implementation Plan

## What "MVP" Means Here

A playable demo that proves the game is fun:

- Roan moves fluidly: walk, run, jump, wall jump, roll
- Combat: light attack, heavy attack, parry, blaster deflect
- One Force power: Force Push
- One real enemy: T1 Security Droid with patrol + attack AI
- One real level loaded from a Tiled `.tmx` file
- HP bar + Force energy bar on the HUD
- Camera follows Roan with screenshake on hits
- Basic audio: saber swing, Force Push, droid alert, footsteps
- Feels good to play — controls are responsive, combat has weight

Everything beyond this (more Force powers, more enemies, story, cutscenes, save system) is post-MVP. The goal is a single playable session that demonstrates the core loop is fun.

---

## Current Status

| Milestone | Status |
|---|---|
| **M0 — Engine skeleton** | ✅ Complete |
| **M1 — Entity/component system + Roan moving** | 🔲 Not started |
| **M2 — Tilemap loading + real level** | 🔲 Not started |
| **M3 — Combat + enemy** | 🔲 Not started |
| **M4 — Force Push + HUD** | 🔲 Not started |
| **M5 — Basic audio** | 🔲 Not started |

---

## Reference Docs (read these before coding each phase)

| Phase | Read These First |
|---|---|
| M1 — Entity/component, movement | `CONTROLS.md`, `ANIMATION_STATES.md`, `CAMERA_SYSTEM.md` |
| M2 — Tilemap | `TILEMAP_SPEC.md`, `ART_STYLE.md` |
| M3 — Combat, enemy | `COMBAT_SYSTEMS.md`, `ENEMY_AI.md` |
| M4 — Force Push, HUD | `FORCE_POWERS.md`, `HUD_AND_UI.md` |
| M5 — Audio | `AUDIO.md` |

---

## Milestone 1 — Entity System + Roan on Screen

**Goal:** Animated Roan sprite walks, runs, jumps on a flat hardcoded floor. Camera follows him.

### Step 1.1 — Component Base Class
**File:** `src/entity/Component.h`

```cpp
class Entity;

class Component {
public:
    virtual ~Component() = default;
    virtual void update(float dt) {}
    virtual void render(SDL_Renderer* renderer) {}
    Entity* owner = nullptr;
};
```

No .cpp needed — it's pure virtual with defaults.

---

### Step 1.2 — Transform Component
**File:** `src/entity/components/TransformComponent.h`

Data only — no update/render logic. Everything else reads from this.

```
Fields:
  float x, y          — world position (pixels)
  float scaleX = 1.0f
  float scaleY = 1.0f
  bool  facingRight = true
```

---

### Step 1.3 — Entity Class
**Files:** `src/entity/Entity.h`, `src/entity/Entity.cpp`

```
Fields:
  TransformComponent transform   — always present, not in the component list
  vector<unique_ptr<Component>> components
  bool active = true

Methods:
  template<typename T, typename... Args>
  T* addComponent(Args&&... args)   — construct + store + set owner + return raw ptr

  template<typename T>
  T* getComponent()                 — linear scan, return first match or nullptr

  void update(float dt)             — calls component->update(dt) for all active
  void render(SDL_Renderer*)        — calls component->render(renderer) for all active
```

---

### Step 1.4 — Sprite Component
**Files:** `src/entity/components/SpriteComponent.h/.cpp`

```
Fields:
  SDL_Texture* texture = nullptr
  SDL_Rect     srcRect            — which region of the sheet to draw
  int          drawOffsetX = 0    — pixel offset from transform.x (centers the sprite)
  int          drawOffsetY = 0

render():
  Compute dest rect from transform.x + drawOffsetX, transform.y + drawOffsetY,
  srcRect.w, srcRect.h (scaled by camera zoom — camera handles this externally).
  SDL_RenderCopyEx for horizontal flip when !transform.facingRight.
```

---

### Step 1.5 — Animator Component
**Files:** `src/entity/components/AnimatorComponent.h/.cpp`

```
Struct AnimationClip:
  string name
  int    sheetRow
  int    frameCount
  int    frameWidth, frameHeight
  float  fps
  bool   loop

Fields:
  map<string, AnimationClip> clips
  string   currentClip
  int      currentFrame = 0
  float    frameTimer   = 0.0f
  SpriteComponent* sprite = nullptr   — raw ptr, set in onEnter or first update

Methods:
  void addClip(AnimationClip clip)
  void play(const string& name)       — resets to frame 0 if different clip
  bool isFinished() const             — true if non-looping clip is on last frame
  bool isPlaying(const string& name)

update(float dt):
  frameTimer += dt
  if frameTimer >= 1.0f / clip.fps:
    advance frame
    if at end and loop: wrap to 0
    if at end and !loop: hold last frame
    update sprite->srcRect to match (row * frameHeight, frame * frameWidth)
```

---

### Step 1.6 — Physics Component
**Files:** `src/entity/components/PhysicsComponent.h/.cpp`

```
Fields:
  float velX = 0, velY = 0
  float gravity = 1800.0f        — pixels/s² (tune during testing)
  float maxFallSpeed = 800.0f
  bool  grounded = false
  bool  applyGravity = true      — false for projectiles that don't arc

update(float dt):
  if applyGravity and !grounded:
    velY += gravity * dt
    velY = min(velY, maxFallSpeed)
  transform.x += velX * dt
  transform.y += velY * dt
```

---

### Step 1.7 — Collider Component
**Files:** `src/entity/components/ColliderComponent.h/.cpp`

AABB only. No rotation.

```
Fields:
  float offsetX, offsetY    — offset from transform position
  float width, height
  bool  isTrigger = false   — trigger = no physical resolution, just overlap detection
  function<void(Entity*)> onCollide   — callback, optional

Methods:
  SDL_FRect getRect() const    — returns world-space rect (transform + offset)
```

---

### Step 1.8 — Collision System (tile collision only for now)
**Files:** `src/systems/CollisionSystem.h/.cpp`

```
For M1 (no tilemap yet): just resolve against a hardcoded floor at y=600.

resolve(Entity& entity, float floorY):
  PhysicsComponent*  phys = entity.getComponent<PhysicsComponent>()
  ColliderComponent* col  = entity.getComponent<ColliderComponent>()
  if col->getRect().bottom() >= floorY:
    entity.transform.y = floorY - col->height - col->offsetY
    phys->velY = 0
    phys->grounded = true
  else:
    phys->grounded = false
```

Will be replaced in M2 with real tilemap collision.

---

### Step 1.9 — Player Entity
**Files:** `src/entities/Player.h/.cpp`

The Player is an Entity subclass that owns all Roan's components and handles his input logic.

```
Components it creates in constructor:
  SpriteComponent   (texture loaded from AssetManager)
  AnimatorComponent (all Roan animation clips registered — see ANIMATION_STATES.md)
  PhysicsComponent
  ColliderComponent (32x44 px box, offset to center on sprite)

Input handling in update(dt, Input&):
  Horizontal movement:
    if A held: velX = -RUN_SPEED; facingRight = false
    if D held: velX = +RUN_SPEED; facingRight = true
    if neither: velX moves toward 0 (friction)

  Jumping:
    if Space pressed and grounded: velY = -JUMP_VELOCITY
    variable height: if Space released early, cap velY at -MIN_JUMP_VELOCITY

  Wall jump (M1 stub — implement fully in M2 once collision has wall detection):
    if touching wall and Space pressed: jump away from wall

  Roll:
    if Shift + direction pressed and grounded: enter roll state (12f, i-frames 3-10)

Animation state machine:
  Evaluate each frame which clip to play based on velocity + state flags.
  Priority: death > stagger > attack > roll > air states > run/walk > idle
  (See ANIMATION_STATES.md for full priority list)
```

Constants (tune these during testing — don't hardcode magic numbers):
```
RUN_SPEED       = 280.0f   pixels/s
WALK_SPEED      = 140.0f   pixels/s (auto — not a separate button)
JUMP_VELOCITY   = 620.0f   pixels/s upward
MIN_JUMP_VEL    = 200.0f   pixels/s (short tap jump cap)
ROLL_DURATION   = 0.20f    seconds
GRAVITY         = 1800.0f  pixels/s²
```

---

### Step 1.10 — Camera
**Files:** `src/level/Camera.h/.cpp`

```
Fields:
  float x, y           — top-left corner in world space
  int   viewW, viewH   — viewport size (= window size)
  float zoom = 2.0f    — 2× zoom: 16px tiles render as 32px on screen
  float lerpSpeed = 0.08f
  float lookAheadX = 80.0f
  Entity* target = nullptr
  float shakeIntensity = 0, shakeDuration = 0, shakeTimer = 0
  float shakeOffsetX = 0, shakeOffsetY = 0

update(float dt):
  targetX = target->transform.x - (viewW/2/zoom) + lookAheadX * (facingRight ? 1 : -1)
  x += (targetX - x) * lerpSpeed
  [clamp to level bounds once tilemap is loaded in M2]
  [apply screenshake offset — see CAMERA_SYSTEM.md]

worldToScreen(float wx, float wy) -> {sx, sy}:
  sx = (wx - x) * zoom + shakeOffsetX
  sy = (wy - y) * zoom + shakeOffsetY
```

---

### Step 1.11 — Gameplay Scene (flat floor version)
**Files:** `src/scene/GameplayScene.h/.cpp`

For M1: hardcoded flat floor at y=600. No tilemap yet.

```
Members:
  Player        player
  Camera        camera
  CollisionSystem collision

onEnter():
  player.init(assets)     — load sprite sheet, register animation clips
  camera.target = &player
  camera.viewW = 1280, viewH = 720

update(dt, input):
  player.update(dt, input)
  collision.resolve(player, FLOOR_Y=600.0f)
  camera.update(dt)

render(renderer):
  draw floor rect (hardcoded brown rectangle)
  draw player via camera.worldToScreen()
```

Replace GameplayScene::update with real tilemap in M2.

### M1 Done When:
- [ ] Roan walks left/right (A/D), speed increases to run after 0.3s
- [ ] Jump (Space) — tap = short, hold = full height
- [ ] Roll (Shift) — dashes in movement direction
- [ ] Lands on hardcoded floor, doesn't fall through
- [ ] Animation changes correctly: idle → walk → run → jump → fall → land
- [ ] Camera follows with smooth lerp and lookahead
- [ ] No crash when reaching screen edges

---

## Milestone 2 — Tilemap Loading + Real Level

**Goal:** Replace the hardcoded floor with a `.tmx` level from Tiled. Roan navigates real geometry.

### Step 2.1 — Vendor tinyxml2
**File:** `third_party/tinyxml2.h`, `third_party/tinyxml2.cpp`

Download from https://github.com/leethomason/tinyxml2 (two files only).
Add to `CMakeLists.txt`:
```cmake
target_sources(${PROJECT_NAME} PRIVATE third_party/tinyxml2.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE third_party)
```

---

### Step 2.2 — TileMap Class
**Files:** `src/level/TileMap.h/.cpp`

```
Struct Tile:
  int tileId        — 0 = empty
  bool solid        — from tileset property
  bool hazard       — instant death on touch

Fields:
  int tileWidth, tileHeight          — from map (16, 16)
  int mapWidthTiles, mapHeightTiles
  vector<vector<Tile>> foreground    — collision layer
  vector<vector<int>>  background    — draw-only layer
  SDL_Texture* tilesetTexture        — the tileset PNG
  int tilesetColumns                 — tiles per row in tileset PNG

Methods:
  bool load(const string& tmxPath, AssetManager& assets)
  void renderLayer(SDL_Renderer*, const vector<vector<int>>& layer, Camera&)
  bool isSolidAt(float worldX, float worldY)      — for collision queries
  SDL_FRect getBoundsPixels()                      — level size in pixels
```

Parse order (tinyxml2):
1. Read `<map>` attrs: tilewidth, tileheight, width, height
2. Read `<tileset>` → load tileset PNG, record firstgid, tilesetColumns
3. Read `<layer name="background">` → fill background grid
4. Read `<layer name="foreground">` → fill foreground grid (set solid=true for all non-zero)
5. Read `<objectgroup name="spawns">` → populate spawn list (vector of {type, x, y})

---

### Step 2.3 — Tilemap Collision (replace Step 1.8)
**Update:** `src/systems/CollisionSystem.h/.cpp`

Replace hardcoded floor with tile-based collision:

```
resolveVsMap(Entity& entity, TileMap& map):
  For each of the 4 corner points of the entity's collider rect:
    Check map.isSolidAt(corner)
    If solid:
      Resolve overlap (push entity out of the tile)
      Zero the velocity component in that direction
      Set grounded=true if collision was on the bottom edge
```

---

### Step 2.4 — Level Class
**Files:** `src/level/Level.h/.cpp`

```
Fields:
  TileMap tilemap
  vector<SpawnPoint> spawns   — {string type, float x, float y}

Methods:
  bool load(const string& tmxPath, AssetManager& assets)
  void render(SDL_Renderer*, Camera&)    — renders background then foreground
  const TileMap& getMap()
```

---

### Step 2.5 — Install SDL2_image + Update AssetManager
```
brew install sdl2_image
make rebuild
```
`AssetManager::getTexture()` already handles this via `#ifdef HAS_SDL2_IMAGE` — no code change needed.

---

### Step 2.6 — Design Chapter 1 in Tiled
- Download Tiled: https://www.mapeditor.org/
- Create `assets/tilemaps/ch1_arrival.tmx`
- Use any free 16×16 scifi/ruins tileset from itch.io as placeholder
- Follow the layer convention in `TILEMAP_SPEC.md`: background / foreground / triggers / spawns
- Add a `player_start` spawn object

---

### Step 2.7 — Update GameplayScene
Replace hardcoded floor with:
```
Level level
level.load("assets/tilemaps/ch1_arrival.tmx", assets)
camera.setBounds(level.getMap().getBoundsPixels())
// spawn Roan at the player_start spawn point
```

### M2 Done When:
- [ ] `ch1_arrival.tmx` loads without crash
- [ ] Roan walks on real tile geometry (platforms, ledges)
- [ ] Falls off ledge edges correctly
- [ ] Camera clamps to level bounds
- [ ] Parallax background layers visible

---

## Milestone 3 — Combat + Enemy

**Goal:** Roan attacks, a T1 Droid patrols and fights back, HP matters.

### Step 3.1 — Health Component
**Files:** `src/entity/components/HealthComponent.h/.cpp`

```
Fields:
  int hp, maxHp
  bool dead = false
  float invincibilityTimer = 0   — seconds of i-frames after a hit
  function<void()> onDeath       — callback

Methods:
  void damage(int amount)
    if invincibilityTimer > 0: return
    hp -= amount
    invincibilityTimer = 0.5f    — 30f of i-frames
    if hp <= 0: dead = true; onDeath()

update(float dt):
  invincibilityTimer -= dt (clamp to 0)
```

---

### Step 3.2 — Hitbox Component
**Files:** `src/entity/components/HitboxComponent.h/.cpp`

A hitbox is an AABB that is only active for a specific window of frames during an attack animation. Separate from the ColliderComponent (which is the entity's body).

```
Fields:
  SDL_FRect rect          — in world space, relative to transform
  int       damage
  bool      active = false
  int       activeStartFrame, activeEndFrame   — clip-relative frames
  string    ownerClip     — which animation clip this hitbox belongs to

Methods:
  bool isActiveOnFrame(int frame) const
  SDL_FRect getWorldRect(TransformComponent&) const
```

---

### Step 3.3 — Combat System
**Files:** `src/systems/CombatSystem.h/.cpp`

```
update(vector<Entity*>& entities):
  For each attacker entity with a HitboxComponent:
    if hitbox.active:
      For each other entity with HealthComponent + ColliderComponent:
        if hitbox.getWorldRect() overlaps target.collider.getRect():
          target.health.damage(hitbox.damage)
          apply knockback impulse to target physics
          trigger hitstop (4f pause via Time multiplier)
          screenshake (camera.shake(2.0, 0.15))
```

---

### Step 3.4 — Player Combat (add to Player::update)
**Add to Player:**

```
Attack state machine:
  if Z pressed and not in recovery:
    determine which combo step (track last attack + time since last)
    set animatorComponent->play("attack_light_1") etc.
    activate hitbox for that clip's active frames (frames 6-9 for light_1)

  if X pressed:
    play "attack_heavy"
    activate hitbox frames 9-13

Parry:
  if A pressed:
    enter parry_window state (active for 4 frames)
    if enemy hitbox overlaps during those 4 frames:
      play "parry" clip
      enemy enters STAGGERED state for 30 frames
      no damage taken

Combo tracking:
  comboStep: 0–3
  comboTimer: resets on each hit landed; resets to 0 after 30 frames idle
  On hit: if comboTimer > 0 and comboStep < 3: comboStep++
```

---

### Step 3.5 — AI Component (base)
**Files:** `src/entity/components/AIComponent.h/.cpp`

```
enum class AIState { PATROL, SUSPICIOUS, ALERTED, COMBAT_APPROACH, COMBAT_ATTACK,
                     COMBAT_RECOVER, STAGGERED, DEAD }

Fields:
  AIState state = PATROL
  float   stateTimer = 0
  Entity* target = nullptr     — set to player when alerted
  vector<glm::vec2> patrolPath — waypoints (set from level spawns data)
  int     patrolIndex = 0

update(float dt, Entity& player, TileMap& map):
  run detection checks
  transition states based on conditions (see ENEMY_AI.md for full table)
  execute movement/attack for current state
```

---

### Step 3.6 — T1 Security Droid Entity
**Files:** `src/entities/enemies/Droid.h/.cpp`

```
Components: SpriteComponent, AnimatorComponent, PhysicsComponent,
            ColliderComponent, HealthComponent, AIComponent, HitboxComponent

HP: 50
Detection: sightRange=8 tiles, sightAngle=120°, hearingRadius=4 tiles

Attack set:
  blaster_burst:  fires a blaster bolt projectile every 1.5s when in COMBAT_ATTACK
  melee_swing:    HitboxComponent active frames 10-14, 20 damage, 20f windup
  rush_charge:    PhysicsComponent velX = 400 for 0.3s, hitbox active whole duration

AI state logic (simplified for M3):
  PATROL: walk patrol path at 80px/s
  ALERTED: run toward player at 200px/s
  COMBAT_ATTACK: if player in melee range → melee_swing; else → blaster_burst
  COMBAT_RECOVER: wait recoverFrames then go back to COMBAT_APPROACH
  STAGGERED: wait staggerFrames (30), return to COMBAT_APPROACH
```

---

### Step 3.7 — Blaster Bolt Projectile
**Files:** `src/entities/BlasterBolt.h/.cpp`

```
Components: SpriteComponent, PhysicsComponent (applyGravity=false), ColliderComponent

Behavior:
  Moves at 400px/s in fire direction
  On hitting a wall tile: destroy self
  On hitting player: damage player 12HP, destroy self
  On hitting a parry hitbox: reverse direction (reflects back at droid)
  Self-destructs after 3s regardless
```

---

### Step 3.8 — Spawn enemies from level data
Update `GameplayScene::onEnter()`:
```
For each spawn in level.spawns:
  if type == "droid_t1":
    create Droid at (x, y)
    assign patrol path from navigation layer if present
  if type == "player_start":
    place Roan here
```

---

### Step 3.9 — HUD (HP bar only for now)
**Files:** `src/ui/HUD.h/.cpp`

```
render(SDL_Renderer*, Player& player):
  Draw background bar rect at (16, 16), 200×16 px, color #333333
  Draw fill rect: width = 200 * (hp / maxHp), color based on HP%:
    >60%: green  #4CAF50
    >30%: orange #FF9800
    <=30%: red   #F44336
  Draw "HP" label using SDL_ttf (or skip label if SDL2_ttf not installed yet)
```

See `HUD_AND_UI.md` for full layout spec.

### M3 Done When:
- [ ] Roan's light attack plays and has a hitbox (damages droids)
- [ ] Heavy attack plays and has a hitbox
- [ ] Parry: timed input during droid attack = stagger (30 frames), no damage taken
- [ ] Parry: deflects a blaster bolt back at the droid (droid takes damage)
- [ ] T1 Droid patrols a path
- [ ] Droid alerts when player enters range
- [ ] Droid dies (death animation, then entity removed)
- [ ] Roan can die (death animation)
- [ ] HP bar reflects current HP correctly
- [ ] Hitstop: 4-frame pause on hit lands
- [ ] Screenshake on hit

---

## Milestone 4 — Force Push + Full HUD

**Goal:** First Force power works. FP bar on HUD. Knockback satisfying.

### Step 4.1 — Force User Component
**Files:** `src/entity/components/ForceUserComponent.h/.cpp`

```
Fields:
  float fp = 100.0f, maxFp = 100.0f
  float regenRate = 5.0f           — FP per second
  float regenPauseTimer = 0        — paused after using a power or taking a hit
  map<string, float> cooldowns     — powerName → seconds remaining

update(float dt):
  regenPauseTimer -= dt (clamp 0)
  if regenPauseTimer <= 0:
    fp = min(fp + regenRate * dt, maxFp)
  for each cooldown: reduce by dt, clamp to 0

bool canUse(const string& powerName, float cost):
  return fp >= cost && cooldowns[powerName] <= 0

void spend(float cost, const string& powerName, float cooldown):
  fp -= cost
  cooldowns[powerName] = cooldown
  regenPauseTimer = 0.5f
```

---

### Step 4.2 — Force Power Base
**File:** `src/force/ForcePower.h`

```cpp
class ForcePower {
public:
    virtual ~ForcePower() = default;
    float energyCost  = 15.0f;
    float cooldown    = 0.5f;
    virtual void activate(Entity* caster, vector<Entity*>& entities, TileMap& map) = 0;
};
```

---

### Step 4.3 — Force Push
**Files:** `src/force/powers/ForcePush.h/.cpp`

```
activate(caster, entities, map):
  origin = caster->transform position
  direction = caster facingRight ? +1 : -1
  range = 6 * 16 px (6 tiles)

  For each entity in entities (except caster):
    if entity has ColliderComponent:
      if entity's rect overlaps the push ray (thin rect, 1 tile wide, 6 tiles long):
        if entity has PhysicsComponent:
          apply impulse: velX += direction * 600, velY = -200  (slight upward arc)
        if entity has HealthComponent:
          entity.health.damage(10)

  Play "force_push" animation on caster
  Spawn ForcePushEffect (animated sprite, plays once and destroys self)
```

---

### Step 4.4 — Wire Force Push to Player input
**Add to Player::update:**
```
if Q pressed:
  ForceUserComponent* fuc = getComponent<ForceUserComponent>()
  if fuc->canUse("force_push", 15.0f):
    ForcePush push
    push.activate(this, scene.entities, scene.level.getMap())
    fuc->spend(15.0f, "force_push", 0.5f)
    animator->play("force_push")
```

---

### Step 4.5 — Full HUD (add FP bar + power indicator)
**Update:** `src/ui/HUD.h/.cpp`

Add to render():
```
FP bar: position (16, 40), 200×12 px, color #3F9FFF
Fill:   width = 200 * (fp / maxFp)

Active power icon: position (1180, 660), 32×32 px
  Draw power icon texture
  Draw cooldown arc overlay: pie-slice mask over the icon, shrinks as cooldown expires
  Draw "Q" key hint above it
```

### M4 Done When:
- [ ] Q key triggers Force Push
- [ ] Force Push knocks droids back with a visible impulse
- [ ] Force Push deals 10 damage and visually shows a wave effect
- [ ] FP bar drains on use, regenerates over time
- [ ] FP bar regeneration pauses 0.5s after using power or taking a hit
- [ ] Cooldown ring animates over power icon
- [ ] Pushing a droid into a wall adds collision damage

---

## Milestone 5 — Basic Audio

**Goal:** The game sounds like a Star Wars game. Saber hum, Force whoosh, footsteps, droid death.

### Step 5.1 — Install SDL2_mixer
```
brew install sdl2_mixer
make rebuild
```

---

### Step 5.2 — AudioManager
**Files:** `src/core/AudioManager.h/.cpp`

```
Fields:
  map<string, Mix_Chunk*> sfx
  map<string, Mix_Music*> music
  Mix_Music* currentMusic = nullptr

Methods:
  bool init()                                    — Mix_OpenAudio
  void loadSFX(const string& id, const string& path)
  void loadMusic(const string& id, const string& path)
  void playSFX(const string& id, int channel=-1, int loops=0)
  void playMusic(const string& id, bool fade=true)
  void stopMusic(int fadeMs=500)
  void freeAll()
```

Channel assignments match `AUDIO.md`:
- Channel 0: saber hum (loops continuously)
- Channel 1: footsteps
- Channel 3: Force powers + player attacks
- Channels 4–7: enemy sounds

---

### Step 5.3 — Audio hookup points
Wire `AudioManager::playSFX()` calls at these points:

| Trigger | Sound | Where |
|---|---|---|
| GameplayScene::onEnter | saber hum loop | Ch 0, `loops=-1` |
| Player light attack active frame | `sfx_saber_swing_light` | Ch 3 |
| Player heavy attack active frame | `sfx_saber_swing_heavy` | Ch 3 |
| Player parry success | `sfx_saber_parry` | Ch 3 |
| Player takes hit | `sfx_hit_roan` | Ch 3 |
| Player jumps | `sfx_jump` | Ch 2 |
| Player lands | `sfx_land` | Ch 2 |
| Player footstep (every 8f running) | `sfx_footstep_stone` | Ch 1 |
| Force Push activated | `sfx_force_push` | Ch 3 |
| Droid alerted | `sfx_droid_alert` | Ch 4 |
| Droid fires blaster | `sfx_blaster_fire` | Ch 8 |
| Droid dies | `sfx_droid_death` | Ch 4 |
| Blaster deflected | `sfx_blaster_deflect` | Ch 8 |

---

### Step 5.4 — Background music
```
GameplayScene::onEnter():
  audioManager.playMusic("music_ch1_exterior")

// On level transition later:
  audioManager.playMusic("music_ch2_outpost", fade=true)
```

### M5 Done When:
- [ ] Saber hum loops from level start
- [ ] Saber swing SFX plays on attack active frames
- [ ] Force Push has a whoosh sound
- [ ] Droid alert sound on aggro
- [ ] Blaster deflect sound on parry
- [ ] Background music loops for Chapter 1

---

## Definition of MVP Complete

The MVP is done when all of the following are true in a single play session:

1. **Roan moves** — walk, run, jump (variable height), wall jump, roll — all feel responsive and fluid
2. **Roan attacks** — light combo (3 hits), heavy attack — all have hitboxes and satisfying sound
3. **Parry works** — timed parry staggers enemy and deflects blaster bolts
4. **Force Push works** — knocks droids back, costs FP, has cooldown, has sound
5. **Droids work** — patrol, alert, attack with blaster and melee, die convincingly
6. **Real level** — loaded from a Tiled `.tmx` file, Roan navigates real geometry
7. **HUD** — HP bar + FP bar update correctly, power indicator with cooldown ring
8. **Audio** — saber hum, attack sounds, Force Push whoosh, droid sounds, music
9. **Camera** — follows Roan with smooth lerp, screenshakes on big hits, clamps to level bounds
10. **Animations** — all clips transition correctly, no animation getting stuck

---

## What Comes After MVP

Once all 10 criteria above are met, the natural next additions (in rough priority order):

1. More Force powers (Force Pull → Force Lift → Mind Trick)
2. More enemy types (Stormtrooper → Force Shadow → Sith Acolyte)
3. Remaining chapters (level design in Tiled)
4. Save system (Meditation Points)
5. Progression system (Force power upgrades, collectibles)
6. Story layer (dialog boxes, cutscene panels)
7. More polish (particle effects, lightsaber glow, screen transitions)
