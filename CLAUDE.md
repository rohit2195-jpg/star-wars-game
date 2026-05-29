# Star Wars: The Holocron of Roan Novachez

2D action-platformer in C++17 / SDL2. Jedi Fallen Order-style combat with
Pokémon GBA/DS-quality pixel art environments and Hollow Knight-level character
animation fluidity. Indie game feel. See `docs/` for full design specs.

## Dependencies

    brew install sdl2 sdl2_image sdl2_mixer sdl2_ttf

## Build & Run

    make          # configure + compile (debug)
    make run      # build then launch
    make play     # launch last build without rebuilding
    make release  # optimized build
    make rebuild  # clean + build from scratch
    make clean    # remove build/

## Controls (in-game)

    A / D         move left / right
    Space         jump (hold for higher arc, tap for short hop)
    Left Shift    roll / dash in facing direction
    Left Ctrl     crouch (M3+)
    Z             light attack (M3+)
    X             heavy attack (M3+)
    Q             use Force power (M4+)
    E (hold)      Force power wheel (M4+)
    F             interact (M2+)
    F1            toggle debug collider wireframes
    Escape        quit

## Milestone Validation

### M0 — Engine skeleton ✅
- Window opens at 1280×720
- Green box moves with WASD
- Escape closes cleanly

### M1 — Roan on screen (current)
- [ ] Roan character (green rect placeholder) renders on screen
- [ ] A/D moves Roan left/right; facing direction flips correctly
- [ ] Movement accelerates to run speed; friction stops when key released
- [ ] Space: tap = short hop, hold = tall arc (variable height working)
- [ ] Roan lands on the floor and does not fall through
- [ ] Left Shift rolls in facing direction (fast dash)
- [ ] Idle animation state plays when no input
- [ ] Walk animation state plays at low speed
- [ ] Run animation state plays at high speed
- [ ] Jump animations: rise → apex → fall → land (state machine works)
- [ ] Roll animation plays during roll window
- [ ] Camera follows Roan with smooth lerp (not instant snap)
- [ ] Camera lookahead: viewport shifts forward in movement direction
- [ ] F1 shows/hides green AABB wireframes over Roan and floor line
- [ ] No compilation warnings
- [ ] No crash after 60 seconds of play

### M2 — Real level (Tiled map + tileset, multiple platforms)
### M3 — Combat (lightsaber combos, enemy AI, hit/knockback)
### M4 — Force powers (6 powers, Force meter, power wheel UI)
### M5 — Audio + polish (music, SFX, particle effects, screen transitions)

## Project Structure

    src/core/         Game loop, input, asset manager, timing
    src/scene/        Scene stack (gameplay, title, dialog)
    src/entity/       Entity base class + Component base
    src/entity/components/   SpriteComponent, AnimatorComponent,
                             PhysicsComponent, ColliderComponent,
                             TransformComponent
    src/entities/     Concrete entities (Player, enemies)
    src/systems/      CollisionSystem, CombatSystem (M3+)
    src/level/        Camera, tilemap loader (M2+)
    src/ui/           HUD, menus, dialog boxes (M4+)
    src/force/        Force power base + 6 powers (M4+)
    assets/sprites/   Character + enemy sprite sheets (48×48 frames)
    assets/tilemaps/  .tmx level files + tilesets (M2+)
    assets/audio/     Music (.ogg) + SFX (.wav) (M5+)
    docs/             Design specs — read these before coding each system

## Key Architecture Notes

- Entity owns a flat `vector<unique_ptr<Component>>` — no full ECS, composition via templates
- `TransformComponent` lives directly on Entity (always present, no vtable overhead)
- `AnimatorComponent` lazy-caches its `SpriteComponent*` pointer — no per-frame getComponent scan
- Camera offset is injected into `SpriteComponent` fields (`camX/camY/zoom`) by GameplayScene
  before each render pass — keeps Component::render signature simple
- CollisionSystem is header-only static methods for M1 single-floor resolution
- SDL logical size 1280×720 + `SDL_HINT_RENDER_SCALE_QUALITY=0` = crisp pixel art at any window size
