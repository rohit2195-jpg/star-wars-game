# Audio Design

## Libraries

| Library | Purpose |
|---|---|
| SDL2_mixer | All audio playback (music + SFX) |
| OGG/Vorbis | Background music format (smaller file size than WAV) |
| WAV | Sound effects (low latency; no decoding overhead) |

**Initialization:**
```cpp
Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
Mix_AllocateChannels(32);  // 32 simultaneous SFX channels
```

---

## Channel Allocation

SDL2_mixer assigns SFX to numbered channels. Reserve specific channels to prevent priority conflicts:

| Channel | Reserved For |
|---|---|
| 0 | Lightsaber hum (continuous loop) |
| 1 | Roan footsteps |
| 2 | UI sounds (menu, HUD) |
| 3 | Force power activation |
| 4–7 | Enemy SFX (4 enemies simultaneously) |
| 8–11 | Projectile sounds |
| 12–15 | Environmental / ambient |
| 16–31 | General-purpose (any overflow) |

Music plays on its own track (not a channel): `Mix_PlayMusic()`.

---

## Music

Music is handled as looping background tracks. One track plays at a time. Transitions use a short fade-out / fade-in (`Mix_FadeOutMusic` + `Mix_FadeInMusic`).

### Track List

| Track | File | Where It Plays | Tempo |
|---|---|---|---|
| Main Theme | `music_main_theme.ogg` | Title screen | Orchestral, heroic |
| Prologue | `music_prologue.ogg` | Prologue chapter | Tense, exploration |
| Chapter 1 | `music_ch1_exterior.ogg` | Ch. 1 moon exterior | Open, slightly eerie |
| Chapter 2 | `music_ch2_outpost.ogg` | Ch. 2 Jedi outpost | Nostalgic, decaying |
| Chapter 3 | `music_ch3_underground.ogg` | Ch. 3 ruins | Dark, ancient |
| Chapter 4 | `music_ch4_sith.ogg` | Ch. 4 catacombs | Oppressive, heavy |
| Chapter 5 | `music_ch5_chamber.ogg` | Ch. 5 descent | Quiet tension |
| Boss Vaelen | `music_boss_vaelen.ogg` | Final boss fight | Full orchestral, urgent |
| Victory | `music_victory.ogg` | Post-boss cutscene | Brief fanfare |
| Menu / Pause | `music_menu.ogg` | Pause / inventory screens | Ambient, low-key |

### Combat Music Layer (Optional Enhancement)

In later milestones: use **layered music** by loading a second track that fades in when combat begins. The base track (exploration) and combat layer blend. Fade the combat layer out when all enemies are defeated.

This is an optional enhancement — single-track approach is fine for initial implementation.

---

## Sound Effects

### Player SFX

| Event | File | Channel | Notes |
|---|---|---|---|
| Lightsaber hum (idle) | `sfx_saber_hum.wav` | 0 | Loops continuously while alive |
| Light attack swing | `sfx_saber_swing_light.wav` | 3 | Play on active frame (frame 4) |
| Heavy attack swing | `sfx_saber_swing_heavy.wav` | 3 | Play on active frame (frame 9) |
| Saber clash (blocked) | `sfx_saber_clash.wav` | 3 | Play on parry |
| Perfect parry | `sfx_saber_parry.wav` | 3 | Distinct sound for perfect parry |
| Footstep (stone) | `sfx_footstep_stone.wav` | 1 | Play every 8f while running on stone |
| Footstep (metal) | `sfx_footstep_metal.wav` | 1 | Metal floor surfaces |
| Jump | `sfx_jump.wav` | 2 | On jump input |
| Land (light) | `sfx_land.wav` | 2 | On landing after short fall |
| Land (heavy) | `sfx_land_heavy.wav` | 2 | Landing after long fall |
| Roll | `sfx_roll.wav` | 2 | On roll start |
| Hit received (light) | `sfx_hit_roan.wav` | 3 | On taking light damage |
| Hit received (heavy) | `sfx_hit_roan_heavy.wav` | 3 | On taking heavy damage |

### Force Power SFX

| Power | Activation Sound | File | Notes |
|---|---|---|---|
| Force Push | Whoosh out | `sfx_force_push.wav` | Directional; pitch varies with distance |
| Force Pull | Whoosh in | `sfx_force_pull.wav` | |
| Force Lift | Rising hum | `sfx_force_lift.wav` | Loop while holding; stop on release |
| Mind Trick | Subtle mental resonance | `sfx_force_mindtrick.wav` | Quiet; heard only nearby |
| Slow Time | Deep reverb drop | `sfx_force_slowtime.wav` | Pitch-shifted feel |
| Force Sight | Soft activation tone | `sfx_force_sight.wav` | Activates + deactivates (same sound) |

### Enemy SFX

| Enemy | Event | File | Channel |
|---|---|---|---|
| T1 Droid | Alert | `sfx_droid_alert.wav` | 4 |
| T1 Droid | Blaster fire | `sfx_blaster_fire.wav` | 8 |
| T1 Droid | Death | `sfx_droid_death.wav` | 4 |
| T1 Droid | Melee swing | `sfx_droid_melee.wav` | 4 |
| Stormtrooper | Alert shout | `sfx_trooper_alert.wav` | 5 |
| Stormtrooper | Blaster fire | `sfx_blaster_fire.wav` | 8 |
| Stormtrooper | Grenade throw | `sfx_grenade_throw.wav` | 9 |
| Stormtrooper | Explosion | `sfx_explosion.wav` | 9 |
| Force Shadow | Teleport | `sfx_shadow_teleport.wav` | 6 |
| Force Shadow | Slash | `sfx_shadow_slash.wav` | 6 |
| Sith Acolyte | Saber ignite | `sfx_saber_hum.wav` | 7 |
| Sith Acolyte | Force Push cast | `sfx_force_push.wav` | 7 |
| Vaelen | Force Drain | `sfx_force_drain.wav` | 7 |

### Blaster Bolt Deflect

When Roan deflects a blaster bolt (parry):
1. `sfx_blaster_deflect.wav` plays (distinct from a regular parry)
2. The bolt entity reverses direction
3. Brief screen flash (visual feedback)

### Collectible SFX

| Event | File | Notes |
|---|---|---|
| Holocron Fragment | `sfx_collectible_holocron.wav` | Distinct "Force chime" |
| Journal Page | `sfx_collectible_journal.wav` | Paper rustle sound |
| Lightsaber Part | `sfx_collectible_saber.wav` | Metallic click |
| HP Crystal | `sfx_collectible_crystal.wav` | Gentle pulse |

### Environmental SFX

| Event | File | Notes |
|---|---|---|
| Door opens | `sfx_door_open.wav` | Stone grinding or metal sliding depending on surface |
| Terminal access | `sfx_terminal_access.wav` | Electronic beep sequence |
| Meditation point (activate) | `sfx_meditation.wav` | Peaceful tone; cross-fades music slightly |
| Force Echo (trigger) | `sfx_force_echo.wav` | Soft ethereal sound before vision begins |

---

## Volume Levels

All volumes normalized against these defaults (0–128 scale, SDL2_mixer):

| Category | Default Volume | Notes |
|---|---|---|
| Music | 64 (50%) | Reduced to not overpower SFX |
| SFX — Player | 100 | |
| SFX — Enemies | 90 | |
| SFX — Force powers | 110 | Slightly louder for impact |
| SFX — UI | 80 | |
| SFX — Environment | 70 | Background atmosphere |

User-adjustable via pause menu in a later milestone.

---

## Audio Events System

Audio is triggered via an event-driven system, not polling. The `AudioManager` class (owned by `Game`) exposes:

```cpp
void playMusic(const std::string& trackId, bool fade = true);
void stopMusic(int fadeMs = 500);
void playSFX(const std::string& sfxId, int channel = -1, int loops = 0);
void stopChannel(int channel);
void setMusicVolume(int vol);   // 0–128
void setSFXVolume(int vol);     // 0–128
```

Entities trigger audio by posting to the `AudioManager` directly (no global event queue needed at this scale):
```cpp
// In CombatSystem, when a hit lands:
audioManager->playSFX("sfx_saber_swing_light", 3);
```

---

## Music Zone Transitions

The `ambient_zone` trigger in the tilemap fires an `AudioManager::playMusic()` call when Roan enters it. The `AudioManager` checks if the requested track is already playing — if so, does nothing (prevents restart glitch when re-entering a zone).

Fade duration: 1000ms (1 second) for zone-to-zone transitions, 2000ms for chapter-to-chapter transitions.

---

## Lightsaber Hum

The lightsaber hum is a **continuously looping** SFX on channel 0. It:
- Starts when the gameplay scene loads
- Loops with `Mix_PlayChannel(0, sfx_saber_hum, -1)` (infinite loop)
- Pitch-modulates slightly during combat (optional enhancement — not in MVP)
- Stops when the gameplay scene unloads

This is the one SFX that runs continuously rather than being event-triggered.
