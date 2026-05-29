# Combat Systems

## Overview

Combat is the primary gameplay loop. It's melee-focused (lightsaber) with Force powers as the tactical layer. Inspired by Jedi: Fallen Order — timing and positioning matter more than button mashing. Players who understand the frame data and enemy behaviors will find combat satisfying; players who don't will be punished.

---

## Frame Timing

The game runs at **60 fps**. All timing below is in frames (1 frame = ~16.7ms).

---

## Player Combat

### Basic Attacks

| Move | Input | Wind-up | Active | Recovery | Damage | Notes |
|---|---|---|---|---|---|---|
| Light Attack | Z | 3f | 3f (frames 4–6) | 10f | 15 HP | Can cancel recovery into another light at frame 10 |
| Heavy Attack | X | 8f | 5f (frames 9–13) | 20f | 35 HP | Cannot cancel; knocks back standard enemies |
| Kick | Down + Z | 4f | 2f | 8f | 5 HP + stagger | Staggers shielded enemies, breaks guard |

### Combo Strings

A combo is triggered when the **next input arrives during a defined combo window** after the previous hit lands. Combo windows are generous (10f) to feel responsive.

| String | Inputs | Effect |
|---|---|---|
| Standard Combo | Z → Z → Z | 3-hit chain, last hit has knockback |
| Heavy Finisher | Z → Z → X | 2 lights into a launching heavy (sends enemy airborne) |
| Spin Attack | Z → Z → Z → Z | 4-hit spin with 360° hitbox on final swing |
| Force Strike | During Force Push + Z | Force-enhanced saber swing; bypasses one block |

Combo state resets if:
- No input for 30f
- Roan is hit (interrupt)
- Roan rolls/dashes

### Parry

**Input:** Block button (A) during the wind-up or active frames of an incoming attack.

| Result | Condition | Effect |
|---|---|---|
| Perfect Parry | Block within 4-frame window during enemy wind-up | Enemy staggers for 30f; Roan takes 0 damage |
| Late Block | Block after active frames begin | Roan takes 50% damage; no stagger |
| Miss | No block | Roan takes full damage |

Parrying a **blaster bolt** reflects it back at the shooter. Timing is the same 4-frame window.

Parrying a **Force attack** (Sith Acolytes, Vaelen) requires Force Push or Force Sight to be active — a saber parry alone fails.

### Roll / Dash

**Input:** Direction + dash button  
**Duration:** 12f  
**Invincibility frames:** 3–10 (8 frames of i-frames in the middle)  
**Cooldown:** 20f  

Roll goes in the held direction, not just forward. Can be used to reposition during combat or escape AoE attacks.

### Blocking

Holding block (without timing a parry) gives a **passive guard**:
- Reduces incoming damage by 60%
- **Does not** stagger the attacker
- **Cannot** block heavy attacks from mini-bosses or bosses
- Reduces Force energy by 5 per blocked hit

---

## Enemy Types

### T1 Security Droid (Basic)

**HP:** 50  
**Behavior:** Patrols a fixed path. Alerts when Roan enters a 6-tile detection radius.

**Attack set:**
| Attack | Windup | Damage | Counter |
|---|---|---|---|
| Blaster burst (2 shots) | 15f | 10 each | Parry to deflect |
| Melee swing | 20f (slow, telegraphed) | 20 | Parry or roll |
| Rush charge | 10f (faster) | 25 | Roll sideways; heavy attack follow-up |

**AI states:** `PATROL → ALERT → ATTACK → RESET`  
Reset occurs if Roan leaves detection range for 5 seconds.

---

### T2 Security Droid (Shielded)

**HP:** 80 (shield absorbs first 40 damage)  
**Behavior:** Same as T1 but holds an energy shield. Shield is broken by a kick or Force Push.

**Additional attack:** Energy shield bash — 12f windup, knocks Roan back 3 tiles.

---

### Imperial Stormtrooper

**HP:** 60  
**Behavior:** Grouped units (2–4 together). One designates as "flanker" if Roan engages another directly.

**Attack set:**
| Attack | Windup | Damage | Counter |
|---|---|---|---|
| Blaster shot | 10f | 12 | Parry to reflect |
| Grenade (thrown) | 30f wind-up + 1.5s fuse | 40 AoE | Force Pull back at them |
| Melee rifle stock | 8f (close range only) | 18 | Roll or parry |

**Flanking behavior:** If a second trooper is present and out of Roan's sight cone, it moves to a flanking position after 3 seconds. Forces player to reposition frequently in grouped fights.

---

### Force Shadow Enemy

**HP:** 70 (standard attacks deal 50% damage; Force-charged attacks deal full damage)  
**Behavior:** Aggressive. Teleports short distances to close range.

**How to fight:**
- Standard attacks deal half damage — recognizable by a "phantom absorption" visual when hit
- Force Push or Force Pull applied to the enemy before striking makes them **vulnerable** for 5 seconds (full damage)
- Force Sight reveals their weak spot (glowing core) — attacking the core deals full damage always
- Slow Time negates their teleport ability while active

**Attack set:**
| Attack | Windup | Damage | Counter |
|---|---|---|---|
| Shadow Slash | 6f (fast) | 20 | Parry |
| Teleport Strike | 0f visible windup — audio cue only | 25 | Listen for audio; dodge on cue |
| Phase Burst | 20f charge | 35 AoE | Roll away; large AoE circle telegraph |

---

### Sith Acolyte (Mini-boss)

**HP:** 200  
**Phase 1 (100%–50% HP):** Standard attacks, single red lightsaber  
**Phase 2 (below 50% HP):** Gains Force telekinesis; adds projectile throws and Force Push

**Attack set:**
| Attack | Phase | Windup | Damage | Counter |
|---|---|---|---|---|
| Light Saber Strike | Both | 5f | 20 | Parry |
| Heavy Overhead | Both | 12f | 40 | Parry (tight window) or roll back |
| Saber Throw | Both | 20f | 30 | Roll |
| Force Push (their own) | P2 | 15f | 0 dmg + knock back | Cannot parry; dodge sideways |
| Debris Barrage (3 rocks) | P2 | 25f | 15 each | Parry each individually or Force Push back |

**Parry reward:** A perfect parry on a saber throw causes them to lose the saber for 5 seconds — attack window.

---

### Patrol Droid Commander (Chapter 1 mini-boss)

**HP:** 150  
**Gimmick:** Energy shield (front only) absorbs all damage. Must use Force Push to spin it around, then attack from behind. Shield reactivates after 8 seconds.

**Phases:**
1. Shield up → walk toward Roan → melee slam (easy to read, Force Push to interrupt)
2. Below 75 HP: calls two T1 droids to assist (clear them first to avoid being flanked)

---

### Corrupted Archive Guardian (Chapter 2 mini-boss)

**HP:** 180  
**Gimmick:** Armored on all sides. Force Lift reveals its undercarriage. Must hold Lift while attacking the exposed area.

**Mechanic:** `Force Lift → Hold → Attack exposed area → Lift releases at 30f hold → Enemy drops → Repeat`  
Player must time attacks within the lift window. Adds complexity as a skill check on Force Lift.

---

### Dark Droid General (Chapter 4 mini-boss)

**HP:** 300  
**Gimmick:** Force Sight required to see weak points (3 glowing panels). Each panel must be destroyed individually.  
**Secondary gimmick:** Force Lift is required to flip open armored plates before each panel is accessible.

Sequence: `Force Sight on → spot panel → Force Lift flips plate → attack panel → repeat ×3`

---

### Boss: Darth Vaelen (Chapter 5)

**HP:** 600 (three phases, each ~200 HP)

#### Phase 1 — "The Test" (600–400 HP)

Vaelen fights conservatively. He is testing Roan's abilities. Attacks are learnable and punishable.

| Attack | Windup | Damage | Counter |
|---|---|---|---|
| Light Combo (3 hits) | 5f each | 15 each | Parry all three |
| Heavy Overhead | 15f | 50 | Roll back; parry window very tight |
| Force Push | 12f | 0 + knockback | Cannot parry; dodge sideways |
| Saber Lock | Clashes with Roan's attack | — | Mash attack button to win; lose = knocked back |

**Phase end:** Vaelen backs off. Brief banter. Phase 2 begins.

#### Phase 2 — "The Pressure" (400–200 HP)

Vaelen fights faster. Adds Slow Time counter (he uses his own version) and shadow clones.

| Attack | Windup | Damage | Counter |
|---|---|---|---|
| All Phase 1 attacks | — | (same) | (same) |
| Shadow Clone Dash | 0f visible | 35 | Force Sight reveals which is real |
| Dark Side Slow Time | — | — | Player's Slow Time canceled; must wait it out |
| Force Drain (ranged) | 30f charge | 0 dmg; drains 40 FP | Interrupt with Force Push |

**Phase end:** Environmental event — part of the chamber collapses. Roan must survive falling debris (Slow Time helps). Vaelen uses the chaos to recover.

#### Phase 3 — "No More Holding Back" (200–0 HP)

Vaelen drops pretense. Much faster, unpredictable. Force Sight required to see his next attack before it happens.

| Attack | Windup | Damage | Counter |
|---|---|---|---|
| All previous attacks | — | (increased) | (same counters) |
| Saber Flurry (7 hits) | 3f each | 10 each | Parry all 7 — perfect timing required |
| Chamber Slam | 40f wind-up | 80 AoE | Roll to a platform edge |
| Dark Force Wave | 20f | 60 | Force Push back at him (parried with Force) |
| Final Saber Lock | Triggered at <50 HP | — | Final QTE mash — Roan wins with full combo |

**Victory:** Roan disarms Vaelen. The choice cutscene begins (see `STORY_OUTLINE.md`).

---

## Combat Feel Targets

These are subjective goals to calibrate against during playtesting:

| Target | Metric |
|---|---|
| Parry timing | Should feel achievable but require attention — not free, not impossible |
| Standard enemy HP | Should die in 3–5 hits on a clean combo |
| Roan death pace | Player should die to careless play; survival should feel earned |
| Force power usage | Player should feel rewarded for using Force powers in combat, not just as puzzle tools |
| Boss phase length | Each phase should take ~2–3 minutes of focused play; total boss fight ~8 min |

---

## Damage Numbers Reference

| Source | Damage |
|---|---|
| Roan light attack | 15 |
| Roan heavy attack | 35 |
| Roan Force Strike | 45 |
| T1 Droid blaster | 10 |
| T1 Droid melee | 20 |
| Stormtrooper blaster | 12 |
| Stormtrooper grenade | 40 AoE |
| Shadow slash | 20 |
| Shadow teleport strike | 25 |
| Acolyte heavy | 40 |
| Vaelen light (P1) | 15 |
| Vaelen heavy (P1) | 50 |
| Vaelen flurry (P3) | 10/hit × 7 |

> All values are starting points. Tuned during playtesting. Roan's base HP is 200.
