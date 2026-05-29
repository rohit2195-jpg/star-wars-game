# Controls & Movement Systems

## Input Mapping

### Keyboard (Primary)

| Action | Key | Notes |
|---|---|---|
| Move Left | A or ← | |
| Move Right | D or → | |
| Jump | Space | Hold for higher jump (variable jump height) |
| Roll / Dash | Left Shift + direction | Direction from current movement input |
| Crouch / Stealth | Left Ctrl | Hold to maintain crouch |
| Light Attack | Z | |
| Heavy Attack | X | |
| Kick | Down + Z | While standing still or crouching |
| Block / Parry | A (held = passive block; timed = parry) | |
| Force Power (use) | Q | Uses currently selected power |
| Force Power Wheel | E (hold) | Radial menu; release on desired power |
| Interact | F | Context-sensitive: talk / use terminal / open door |
| Jump (wall) | Space while touching wall | |
| Double Jump | Space mid-air (unlocked Ch.3) | |
| Force Sight | Tab (toggle) | |
| Pause / Map | Escape | |
| Debug Colliders | F1 | Dev only — toggle AABB wireframe overlay |

### Gamepad (Optional / Later Milestone)

| Action | Button |
|---|---|
| Move | Left Stick |
| Jump | A (Xbox) / X (PS) |
| Roll | B / Circle + direction |
| Light Attack | X / Square |
| Heavy Attack | Y / Triangle |
| Block / Parry | LB / L1 |
| Force Power | RT / R2 |
| Force Wheel | LT hold / L2 hold |
| Interact | A / X |
| Force Sight | RS click / R3 |
| Pause | Start / Options |

---

## Movement System

### Ground Movement

| State | Speed (tiles/sec) | Transition |
|---|---|---|
| Walk | 4 | Default when moving |
| Run | 8 | Automatic after 0.3s of walking (no button required) |
| Crouch walk | 2 | Hold Ctrl while moving |
| Idle | 0 | No input |

**Acceleration:** Roan does not instantly reach top speed. He accelerates over 6f to walk speed and 12f to run speed. Deceleration is 8f. This gives weight to movement without feeling sluggish.

**Turning:** Immediate (no momentum penalty for direction changes).

### Jumping

**Variable jump height:** Hold Space to jump higher. Jump height is determined by how long the button is held, up to a maximum.

| Jump Type | Height | Notes |
|---|---|---|
| Tap jump | 2.5 tiles | Minimum height |
| Full jump (held) | 5 tiles | Maximum height |
| Wall jump | 4 tiles | Deflects off wall at ~45° angle |
| Double jump | +3 tiles from apex | Unlocked Ch. 3; can be used at any point mid-air |

**Coyote time:** 6f window after walking off a ledge where a jump is still accepted. Prevents frustrating missed jumps at ledge edges.

**Jump buffer:** If Space is pressed 8f before landing, the jump executes on the first frame of landing. Prevents missed inputs during fast falls.

### Roll / Dash

- **Duration:** 12f
- **Distance:** ~2.5 tiles
- **Invincibility frames:** 3–10 (8 i-frames in the middle of the roll)
- **Cooldown:** 20f after roll ends
- **Can roll in any direction** (including backward and diagonally)
- **Cannot roll** during an attack wind-up or if in a combo recovery frame

### Crouch

- Reduces Roan's hitbox height by ~50%
- Required for stealth approach (enemies have reduced detection range against crouching Roan)
- Can attack from crouch (light attack becomes a low sweep — same damage, different animation)
- **Slide:** Running + Ctrl = slide. Covers 4 tiles in 14f with a low hitbox throughout.

### Wall Jump

- Available from the start
- Roan must be pressing toward the wall while touching it
- He "sticks" for 6f (allowing the player to press jump)
- Wall jump velocity: same height as a full held jump, direction is away from wall
- **Chains:** Can wall jump between two parallel walls to climb

### Double Jump

Unlocked via story in Chapter 3.

- Available once per airborne period (resets on landing)
- Can be used at any point during a jump or fall
- Slightly different animation from ground jump (a Force-assisted burst)

---

## Physics Constants

These values define the feel of movement. They can be tuned during development.

| Constant | Value | Notes |
|---|---|---|
| Gravity | 28 tiles/s² | Standard gravity pull |
| Max fall speed | 20 tiles/s | Terminal velocity |
| Jump velocity (tap) | 10 tiles/s upward | Minimum jump |
| Jump velocity (full) | 16 tiles/s upward | Maximum jump |
| Run speed | 8 tiles/s | Horizontal |
| Walk speed | 4 tiles/s | Horizontal |
| Ground friction | High (stop in ~8f) | Crisp stops |
| Air control | 70% of ground speed | Can steer mid-air but not as sharply |

---

## Context Actions (F key)

The Interact key is context-sensitive. Roan can interact with:

| Object | Action | Notes |
|---|---|---|
| NPC | Talk | Opens dialog |
| Terminal | Hack / Read | Some require Tazi to open |
| Door switch | Activate | Some require Force Pull/Mind Trick |
| Meditation Point | Rest | Saves, restores, respawns enemies |
| Downed enemy | Loot | Optional; yields nothing mechanical currently |
| Collectible | Pick up | Journal, lightsaber part, holocron fragment |

A small prompt appears above interactive objects when Roan is within range (1.5 tiles).

---

## HUD Layout

```
┌──────────────────────────────────────────────┐
│  [HP ████████████░░░░]    [FP ██████░░░░░░]  │
│                                              │
│                                              │
│                  (game world)                │
│                                              │
│                                              │
│                              [Q: ForcePush]  │
│                              [Cooldown: ░░░] │
└──────────────────────────────────────────────┘
```

- **HP bar:** Top-left. Green → yellow (below 50%) → red (below 25%)
- **FP bar:** Top-right. Blue. Drains on Force power use; regenerates at 5 FP/s while not using powers
- **Active power indicator:** Bottom-right. Shows current power icon + cooldown ring animation
- **Interact prompt:** Appears above objects in world space, not fixed HUD

### Force Power Wheel

Hold E to open. Radial menu centered on screen. Release on desired power.

```
         [Force Lift]
              ↑
[Mind Trick] ←   → [Force Push]
              ↓
         [Force Pull]

     [Slow Time] (lower-left)
     [Force Sight] (lower-right)
```

Locked powers are grayed out. Selected power highlighted in blue.

---

## Stealth System

Not a core mechanic — a contextual option when crouching near enemies.

**Detection mechanics:**
- Each enemy has a **sight cone** (visual) and a **detection radius** (audio)
- Crouching reduces the audio detection radius by 60%
- Crouching outside the sight cone = undetected
- Running inside detection radius = instant alert
- Standing still in shadows (low-light tiles) grants a slight stealth bonus (not required, just a nice detail)

**Detection states:**
1. `UNAWARE` — patrolling normally
2. `SUSPICIOUS` — heard something; moves toward last position; question mark indicator
3. `ALERTED` — spotted Roan; combat state; exclamation mark indicator

**Alert reset:** If Roan breaks line of sight and all enemies lose track of him for 6 seconds, detection resets to `SUSPICIOUS` then `UNAWARE`.

**This system is intentionally simple.** Stealth is a bonus option, not a required mechanic (except Mind Trick which synergizes with it).
