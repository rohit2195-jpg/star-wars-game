# Animation States

## Architecture

The `AnimatorComponent` owns a map of named `AnimationClip` objects and a pointer to the current active clip. Each clip references a row of frames in a sprite sheet.

```cpp
struct AnimationClip {
    std::string  name;
    int          sheetRow;      // row index in sprite sheet
    int          frameCount;    // number of frames in this clip
    int          frameWidth;    // pixel width of each frame
    int          frameHeight;   // pixel height of each frame
    float        fps;           // playback speed
    bool         loop;          // true = loops; false = holds last frame
    bool         flipX;         // mirror for left-facing direction
};
```

State transitions are triggered by the entity that owns the animator (e.g., `Player::update()` calls `animator->play("run")` when movement is detected).

**Sprite sheet convention:**
- Each character has one sprite sheet PNG
- Rows = animation clips (in the order listed below)
- Columns = frames within that clip
- All frames are the same pixel size within one sheet

---

## Animation Philosophy

**Target quality: Hollow Knight / Dead Cells fluidity.** Every action needs three components:
1. **Anticipation** — the wind-up (crouch before jump, draw-back before swing)
2. **Strong keyframe** — the peak of the action (fully extended saber, apex of jump)
3. **Follow-through** — the settle (saber arc completing, cape catching up, landing squash)

This is what separates fluid indie-quality animation from stiff retro pixel art. Never cut these — especially on attacks. When in doubt, add a frame rather than remove one.

**Squash and stretch:** Apply subtly on jump landings (squash) and jump launches (stretch). Roan is a Jedi — his movement should feel slightly superhuman without breaking the pixel art aesthetic.

---

## Roan Novachez — Animation States

**Sprite sheet dimensions:** 48×48 px per frame (character + full lightsaber swing clearance)

> Frame counts reflect the target for a fluid feel. Placeholder art can use fewer frames initially — bump to target counts when final sprites are drawn.

| Row | State Name | Frames | FPS | Loop | Trigger |
|---|---|---|---|---|---|
| 0 | `idle` | 8 | 8 | yes | No input, grounded — subtle breathing + cape drift |
| 1 | `walk` | 10 | 12 | yes | Moving, grounded, < run speed |
| 2 | `run` | 10 | 16 | yes | Moving, grounded, ≥ run speed — lean forward, cape trailing |
| 3 | `jump_start` | 5 | 20 | no | Jump input — crouch anticipation + launch stretch |
| 4 | `jump_rise` | 3 | 8 | yes | Ascending (velocity Y < 0) |
| 5 | `jump_apex` | 3 | 6 | yes | Near apex (|velocity Y| < 2) — body tucks briefly |
| 6 | `jump_fall` | 3 | 8 | yes | Descending (velocity Y > 0) |
| 7 | `jump_land` | 5 | 22 | no | Landing squash + recovery — transitions to idle/run |
| 8 | `roll` | 8 | 30 | no | Roll input — full tuck and extend |
| 9 | `crouch_idle` | 4 | 6 | yes | Crouching, no input |
| 10 | `crouch_walk` | 8 | 10 | yes | Crouching + moving |
| 11 | `slide` | 6 | 22 | no | Slide input (run + crouch) |
| 12 | `wall_stick` | 3 | 6 | yes | Touching wall, airborne — slight grip animation |
| 13 | `wall_jump` | 5 | 20 | no | Jump from wall — push-off anticipation |
| 14 | `double_jump` | 6 | 22 | no | Double jump — Force burst effect + body stretch |
| 15 | `attack_light_1` | 10 | 24 | no | Light attack 1 — draw-back → slash → follow-through |
| 16 | `attack_light_2` | 10 | 24 | no | Light attack 2 — opposite arc |
| 17 | `attack_light_3` | 12 | 24 | no | Light attack 3 — forward thrust with weight |
| 18 | `attack_light_4` | 14 | 20 | no | Spin — 360° arc, cape follows |
| 19 | `attack_heavy` | 14 | 16 | no | Heavy — long wind-up, big swing, long follow-through |
| 20 | `attack_kick` | 6 | 22 | no | Kick — snap out, snap back |
| 21 | `attack_heavy_finisher` | 12 | 18 | no | Heavy finisher (Z Z X) — launching overhead |
| 22 | `air_attack` | 10 | 22 | no | Air light — downward slash arc |
| 23 | `parry` | 5 | 30 | no | Parry — snap block + recoil |
| 24 | `block` | 3 | 6 | yes | Holding block — guarded stance |
| 25 | `hit_light` | 3 | 20 | no | Receiving light damage |
| 26 | `hit_heavy` | 5 | 16 | no | Receiving heavy damage (knockback) |
| 27 | `stagger` | 6 | 12 | no | Staggered by Force Push or parry |
| 28 | `death` | 8 | 12 | no | HP reaches 0 |
| 29 | `force_push` | 4 | 20 | no | Force Push activation |
| 30 | `force_pull` | 4 | 20 | no | Force Pull activation |
| 31 | `force_lift` | 4 | 16 | yes | Force Lift hold (loops while holding) |
| 32 | `force_lift_end` | 3 | 20 | no | Force Lift release |
| 33 | `force_mindtrick` | 5 | 14 | no | Mind Trick cast |
| 34 | `force_slowtime` | 4 | 16 | no | Slow Time activation |
| 35 | `force_sight_on` | 3 | 18 | no | Force Sight toggle on |
| 36 | `force_sight_idle` | 2 | 6 | yes | Force Sight active loop |
| 37 | `force_sight_off` | 3 | 18 | no | Force Sight toggle off |
| 38 | `interact` | 4 | 12 | no | Using terminal / picking up item |
| 39 | `meditate` | 6 | 8 | no | Sitting at Meditation Point |

**State machine priority (highest wins):**
1. `death` (uninterruptible once started)
2. `hit_heavy` / `stagger`
3. Any `attack_*` or `force_*` (play to completion unless interrupted by damage)
4. `roll`, `slide`
5. `parry`
6. `jump_*` states (air movement)
7. `walk` / `run` / `crouch_*`
8. `idle` / `crouch_idle`

---

## T1 Security Droid — Animation States

**Sprite sheet dimensions:** 32×32 px per frame

| Row | State Name | Frames | FPS | Loop | Trigger |
|---|---|---|---|---|---|
| 0 | `patrol` | 6 | 10 | yes | PATROL state |
| 1 | `alert` | 4 | 16 | no | Entering ALERTED state |
| 2 | `chase` | 6 | 14 | yes | COMBAT_APPROACH |
| 3 | `shoot` | 4 | 20 | no | Blaster attack |
| 4 | `melee_swing` | 5 | 16 | no | Melee attack |
| 5 | `rush_charge` | 6 | 20 | no | Rush charge |
| 6 | `hit` | 3 | 20 | no | Taking damage |
| 7 | `stagger` | 5 | 14 | no | Staggered |
| 8 | `death` | 7 | 14 | no | HP reaches 0 |
| 9 | `idle` | 2 | 4 | yes | Standing guard (IDLE AI state) |

---

## T2 Security Droid (Shielded) — Animation States

**Sprite sheet dimensions:** 36×36 px per frame (slightly larger)

Same rows as T1, with two additions:

| Row | State Name | Frames | FPS | Loop | Trigger |
|---|---|---|---|---|---|
| ... | (all T1 states) | — | — | — | — |
| 10 | `shield_up` | 3 | 16 | no | Shield activates |
| 11 | `shield_broken` | 4 | 20 | no | Shield breaks (Force Push or kick) |
| 12 | `shield_idle` | 2 | 4 | yes | Walking with shield raised |

---

## Imperial Stormtrooper — Animation States

**Sprite sheet dimensions:** 32×40 px per frame (taller humanoid)

| Row | State Name | Frames | FPS | Loop | Trigger |
|---|---|---|---|---|---|
| 0 | `patrol` | 8 | 12 | yes | PATROL state |
| 1 | `alert` | 4 | 18 | no | ALERTED |
| 2 | `run` | 8 | 16 | yes | COMBAT_APPROACH |
| 3 | `shoot` | 4 | 20 | no | Blaster shot |
| 4 | `grenade_throw` | 6 | 14 | no | Grenade throw |
| 5 | `melee_stock` | 4 | 20 | no | Rifle stock melee |
| 6 | `hit` | 3 | 20 | no | Taking damage |
| 7 | `death` | 6 | 14 | no | HP reaches 0 |
| 8 | `crouch_shoot` | 3 | 14 | yes | Taking cover + shooting |

---

## Force Shadow Enemy — Animation States

**Sprite sheet dimensions:** 32×40 px per frame

| Row | State Name | Frames | FPS | Loop | Trigger |
|---|---|---|---|---|---|
| 0 | `float_idle` | 4 | 8 | yes | PATROL/IDLE (hovers in place) |
| 1 | `approach` | 6 | 12 | yes | COMBAT_APPROACH |
| 2 | `shadow_slash` | 5 | 22 | no | Shadow slash attack |
| 3 | `teleport_out` | 3 | 24 | no | Teleporting away |
| 4 | `teleport_in` | 3 | 24 | no | Appearing at new position |
| 5 | `phase_burst_charge` | 5 | 10 | no | Charging burst (loops until release) |
| 6 | `phase_burst_release` | 4 | 20 | no | Burst release |
| 7 | `vulnerable` | 2 | 6 | yes | After Force Push hit — glowing |
| 8 | `hit` | 3 | 20 | no | Taking damage |
| 9 | `death` | 8 | 14 | no | HP reaches 0 |

---

## Sith Acolyte (Mini-Boss) — Animation States

**Sprite sheet dimensions:** 40×48 px per frame

| Row | State Name | Frames | FPS | Loop | Trigger |
|---|---|---|---|---|---|
| 0 | `idle` | 4 | 8 | yes | COMBAT (waiting to attack) |
| 1 | `walk` | 6 | 12 | yes | COMBAT_APPROACH |
| 2 | `light_combo_1` | 4 | 22 | no | Combo hit 1 |
| 3 | `light_combo_2` | 4 | 22 | no | Combo hit 2 |
| 4 | `light_combo_3` | 5 | 22 | no | Combo hit 3 |
| 5 | `heavy_overhead` | 7 | 14 | no | Heavy overhead |
| 6 | `saber_throw` | 6 | 16 | no | Saber throw |
| 7 | `saber_throw_return` | 4 | 16 | no | Saber returning to hand |
| 8 | `force_push_cast` | 4 | 18 | no | Their Force Push (P2) |
| 9 | `debris_throw` | 6 | 14 | no | Debris barrage (P2) |
| 10 | `hit` | 3 | 20 | no | Taking damage |
| 11 | `stagger_heavy` | 6 | 12 | no | Heavy stagger (parried) |
| 12 | `saber_lost` | 3 | 18 | no | Saber thrown back at them |
| 13 | `phase2_transition` | 5 | 10 | no | Entering Phase 2 |
| 14 | `death` | 9 | 12 | no | Defeated |

---

## Darth Vaelen (Boss) — Animation States

**Sprite sheet dimensions:** 48×56 px per frame

| Row | State Name | Frames | FPS | Loop | Trigger |
|---|---|---|---|---|---|
| 0 | `idle_menacing` | 4 | 6 | yes | Between attacks |
| 1 | `walk` | 6 | 10 | yes | Approaching |
| 2 | `light_combo_1` | 4 | 24 | no | Fast light 1 |
| 3 | `light_combo_2` | 4 | 24 | no | Fast light 2 |
| 4 | `light_combo_3` | 4 | 24 | no | Fast light 3 |
| 5 | `heavy_overhead` | 8 | 16 | no | Overhead slam |
| 6 | `force_push_cast` | 5 | 18 | no | His Force Push |
| 7 | `saber_throw` | 6 | 16 | no | Saber throw |
| 8 | `clone_split` | 5 | 14 | no | Splitting into clones (P2) |
| 9 | `clone_idle` | 2 | 6 | yes | Shadow clone standing |
| 10 | `slow_time_cast` | 6 | 10 | no | His Slow Time (P2) |
| 11 | `force_drain_charge` | 4 | 8 | yes | Charging drain (P2) |
| 12 | `force_drain_release` | 4 | 18 | no | Drain release |
| 13 | `flurry_7hit` | 12 | 28 | no | 7-hit flurry (P3) |
| 14 | `chamber_slam_charge` | 7 | 10 | no | Charging slam (P3) |
| 15 | `chamber_slam_release` | 5 | 20 | no | AoE slam |
| 16 | `dark_wave` | 6 | 16 | no | Dark Force Wave (P3) |
| 17 | `saber_lock` | 4 | 10 | yes | Final saber lock loop |
| 18 | `hit` | 3 | 18 | no | Taking damage |
| 19 | `phase_transition` | 8 | 10 | no | Phase 1→2, 2→3 transitions |
| 20 | `defeated` | 12 | 10 | no | Final defeat animation |

---

## Projectile Animations

Blaster bolts, thrown grenades, Force-lifted objects use separate small sprite sheets.

| Object | Dimensions | Frames | FPS | Notes |
|---|---|---|---|---|
| Blaster bolt | 16×6 px | 2 | 12 | Horizontal; rotated in code for angle |
| Grenade | 8×8 px | 1 | — | Static; fuse flicker is a color swap |
| Grenade explosion | 32×32 px | 8 | 20 | One-shot, no loop |
| Force Push wave | 64×16 px | 6 | 24 | Distortion wave traveling right |
| Force glow (general) | 16×16 px | 4 | 10 | Looping; tinted per power color |

---

## Transition Rules Summary

These apply globally in `AnimatorComponent::update()`:

| Condition | Transition Rule |
|---|---|
| `hit_heavy` or `stagger` playing | Interrupt anything except `death` |
| `death` playing | Uninterruptible; entity destroyed at final frame |
| Attack clip ends (no-loop) | Return to `idle` (or `air_attack` → `jump_fall`) |
| `jump_land` ends | Check velocity: → `idle` if stopped, → `run` if moving |
| Force power clip ends | Return to last movement state |
| `roll` ends | Return to `idle` |
| `parry` ends | Return to `idle` |
| `meditate` ends (all frames done) | Return to `idle` |
