# Star Wars: The Holocron of Roan Novachez — Game Design & Implementation Plan

## Story Summary

**Setting:** Years after graduating from Jedi Academy, Roan Novachez is now a young Jedi Knight (~17–18). Set between the fall of the Republic and the rise of the Rebellion, Roan is dispatched to investigate a forgotten Jedi outpost on the Outer Rim. Strange Force disturbances and rogue droids are reported — but Roan uncovers something far more dangerous: a Sith holocron tied to ancient secrets. And he's not the only one searching for it.

**Tone:** Mix of humor (true to the books) and growing maturity. Personal growth + Star Wars action. Comic-style panel cutscenes for a nostalgic feel.

---

## Characters

| Character | Role | Notes |
|---|---|---|
| **Roan Novachez** | Player character | 17–18, confident but impulsive. Green lightsaber. Earns Force powers progressively. |
| **Master Rykon** | Mentor | Stoic Jedi archivist. Communicates via holo. |
| **Tazi** | Ally / comic relief | Local mechanic and rebel sympathizer. Brilliant. |
| **Darth Vaelen** | Main villain | Original Sith character. Wants the holocron. Final boss. |
| **Cameos** | Yoda, Cyrus, Gaiana | Via flashbacks or holo-messages. |

---

## Chapter Structure

| Chapter | Setting | Mechanic Focus |
|---|---|---|
| Prologue | Jedi Temple (flashback) | Tutorial — movement, basic attack |
| Chapter 1 | Remote moon exterior | Platforming, basic droid combat |
| Chapter 2 | Abandoned Jedi Outpost | Force Lift + Push puzzles |
| Chapter 3 | Underground ruins | Stealth, Mind Trick, environmental hazards |
| Chapter 4 | Sith Lair | Heavy combat, Force shadow enemies |
| Chapter 5 | Holocron Chamber | Final boss + emotional choice |

---

## Gameplay Mechanics

### Core Movement
- Walk / Run / Jump / Roll-dash
- Wall jump and double jump (unlocked mid-game)
- Context actions: talk, activate switches, hack terminals

### Force Powers (earned progressively)

| Power | Unlock | Mechanic |
|---|---|---|
| Force Push | Prologue | Raycast facing direction → apply impulse to physics objects + enemies |
| Force Pull | Chapter 1 | Reverse — draw objects/enemies toward Roan |
| Force Lift | Chapter 2 | Hold → levitate entities within radius; use for block puzzles |
| Mind Trick | Chapter 3 | Target NPC → override behavior for N seconds |
| Slow Time | Chapter 3 | Scale global delta time down; cosmetic blur effect |
| Force Sight | Chapter 4 | Toggle alternate render mode revealing hidden geometry/paths |

All powers draw from a shared **Force Energy (FP)** pool that regenerates slowly over time.

### Combat

**Roan's moves:**
- Light attack: fast, short hitbox (3-frame wind-up, active frames 4–6)
- Heavy attack: slow, large hitbox (8-frame wind-up, 20-frame recovery)
- Combo system: input sequence within a time window triggers chained animations
- Parry: input within a 4-frame window during an enemy attack → deflect + enemy stun
- Blaster deflect: parry during incoming bolt → reflect back at shooter

**Enemy types:**
- Imperial Security Droids — patrol AI, blaster fire, basic melee
- Wildlife creatures — terrain-aware, aggro on proximity
- Force Shadow enemies — resist regular attacks, require Force powers to expose
- Sith Acolytes / Dark Droid Generals — mini-bosses with combo patterns

**Boss fights:**
- Dark Jedi Duel: timed parry windows + Force powers
- Giant Droid Beast: environmental puzzle + combat hybrid
- Darth Vaelen: full Force + lightsaber mechanics

### Puzzle Types
- Pressure plates with Force-lifted blocks
- Energy beam redirection
- Mind Trick to make NPCs open doors or trigger devices
- Multi-part statues pulled into alignment
- Slowed-time platforming sequences

---

## Technical Design

### Tech Stack

| Tool | Purpose |
|---|---|
| **C++** | Primary language |
| **SDL2** | Window, renderer, event loop |
| **SDL2_image** | PNG texture loading |
| **SDL2_mixer** | OGG music + WAV sound effects |
| **SDL2_ttf** | Font rendering |
| **CMake** | Build system (FetchContent for SDL2 deps) |
| **Tiled** | Level editor (exports `.tmx` XML) |
| **tinyxml2** | Parse `.tmx` files (single-header, vendored) |

### Architecture: Composition-based OOP

Entities own a list of components. No deep inheritance hierarchies — adding behavior means adding a component, not creating a subclass.

```
Entity
  transform: TransformComponent  (always present)
  components: vector<unique_ptr<Component>>
  addComponent<T>(args...) -> T*
  getComponent<T>() -> T*
  update(float dt)
  render(SDL_Renderer*)

Component (base)
  owner: Entity*
  virtual update(float dt)
  virtual render(SDL_Renderer*)
```

**Components:**

| Component | Responsibility |
|---|---|
| `TransformComponent` | Position, rotation, scale |
| `SpriteComponent` | SDL_Texture + source rect |
| `AnimatorComponent` | Sprite sheet frame sequencer |
| `ColliderComponent` | AABB bounding box + collision callbacks |
| `PhysicsComponent` | Velocity, gravity, grounded state |
| `HealthComponent` | HP, `damage()`, death callback |
| `ForceUserComponent` | Energy pool, unlocked powers, cooldowns |

**Scene management:** Stack-based. GameplayScene sits at the bottom; DialogScene or PauseScene pushes on top without destroying game state. `push / pop / swap`.

**Force powers:** All implement a `ForcePower` base interface with `activate(Entity*, World*)`, `energyCost`, and `cooldown`. Registered on `ForceUserComponent`.

### Project Structure

```
star-wars-game/
├── CMakeLists.txt
├── assets/
│   ├── sprites/          # sprite sheets (.png)
│   ├── tilemaps/         # .tmx level files + tileset PNGs
│   ├── audio/            # .ogg music, .wav SFX
│   └── fonts/
├── docs/
│   └── GAME_DESIGN.md    # this file
├── src/
│   ├── main.cpp
│   ├── core/             # Game, Time, Input, AssetManager
│   ├── scene/            # Scene base, SceneManager, concrete scenes
│   ├── entity/           # Entity, Component base, all components
│   ├── entities/         # Player, Droid, SithAcolyte, etc.
│   ├── systems/          # CollisionSystem, CombatSystem, RenderSystem
│   ├── level/            # TileMap (.tmx parser), Level, Camera
│   ├── ui/               # HUD, DialogBox, Menu
│   └── force/            # ForcePower base + all six powers
└── third_party/          # tinyxml2
```

### Tilemap Layer Convention (Tiled)

| Layer Name | Type | Purpose |
|---|---|---|
| `background` | Tile | Decorative, no collision |
| `foreground` | Tile | Solid tiles, collision layer |
| `triggers` | Object | Invisible rects for events/cutscenes |
| `spawns` | Object | Entity spawn points with type property |

---

## HUD & UI

- **HP bar** — textured progress bar, top-left
- **FP bar** — below HP bar, blue/purple
- **Force power indicator** — active power icon + cooldown ring
- **Force power wheel** — hold button → radial menu of unlocked powers
- **Dialog box** — comic-panel PNG strips overlaid on gameplay
- **Minimap** — optional, added later

---

## Audio

- Retro orchestral MIDI or chiptune Star Wars-style music
- Lightsaber hum on movement/combat
- Force power whooshes on activation
- Droid beeps and mechanical SFX
- Optional character voice clips (R2-D2-style beeps for personality)

---

## Art Direction

**Target aesthetic:** Pokémon-quality pixel art environments (rich, layered, detailed tilesets) with Hollow Knight / Dead Cells-level character animation fluidity. Indie craftsmanship — not retro minimalism.

- **Tile grid:** 16×16 px tiles; characters are 48×48 px (Roan) down to 32×32 px (droids)
- **Environments:** 4-layer depth system (sky → background → midground → foreground overlay). Each chapter has a distinct color palette signature — see `docs/ART_STYLE.md`
- **Characters:** High frame-count animations (8–16 frames on attacks, not 3–5). Anticipation + keyframe + follow-through on every action. Readable silhouettes and color coding at a glance.
- **Cutscenes:** Pre-rendered comic-panel PNG strips (true to the book's visual style)
- **Atmosphere:** Simulated via background color tinting, animated particles, and additive-blend lightsaber glow — no dynamic lighting engine needed
- **Source:** Free/open packs from itch.io and OpenGameArt as a base; customized to fit the aesthetic. Full spec: `docs/ART_STYLE.md`

---

## Implementation Milestones

### Milestone 0 — Engine Skeleton
Window opens, game loop runs, can load a texture, keyboard input works.

- `CMakeLists.txt` with SDL2/SDL2_image/SDL2_mixer/SDL2_ttf via FetchContent
- `Game` class: SDL init, window/renderer creation, event loop, shutdown
- `Time`: delta time, 60fps cap
- `Input`: keyboard state snapshot each frame
- `AssetManager`: texture/audio load + string-keyed cache
- `SceneManager` + `Scene` base
- Test scene: colored rectangle on screen

**Done when:** `cmake --build build && ./build/star-wars-game` opens a 1280×720 window, delta time logs each frame, closes clean on Escape.

---

### Milestone 1 — Roan on Screen
Animated Roan sprite walks, runs, and jumps on a flat test level. Camera follows.

- `Entity` + `Component` base classes
- `TransformComponent`, `SpriteComponent`, `AnimatorComponent`
- `PhysicsComponent` (velocity, gravity, grounded flag)
- `ColliderComponent` (AABB) + `CollisionSystem` (tile collision only)
- `Player` entity — assembles components, reads `Input`
- `Camera` — follow target, lerp smoothing
- `GameplayScene` — hardcoded flat floor tiles

**Done when:** Roan walks/runs/jumps, lands on floor, camera follows, animations cycle correctly.

---

### Milestone 2 — First Real Level
Load a Tiled `.tmx` level. Multi-layer tilemap renders. Roan navigates real geometry.

- `TileMap` — parse `.tmx` XML via tinyxml2, build tile grid per layer
- `Level` — loads TileMap, reads spawn object layer
- `assets/tilemaps/chapter1.tmx` — first level built in Tiled (moon outpost exterior)
- Parallax background: scrolls at 0.5× camera speed

**Done when:** `chapter1.tmx` loads, Roan navigates real level geometry, parallax works.

---

### Milestone 3 — Combat
Roan attacks. Droids fight back. Health matters.

- `HealthComponent`
- `CombatSystem` — hitbox activation windows, parry detection, damage dispatch
- `Droid` entity — patrol ↔ alert ↔ attack AI state machine
- `HUD` — HP bar as textured progress bar

**Done when:** Roan kills a droid. A droid kills Roan. Parry deflects a blaster bolt back.

---

### Milestone 4 — Force Powers
Force Push and Pull work. FP bar drains and regenerates.

- `ForceUserComponent` — energy pool, regen, cooldown map
- `ForcePower` base + `ForcePush` + `ForcePull` implementations
- HUD update: FP bar + active power icon

**Done when:** Push knocks back a droid. Pull draws a crate toward Roan. FP bar animates correctly.

---

### Milestone 5 — Story & Audio
Comic-panel cutscene triggers. Title screen. SFX and music.

- `DialogScene` — render PNG panel strips overlaid on game
- `TitleScene` — logo + start/quit menu
- Audio hookup: lightsaber SFX on attack, Force whoosh on power use, background music

**Done when:** Entering the outpost triggers a comic cutscene. Title screen navigates correctly. SFX plays.

---

### Milestone 6+ — Open-Ended Expansion

Build in whatever order feels fun:
- Chapter 2: Force Lift + puzzle system
- Chapter 3: Stealth + Mind Trick
- Save/load system
- More enemies: Sith Acolytes, Force shadow enemies
- Boss fights
- Upgrade/collectible system (lightsaber mods, holocron lore)
- Flashback levels

---

## Debug Tooling

Keep a debug render mode (toggle `F1`) that draws all AABB colliders as wireframe rectangles over the scene. Zero cost when disabled in release. Essential for tuning hitboxes and collision geometry.
