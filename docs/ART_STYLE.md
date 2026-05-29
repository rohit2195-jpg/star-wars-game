# Art Style Guide

## The Target Aesthetic

**One-line summary:** Pokémon-quality pixel art environments with Hollow Knight-level character animation fluidity — indie craftsmanship, not retro minimalism.

This is not "retro 8-bit." It's modern pixel art that uses the medium intentionally: rich, layered backgrounds with real depth, and characters that move expressively enough to convey every Jedi action without feeling stiff.

### Reference Games

| Game | What to take from it |
|---|---|
| **Hollow Knight** | Character animation quality — weight, anticipation frames, follow-through. Every action reads instantly. Fluid combat that feels great to control. |
| **Dead Cells** | Combat animation speed and snap. Hits feel impactful. Enemy death animations are satisfying. Lots of frames on attacks. |
| **Pokémon (GBA / DS era)** | Environment and tilework quality — richly detailed backgrounds, foreground props, atmospheric color palettes per zone. Each area feels like a distinct place. |
| **Pokémon Mystery Dungeon** | Side-scrolling pixel environments with that warm, detailed Pokémon tile aesthetic applied to caves, ruins, forests. |
| **Celeste** | Use of color and light to define mood. Every screen is composed, not just functional. Backgrounds tell a story. |
| **Shovel Knight** | How to do expressive pixel art characters with limited size — clear silhouettes, readable poses, strong keyframes. |

---

## Environments (Pokémon Tilework Quality)

### Philosophy
Each chapter's environment should feel like a distinct Pokémon biome — you know immediately where you are from the color palette, tile shapes, and background details. The difference is these are side-scrolling with parallax depth instead of top-down.

### Layer System (4 visible layers)

| Layer | Content | Example |
|---|---|---|
| **Sky / Deep Background** | Distant atmosphere — stars, sky gradient, far mountains | Moon's orange horizon, Sith cave's glowing magma ceiling |
| **Background** | Architectural structures, distant ruins, cave walls | Jedi outpost pillars, carved stone walls, cliff faces |
| **Midground** | Interactive environment, terrain, decorative props | Platforms, crates, vines, rubble, foreground rocks |
| **Foreground Overlay** | Depth elements rendered in front of the player | Hanging vines, stalactites, archway edges, close-up foliage |

### Tile Size
- **16×16 px tiles** (Pokémon standard) for the base grid
- Characters and props can span multiple tiles (e.g., a large archway is 4×3 tiles = 64×48 px)
- Roan's sprite: **48×48 px** — 3×3 tiles, giving room for the lightsaber swing arc

### Per-Chapter Palette Signatures

| Chapter | Primary Palette | Mood |
|---|---|---|
| Prologue | Steel grey, dim blue emergency lighting, muted orange sparks | Abandoned, tense |
| Chapter 1 | Dusty orange sky, warm tan rock, pale green sparse vegetation | Desolate but open |
| Chapter 2 | Aged stone beige, faded Jedi blue-grey, moss green | Ancient, melancholy |
| Chapter 3 | Deep blue-black, bioluminescent teal crystal glow, wet stone grey | Mysterious, alien |
| Chapter 4 | Dark crimson, corrupted purple energy, cracked black stone | Oppressive, evil |
| Chapter 5 | Pure black void with golden ancient Sith script glowing | Final, mythic |

### Environmental Detail Density
Every tile screen should have:
- At least 3 background elements (not just flat color)
- Foreground props that break the "box" silhouette of the level
- An animated element somewhere on screen (dripping water, flickering light, blowing dust)
- Readable depth — player should clearly know what's foreground, midground, background

---

## Characters (Hollow Knight / Dead Cells Animation Quality)

### Philosophy
Fluid does not mean rotoscoped or hyper-realistic. It means every action has:
1. **Anticipation frame(s)** — the wind-up before an action (crouch before a jump, drawback before a swing)
2. **Strong keyframe** — the peak of the action (highest point of jump, saber fully extended)
3. **Follow-through frame(s)** — the settle after an action (hair settling, lightsaber arc completing)
4. **Squash and stretch** — slight distortion on impacts (landing squash, jump stretch)

This is what makes Hollow Knight's Knight feel satisfying to control despite being small on screen. The same principle applies to Roan.

### Animation Frame Budgets

| Action Type | Minimum Frames | Target Frames | Notes |
|---|---|---|---|
| Idle | 6 | 8–10 | Subtle breathing, cape movement |
| Walk | 8 | 10–12 | Clear weight shift |
| Run | 8 | 10 | Slight lean, cape trailing |
| Jump (full arc) | 10 total | 12–16 total | Separate rise / apex / fall states |
| Light attack | 8 | 10–12 | Anticipation + swing + follow-through |
| Heavy attack | 10 | 12–16 | Longer wind-up, bigger follow-through |
| Force Push | 6 | 8 | Hand extend → blast → recoil |
| Force Lift (hold) | 4 loop | 4–6 loop | Strain visible in pose |
| Hit reaction | 4 | 5–6 | Convincing recoil |
| Death | 10 | 12–16 | Respectful, not comedic |

### Sprite Dimensions

| Character | Sprite Size | Notes |
|---|---|---|
| Roan Novachez | 48×48 px | 3×3 tiles; lightsaber extends to edge of frame |
| T1 Droid | 32×32 px | 2×2 tiles |
| T2 Droid (shielded) | 36×40 px | Slightly larger for bulk |
| Stormtrooper | 32×40 px | Humanoid proportions |
| Force Shadow | 32×40 px | Deliberately similar silhouette to Roan |
| Sith Acolyte | 40×48 px | Slightly taller than Roan |
| Darth Vaelen | 48×56 px | Notably larger presence |
| Companion NPCs (Pasha, Gaiana, Tazi) | 32×40 px | Lighter sprite; less combat detail needed |

### Character Design Principles

- **Readable silhouettes:** Roan's lightsaber must be visible and distinct in every combat pose. Even at game resolution the player should be able to tell what attack is happening.
- **Color-coded at a glance:** Green lightsaber = Roan. Red = Sith. Each enemy type has a signature shape/color that's instantly distinguishable.
- **Expressive without faces:** Take a cue from Hollow Knight — characters wear helmets or have minimal facial detail, but body language and animation carry full emotion.
- **Indie craftsmanship feel:** Sprites should feel hand-made, not generated. Slightly imperfect charm is better than sterile perfection.

---

## Lighting and Atmosphere

No dynamic real-time lighting engine (too complex). Simulate atmosphere through:

1. **Background color tinting** — each area's deep background layer uses a strong color that bleeds atmosphere into the scene
2. **Animated particles** — floating dust motes, falling ash, glowing crystals, drifting Force energy wisps
3. **Foreground darkening** — foreground overlay tiles are slightly darker/more saturated than midground, reinforcing depth
4. **Enemy glow effects** — Force shadow enemies have a faint animated glow halo (drawn as a semi-transparent colored circle)

### Lightsaber Glow
Roan's green lightsaber should emit a soft local glow on nearby tiles when in combat. Implement as a small semi-transparent circle SDL_Texture drawn at the saber tip position each frame, using SDL_BlendMode additive blending.

---

## HUD Aesthetic

The HUD should feel like it belongs to the same world — not a generic UI pasted on top.

- Bars use a stone/metal texture from the game's tileset rather than flat colors
- Icons are pixel art, not flat symbols
- Font is a pixel font with a clean, slightly angular style (Star Wars-adjacent without being the actual SW font)
- The Force power wheel uses the game's energy color palette (blue glow, no neon)

---

## What to Avoid

| Avoid | Why |
|---|---|
| Flat single-color backgrounds | Kills the Pokémon environment richness |
| Low frame count on player attacks | Makes combat feel stiff — this is the one area to never cut frames |
| Generic UI (flat health bars, default fonts) | Breaks immersion in the world |
| Inconsistent tile scale | Characters and environment must feel like they exist in the same space |
| Overly dark, unreadable scenes | Chapter 4 is dark — but enemies and platforms must always be legible |
| Blurry scaling | SDL hint `SDL_HINT_RENDER_SCALE_QUALITY = 0` is already set; never change this |
