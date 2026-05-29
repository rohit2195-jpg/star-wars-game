# Force Powers — Detailed Specifications

## Architecture Overview

All Force powers implement the `ForcePower` base class:

```cpp
class ForcePower {
public:
    float energyCost;       // FP deducted on activation
    float cooldownSeconds;  // Seconds before power can be used again
    bool  isUnlocked;       // Only usable when true

    virtual void activate(Entity* caster, World* world) = 0;
    bool canUse(ForceUserComponent* fuc);  // checks energy + cooldown
};
```

The `ForceUserComponent` on Roan owns a `map<PowerType, ForcePower*>` of all 6 powers plus the current FP pool and per-power cooldown timers.

---

## Force Push

**Energy cost:** 15 FP  
**Cooldown:** 0.5s  
**Unlock:** Prologue (already known)

### Behavior

1. On activation, fire a **raycast** from Roan's center in the direction he's facing
2. The ray travels up to **6 tiles**
3. Any entity hit with a `PhysicsComponent` receives an **impulse** of 600 units in the push direction
4. Any entity hit with a `HealthComponent` (and no Push immunity) takes **10 damage**
5. The push also affects **physics objects** (crates, platforms, debris)
6. Enemies that collide with a wall after being pushed take an additional **15 collision damage**

### Visual
- Brief glow on Roan's hand
- Distortion wave effect traveling outward (shader or animated sprite overlay)
- Pushed entity has a brief "pushed" animation

### Upgrade 1 — Extended Push
Range increased from 6 to 9 tiles.

### Upgrade 2 — Concussive Push
Stagger duration on hit enemies increased from 15f to 35f. AoE radius: 1.5 tiles around the ray endpoint (small cone effect).

### Combat Uses
- Break enemy guard
- Knock enemies off ledges (death if they fall far enough)
- Interrupt enemy attacks (if pushed before active frames)
- Combo extender: Push a stunned enemy into a wall → wall collision damage → combo follows

### Puzzle Uses
- Move heavy objects blocking paths
- Push floating platforms into position
- Trigger pressure plates from a distance

---

## Force Pull

**Energy cost:** 15 FP  
**Cooldown:** 0.5s  
**Unlock:** Chapter 1 (environmental puzzle forces use)

### Behavior

Same as Force Push but in reverse:
1. Raycast in facing direction up to **6 tiles**
2. Hit entity is **pulled toward Roan** with an impulse of 600 units
3. No direct damage on pull
4. If the pulled entity hits Roan (arrives at his position): 10 damage to the entity (melee impact)
5. Enemies pulled within melee range are **briefly staggered** (20f) on arrival

### Visual
- Same as Push but direction-reversed animation

### Upgrade 1 — Pull Throw
After pulling an entity, if the player holds a direction and releases the Force input, the entity is **thrown** in that direction (600 units impulse). Effective for throwing enemies into walls or throwing crates at distant enemies.

### Upgrade 2 — Double Pull
Can target and pull **two entities simultaneously** (within 2-tile radius of the raycast endpoint).

### Combat Uses
- Pull a ranged enemy into melee range
- Pull a projectile (grenade) back to the thrower
- Disarm enemies (Pull weapon out of hand — specific enemy types only)
- Pull a shielded enemy off balance (breaks guard briefly)

### Puzzle Uses
- Pull levers or switches out of reach
- Pull platforms toward Roan to create a path
- Draw objects off pressure plates

---

## Force Lift

**Energy cost:** 20 FP (initial); 5 FP/second while holding  
**Cooldown:** 1.0s  
**Unlock:** Chapter 2 (door blocked by debris)

### Behavior

1. On activation, **all physics entities** within a **3-tile radius** of Roan's aim point levitate
2. Entities rise to **4 tiles above their original height** over 0.5s
3. Player can **hold** the power to keep them suspended (costs 5 FP/sec)
4. On release: entities fall, taking **fall damage** if dropped from height
5. Maximum hold time without Upgrade 1: **30f (~0.5s)**. With Upgrade 1: **60f (~1s)**

### Targeting
- Aim point is determined by the look direction (right stick or mouse position)
- A circle indicator shows the 3-tile lift radius on the ground

### Visual
- Blue aura around lifted entities
- Roan's arm raised, straining animation for long holds

### Upgrade 1 — Sustained Lift
Hold duration extended from 30f to 60f. Significantly easier to attack lifted enemies.

### Upgrade 2 — Lift Slam
On release, instead of dropping, press the power key again to **slam entities downward**. Slam deals 40 AoE damage to all entities within 2 tiles of the impact point.

### Combat Uses
- Lift enemies to interrupt their attacks
- Lift group of enemies, then slam for AoE damage
- Lift enemy while ally (Pasha) attacks — scripted moments
- Lift shielded enemies to expose underside

### Puzzle Uses
- Lift heavy debris to clear paths
- Lift and reposition crates onto pressure plates
- Hold a platform up while running across it

---

## Mind Trick

**Energy cost:** 25 FP  
**Cooldown:** 3.0s  
**Unlock:** Chapter 3 (Imperial guard blocks a door, only solution)

### Behavior

1. On activation, enter a **targeting mode** (brief pause in time — not full Slow Time)
2. A crosshair appears; move it onto a target NPC within **5 tiles**
3. Confirm → the target NPC's `AIComponent` enters **OVERRIDDEN** state for **10 seconds**
4. While overridden: NPC follows a specific behavior directive

**Behavior directives (context-dependent):**
| Context | Override behavior |
|---|---|
| Door guard | Moves to "open door" action, walks away |
| Patrol enemy | Walks to a designated distraction point |
| Attacking enemy | "Turns off" temporarily — walks in random direction, ignores Roan |
| Terminal operator | Activates terminal / switch |

After the duration expires, the NPC returns to its normal state (slightly confused — brief `SUSPICIOUS` state before reverting to UNAWARE).

### Limits
- Cannot be used on bosses
- Cannot be used on droids (no organic mind to trick)
- Can be used on: Stormtroopers, other humans, humanoid NPCs

### Visual
- Ripple effect on target when trick lands
- Faint glow in the target's eyes

### Upgrade 1 — Extended Trick
Duration extended from 10s to 20s.

### Upgrade 2 — Group Trick
Can target up to 2 NPCs simultaneously. Second target costs an additional 15 FP.

### Combat Uses
- Remove a dangerous enemy from a fight temporarily
- Turn enemies against each other (if aimed at two: second target interprets first as enemy — untested, optional mechanic)
- Create distraction to break alert state

### Puzzle Uses
- Make guards open locked doors
- Make engineers activate mechanisms
- Distract patrol at the moment Roan needs to pass unseen

---

## Slow Time

**Energy cost:** 10 FP activation; 8 FP/second while active  
**Cooldown:** 4.0s (starts after effect ends)  
**Unlock:** Chapter 3 (falling debris section)

### Behavior

1. On activation, global `deltaTime` multiplier drops to **0.3× (30% speed)**
2. Roan himself is **not slowed** — he moves at full speed
3. All other entities (enemies, projectiles, physics objects, traps) move at 30% speed
4. Duration limited by FP: at 8 FP/sec and 100 base FP (saving 15 for a Follow-up), ~10 seconds max
5. A **visual filter** is applied: slight blue-grey desaturation, subtle blur on fast objects

### Practical effect
- Projectiles can be dodged easily
- Enemy attacks become readable (even fast ones)
- Trap timings become manageable
- Platforming challenges with moving parts become doable

### Limits
- Does **not** affect Darth Vaelen's Slow Time in Phase 2 (his counter-slow cancels Roan's)
- Roan's Force-based attacks during Slow Time do NOT deal extra damage (it's a time-management tool, not a damage multiplier)

### Visual
- Screen border gains a soft blue glow
- Particle effects move in slow motion
- Music pitch drops slightly

### Upgrade 1 — Extended Slow
Duration: FP drain reduced to 5 FP/sec (effectively ~20s at base FP). Duration is still FP-limited.

### Upgrade 2 — Combat Slow
When Slow Time is active during a combo, the combo window (time between inputs) is extended by +15f. Makes perfect parries easier during Slow Time.

### Combat Uses
- Dodge fast multi-hit attacks
- Extend parry window for difficult boss attacks
- Create space to heal (via meditation) — actually cannot; meditation requires no combat state
- Navigate around AoE attacks

### Puzzle Uses
- Pass through rapidly oscillating traps
- Platform on moving surfaces that move too fast normally
- Catch and redirect fast-moving objects

---

## Force Sight

**Energy cost:** 0 (activation is free); 5 FP/second while active  
**Cooldown:** None  
**Unlock:** Chapter 4 (barrier room puzzle)

### Behavior

1. On activation, renders switch to an alternate **visual mode**
2. Normal world fades to grey/low-contrast
3. The following are highlighted:
   - **Hidden paths or doors** (glow blue)
   - **Force Shadow weak points** (glow orange)
   - **Enemy weak points** on bosses (glow red)
   - **Force Echo locations** (glow white)
   - **Vaelen's true position** in Phase 2 clone attack (glow red; clones glow grey)
4. Can be maintained indefinitely while FP lasts

### Limits
- Cannot attack while Force Sight is active (both hands conceptually occupied)
- Toggle off to resume combat
- Enemies are still dangerous in Sight mode — being hit interrupts it

### Visual
- Black-and-white filter with colored highlights
- Similar aesthetic to Detective Mode (Batman: Arkham) but Force-themed (energy visible as blue lines)

### Upgrade 1 — Sight Range
Detection radius for hidden objects increased by 50%. Enemies can be sensed through walls (their position shown as a silhouette).

### Upgrade 2 — Sight Mark
After spotting an enemy with Force Sight and attacking them, they remain "marked" for 10 seconds. Marked enemies take +25% damage.

### Combat Uses
- Identify which Force Shadow enemy is vulnerable vs immune
- Spot hidden boss weak points
- Distinguish Vaelen's real position from clones
- Marked enemy buff for focused damage

### Puzzle Uses
- Reveal hidden paths and false walls
- Locate hidden Force Echoes and collectibles
- Identify pressure plates hidden under debris

---

## FP Regeneration

FP regenerates at **5 FP/second** while:
- Not actively using a Force power
- Not in a combo (attacking interrupts regen briefly — 0.5s pause on hit)
- Not taking damage (1.5s pause after being hit)

**Full regen from 0:** ~20 seconds at base 100 FP. With full upgrades (175 FP): ~35 seconds.

This makes FP a **short-term resource** managed in bursts, not a permanent currency. Players should feel comfortable using powers frequently, not hoarding.

---

## Force Energy Bar Visual States

| FP Level | Bar Color | Player Signal |
|---|---|---|
| 100–60% | Full blue | Plentiful |
| 60–30% | Mid blue | Use carefully |
| 30–10% | Pale blue | Almost empty |
| 10–0% | Flickering pale | Nearly depleted |
| 0% | Grey | No powers available |

When a power is used and FP drops, the portion used briefly flashes white before draining visually. This makes the cost legible frame-by-frame.
