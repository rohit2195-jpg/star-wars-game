# Tilemap Specification

## Overview

Levels are designed in **Tiled Map Editor** (free, cross-platform: https://www.mapeditor.org/). Tiled exports `.tmx` files — XML documents that describe layers, tilesets, and object data. Our `TileMap` class parses these at runtime using **tinyxml2**.

---

## File Structure

```
assets/tilemaps/
├── chapter1.tmx          ← level file
├── chapter1_bg.png       ← background tileset (decorative)
├── chapter1_fg.png       ← foreground tileset (solid tiles)
├── shared_objects.tsx    ← reusable object definitions (Tiled tileset)
└── chapter2.tmx
```

Each level's `.tmx` file references its tileset PNGs by relative path. Both the `.tmx` and its tilesets live in `assets/tilemaps/`.

---

## Layer Convention

Every `.tmx` file must contain these layers **in this order** (bottom to top in Tiled):

| Layer Name | Type | Purpose | Collision |
|---|---|---|---|
| `background` | Tile Layer | Decorative backdrop (sky, distant structures) | None |
| `midground` | Tile Layer | Secondary decoration (rocks, props in back) | None |
| `foreground` | Tile Layer | Solid terrain the player walks on | Yes (all tiles) |
| `overlay` | Tile Layer | Foreground decoration rendered above the player | None |
| `triggers` | Object Layer | Invisible rectangles — cutscene zones, zone transitions, environmental events | Special (see below) |
| `spawns` | Object Layer | Entity spawn points | None |
| `navigation` | Object Layer | Pathfinding waypoints for enemy patrol routes | None |

**Rendering order:** background → midground → foreground → Player/Entities → overlay

The `overlay` layer renders above all entities. Use it for stalactites, hanging vines, foreground pillars — things that should feel "in front of" the player.

---

## Tile Dimensions

| Parameter | Value |
|---|---|
| Tile size | 16×16 px |
| Typical tileset sheet | 256×256 px (16×16 grid of tiles = 256 unique tiles) |
| Level width (typical) | 100–200 tiles wide |
| Level height (typical) | 20–40 tiles tall |

---

## Collision Detection

Only tiles on the `foreground` layer produce collision. The `TileMap` class builds a 2D boolean grid from the `foreground` layer on load. Every non-empty tile in that layer is treated as a solid 16×16 block.

**No per-tile collision shapes** — all foreground tiles are axis-aligned rectangles. Slopes, one-way platforms, and other custom shapes are handled via object layer triggers (see below), not tile shapes.

### One-Way Platforms

One-way platforms (jump up through them, land on top, fall through by pressing down + jump) are defined as rectangle objects in the `triggers` layer with the property:

```
type = "one_way_platform"
```

The object's rectangle defines the platform bounds. Entities only collide with the top edge of these rectangles.

---

## Tileset File Format

Tilesets are `.png` files with a `.tsx` sidecar file (Tiled Tileset XML). The `.tsx` defines tile IDs and optional properties.

**Required tileset properties per tile:**

| Property | Type | Required | Values |
|---|---|---|---|
| `solid` | bool | Yes (foreground tiles) | true / false |
| `hazard` | bool | No | true = instant death on touch |
| `animated` | bool | No | true = tile animates (e.g. lava, water) |
| `animation_fps` | int | If animated | e.g. 8 |

Hazard tiles (lava, spikes) are placed in the `foreground` layer but marked `hazard = true`. The collision system checks this flag and triggers death rather than physical blocking.

---

## Triggers Layer — Object Properties

Objects in the `triggers` layer are invisible rectangles with a `type` property that tells the game what to do when Roan's collider overlaps them.

| `type` value | Behavior | Required Properties |
|---|---|---|
| `zone_transition` | Load a new level | `target_level` (string), `spawn_point` (string) |
| `cutscene` | Play a comic-panel cutscene | `cutscene_id` (string) |
| `checkpoint` | Auto-save without a Meditation Point | none |
| `one_way_platform` | Jump-through platform | none |
| `hazard_zone` | Deals damage while inside | `damage_per_second` (int) |
| `death_zone` | Instant death (out-of-bounds pits) | none |
| `ability_gate` | Blocks path until ability unlocked | `required_ability` (string) |
| `ambient_zone` | Changes background music/ambience | `audio_id` (string) |
| `force_echo` | Plays a Force Echo vision | `echo_id` (string) |
| `dialogue_trigger` | Starts NPC dialogue | `npc_id` (string), `dialogue_id` (string) |

---

## Spawns Layer — Object Properties

Objects in the `spawns` layer define where entities are created when the level loads.

| Property | Type | Required | Description |
|---|---|---|---|
| `type` | string | Yes | Entity type to spawn (see list below) |
| `id` | string | Yes | Unique ID within the level (for referencing in triggers) |
| `facing` | string | No | `left` or `right` (default: `right`) |
| `patrol_path` | string | No | Name of a `navigation` path object this enemy follows |
| `active` | bool | No | false = spawned but disabled until a trigger activates it |

**Spawn type values:**

```
player_start        ← Roan's starting position
droid_t1            ← T1 Security Droid
droid_t2            ← T2 Shielded Droid
stormtrooper        ← Imperial Stormtrooper
force_shadow        ← Force Shadow Enemy
sith_acolyte        ← Sith Acolyte (mini-boss)
boss_vaelen         ← Darth Vaelen (Ch. 5 only)
npc_pasha           ← Pasha companion NPC
npc_gaiana          ← Gaiana companion NPC
npc_tazi            ← Tazi
collectible_journal ← Journal Page pickup
collectible_holocron← Holocron Fragment
collectible_saber   ← Lightsaber Part
collectible_hp_large← HP Crystal (large)
collectible_hp_small← HP Crystal (small)
collectible_fp      ← Force Orb
force_echo          ← Force Echo imprint
meditation_point    ← Meditation Point
physics_crate       ← Moveable crate (Force-pushable/liftable)
platform_moving     ← Moving platform (needs path)
```

---

## Navigation Layer — Patrol Paths

Objects in the `navigation` layer are **polyline** objects. Each polyline is a connected sequence of waypoints an enemy follows in order (then reverses or loops).

| Property | Type | Required | Description |
|---|---|---|---|
| `id` | string | Yes | Matches `patrol_path` on a spawn object |
| `loop` | bool | No | true = loops back to start; false = ping-pongs |

---

## Parallax Background Layers

Layers with names prefixed `parallax_` are rendered at a reduced scroll speed relative to the camera.

| Layer Name | Scroll Factor | Example Content |
|---|---|---|
| `parallax_far` | 0.2× | Distant moon surface, stars |
| `parallax_mid` | 0.5× | Mid-distance ruins, cliff faces |
| `parallax_near` | 0.8× | Near foreground props |
| `foreground` | 1.0× | Solid terrain (normal) |
| `overlay` | 1.0× | Foreground decoration (normal) |

The `TileMap::render()` function reads the `parallax_*` prefix and applies the corresponding scroll factor when calculating the draw position.

---

## TMX Parsing — Implementation Notes

The `TileMap` class parses `.tmx` XML using **tinyxml2** (vendored in `third_party/tinyxml2.h`).

**Parse order:**
1. Read `<tileset>` elements → load tileset PNGs via `AssetManager`, build `tileId → SDL_Rect` lookup
2. Read `<layer>` elements of type `tilelayer` → populate tile grids per layer
3. Read `<objectgroup>` elements → populate trigger list, spawn list, navigation paths
4. Build collision grid from `foreground` tile layer (boolean 2D array)
5. Build parallax layer list (ordered by `parallax_*` prefix)

**Coordinate system:**
- Tiled uses top-left origin, Y increases downward — matches SDL2's coordinate system directly. No conversion needed.

---

## Asset Naming Convention

```
assets/tilemaps/
  ch{N}_{name}.tmx          ← level file    (e.g. ch1_arrival.tmx)
  ch{N}_{name}_bg.png       ← bg tileset    (e.g. ch1_arrival_bg.png)
  ch{N}_{name}_fg.png       ← fg tileset    (e.g. ch1_arrival_fg.png)

assets/sprites/
  roan.png                  ← Roan sprite sheet
  droid_t1.png              ← T1 droid sprite sheet
  droid_t2.png              ← T2 droid sprite sheet
  stormtrooper.png
  force_shadow.png
  sith_acolyte.png
  vaelen.png
  npc_pasha.png
  npc_gaiana.png
  npc_tazi.png
  projectile_blaster.png
  projectile_grenade.png
  fx_explosion.png
  fx_force_push.png
  fx_force_glow.png
  ui_hud.png                ← HUD spritesheet (bars, icons)
  ui_force_wheel.png        ← Force power wheel icons
  ui_meditation.png         ← Meditation point sprite

assets/audio/
  music_prologue.ogg
  music_ch1_exterior.ogg
  music_ch2_outpost.ogg
  music_ch3_underground.ogg
  music_ch4_sith.ogg
  music_ch5_chamber.ogg
  music_boss_vaelen.ogg
  sfx_saber_swing_light.wav
  sfx_saber_swing_heavy.wav
  sfx_saber_hum.wav
  sfx_saber_clash.wav
  sfx_saber_parry.wav
  sfx_force_push.wav
  sfx_force_pull.wav
  sfx_force_lift.wav
  sfx_force_mindtrick.wav
  sfx_force_slowtime.wav
  sfx_force_sight.wav
  sfx_blaster_fire.wav
  sfx_blaster_deflect.wav
  sfx_droid_alert.wav
  sfx_droid_death.wav
  sfx_footstep_stone.wav
  sfx_footstep_metal.wav
  sfx_jump.wav
  sfx_land.wav
  sfx_roll.wav
  sfx_hit_roan.wav
  sfx_meditation.wav
  sfx_collectible_pickup.wav
  sfx_door_open.wav
  sfx_terminal_access.wav

assets/fonts/
  main_font.ttf             ← UI font
  dialog_font.ttf           ← Dialog box font
```
