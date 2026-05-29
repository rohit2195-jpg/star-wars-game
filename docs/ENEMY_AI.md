# Enemy AI Design

## Architecture

Each enemy has an `AIComponent` that owns a **state machine**. The state machine drives what the enemy does each frame. States are simple C++ enums; transitions are triggered by conditions evaluated in `AIComponent::update()`.

```cpp
enum class AIState {
    PATROL,
    IDLE,
    SUSPICIOUS,
    ALERTED,
    COMBAT_APPROACH,
    COMBAT_ATTACK,
    COMBAT_RECOVER,
    STAGGERED,
    FLEEING,
    OVERRIDDEN,   // Mind Trick active
    DEAD
};
```

The `AIComponent` is attached to enemy entities alongside `PhysicsComponent`, `ColliderComponent`, `HealthComponent`, and a `CombatComponent`.

---

## Shared Detection System

All enemies share the same detection logic (parameterized per enemy type):

| Parameter | Description |
|---|---|
| `sightRange` | Distance in tiles before enemy can spot Roan |
| `sightAngle` | Field-of-view cone (degrees). 180° = sees in front hemisphere |
| `hearingRadius` | Distance at which running footsteps are detected |
| `crouchHearingMod` | Multiplier on hearingRadius when Roan is crouching (e.g. 0.4) |

**Detection check (each frame):**
1. If Roan is within `sightRange` AND within `sightAngle` cone → trigger `ALERTED`
2. If Roan is within `hearingRadius * crouchHearingMod` while crouching → trigger `SUSPICIOUS`
3. If Roan is within `hearingRadius` while running → trigger `ALERTED`

**Alert propagation:** When one enemy enters `ALERTED` state, all enemies within a **3-tile radius** also enter `ALERTED`. This prevents "fight one at a time in a group" cheese.

---

## State Definitions

### PATROL

Enemy walks along a pre-defined waypoint path (set in Tiled level editor as an object path).

- Walks at 60% of maximum speed
- At each waypoint: pauses for `waitDuration` frames (configurable, typically 60f = 1s)
- Continuously runs detection checks

**Transition:** → `SUSPICIOUS` if hears Roan, → `ALERTED` if sees Roan

---

### IDLE

Enemy stands at a fixed point. Used for guards at doors or specific posts.

- Does not move
- Runs detection checks

**Transition:** Same as PATROL

---

### SUSPICIOUS

Enemy heard something. Moves toward last known position of the sound.

- Speed: 80% of maximum
- On arrival at last sound position: looks around (plays "look" animation for 90f)
- If Roan is found: → `ALERTED`
- If nothing found: → return to `PATROL`/`IDLE` after 5 seconds

**Visual cue:** Question mark icon above enemy head

---

### ALERTED

Enemy knows Roan's position. Moves toward him to engage.

- Speed: 100% (run speed)
- If in range for attack: → `COMBAT_ATTACK`
- If out of range: maintain `ALERTED`, keep moving toward Roan

**Visual cue:** Exclamation mark icon above enemy head; red tint added to enemy

**Losing the player:** If Roan breaks line of sight and enemy cannot locate him for 6 seconds:
→ `SUSPICIOUS` (searches for 5s) → `PATROL`

---

### COMBAT_APPROACH

Enemy is closing the gap for a melee attack or lining up a ranged shot.

- For melee enemies: walks within strike range
- For ranged enemies: steps back to optimal shooting distance

**Transition:** → `COMBAT_ATTACK` when in range

---

### COMBAT_ATTACK

Enemy executes an attack.

- Chooses attack from its available `attackSet` based on:
  - Current range to Roan
  - Cooldown status of each attack
  - Health threshold (some attacks only used below 50% HP)
- After attack animation completes: → `COMBAT_RECOVER`

---

### COMBAT_RECOVER

Post-attack recovery period. Enemy is briefly vulnerable.

- Duration: `recoverFrames` (per attack type, see COMBAT_SYSTEMS.md)
- During recovery: enemy cannot attack, but can move slightly
- **Player's punish window** — this is the frame data that matters for combat feel

**Transition:** → `COMBAT_APPROACH` after recovery ends

---

### STAGGERED

Enemy was hit by a parry or a Force Push.

- Duration: `staggerFrames` (varies by enemy type, typically 30f)
- Cannot attack or move during stagger
- Plays stagger animation

**Transition:** → `COMBAT_APPROACH` after stagger ends

---

### FLEEING

Some enemies (weak droids, wounded troopers below 20% HP) will attempt to flee and call for backup.

- Runs away from Roan at 120% speed
- Triggers backup call (spawns reinforcements from a nearby spawn point, if one exists in level)
- If cornered (path blocked): switches to desperate attack (ignores cooldowns)

---

### OVERRIDDEN (Mind Trick)

Enemy is under Mind Trick effect.

- Executes the behavior directive set by the trick (see FORCE_POWERS.md → Mind Trick)
- Cannot attack
- Duration set by Mind Trick parameters

**Transition:** → `SUSPICIOUS` when duration expires (slightly confused)

---

## Enemy-Specific Parameters

### T1 Security Droid

```
sightRange:        8 tiles
sightAngle:        120°
hearingRadius:     4 tiles
crouchHearingMod:  N/A (cannot be Mind Tricked; no audio detection penalty for crouch)
attackSet:         [blasterBurst, meleeSwing, rushCharge]
fleeThreshold:     none (droids don't flee)
speed:             5 tiles/s
```

### T2 Security Droid (Shielded)

```
sightRange:        8 tiles
sightAngle:        120°
hearingRadius:     4 tiles
attackSet:         [blasterBurst, shieldBash]
fleeThreshold:     none
speed:             4 tiles/s (slower, heavier)
```

### Imperial Stormtrooper

```
sightRange:        10 tiles
sightAngle:        90°  (narrower — focused forward)
hearingRadius:     6 tiles
crouchHearingMod:  0.4
attackSet:         [blasterShot, grenade, rifleStock]
fleeThreshold:     20% HP (triggers backup call)
speed:             6 tiles/s
flankerDesignation: true (one trooper per group is assigned flanker role)
```

### Force Shadow Enemy

```
sightRange:        12 tiles
sightAngle:        360° (omnidirectional — sense through Force)
hearingRadius:     N/A (detects Force presence, not sound)
attackSet:         [shadowSlash, teleportStrike, phaseBurst]
fleeThreshold:     none
speed:             7 tiles/s
teleportCooldown:  3.0s
vulnerableWindow:  5s after Force Push/Pull hits them
```

### Sith Acolyte (Mini-Boss)

```
sightRange:        15 tiles
sightAngle:        360°
hearingRadius:     N/A
attackSet(P1):     [lightCombo, heavyOverhead, saberThrow]
attackSet(P2):     [lightCombo, heavyOverhead, saberThrow, forcePush, debrisBarrage]
fleeThreshold:     none
speed:             8 tiles/s
phaseThreshold:    50% HP triggers Phase 2
```

---

## Flanking Behavior (Stormtroopers)

When 2+ stormtroopers are in `ALERTED` state:
1. One is designated **attacker** (engages Roan directly)
2. One is designated **flanker** (moves to a position 90°+ offset from attacker)
3. Flanker waits 3 seconds then engages from the flank

**Flanker position selection:**
- Pick a point 90°–180° from the attacker's current approach vector
- Must be reachable via pathfinding
- If no valid flank position found, flanker acts as a second attacker

**Anti-cheese rule:** If Roan focuses the flanker, the original attacker becomes the new flanker. This prevents the player from simply ignoring the flanker indefinitely.

---

## Pathfinding

Simple **tile-based pathfinding** using A* on the `foreground` collision layer from the Tiled map.

- Grid resolution: 1 tile
- Enemies update their path every 30f (twice per second)
- Jump-aware: if the direct path requires a jump the enemy can perform, it does so
- Not all enemies can wall jump or double jump (droids cannot; humanoids can)

**Pathfinding limits:**
- Enemies do not path through water, hazards, or triggers
- If pathfinding fails (no valid path), enemy enters `IDLE` and waits for Roan to come closer
- Mini-bosses have expanded pathfinding (updated every 10f for responsiveness)

---

## Group Behavior Summary

| Enemy Count | Behavior Pattern |
|---|---|
| 1 enemy | Standard state machine |
| 2 enemies | One attacker, one flanker |
| 3+ enemies | One attacker, one flanker, rest stay at range (ranged support) |
| Mixed types | Droids push forward; troopers hang back and shoot; shadows teleport unpredictably |

---

## Hit Reactions

When an enemy is hit (takes damage):
1. Brief hitstop: 4f pause on both Roan and the enemy (makes hits feel impactful)
2. Knockback: small impulse in the hit direction (2 tiles for light, 4 for heavy)
3. Hit animation plays
4. If HP drops below `staggerThreshold` (50% of max), next hit triggers `STAGGERED` state

Enemies that are hit from behind take **+25% damage** (backstab bonus).
