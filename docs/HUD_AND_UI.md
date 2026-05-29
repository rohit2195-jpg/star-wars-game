# HUD & UI Specification

## Architecture

All UI is rendered in **screen space** (not world space). The `RenderSystem` draws the game world first, then the HUD layer on top. The HUD is not part of the Entity system — it is a separate `HUD` object owned by `GameplayScene`.

Menus (title, pause) are separate `Scene` subclasses pushed onto the `SceneManager` stack.

---

## HUD Layout (1280×720 viewport, zoom 2×)

```
┌────────────────────────────────────────────────────────────────────────────────┐
│ [HP  ████████████████░░░░░░]    180/200          Force Push  [▓▓░░]  15s      │  ← top bar
│ [FP  ██████████░░░░░░░░░░░░]     75/100                                        │
│                                                                                │
│                                                                                │
│                         (GAME WORLD)                                           │
│                                                                                │
│                                                                                │
│                                                           [Q]  ⚡ Force Push   │  ← bottom-right
│                                                           [E]  Force Wheel     │
└────────────────────────────────────────────────────────────────────────────────┘
```

### HP Bar (top-left)

- Position: `(16, 16)` screen pixels
- Bar width: 200px, height: 16px
- Label: "HP" text to the left (8px font)
- Fill color: changes with HP percentage:
  - 100–60%: `#4CAF50` (green)
  - 60–30%: `#FF9800` (orange)
  - 30–0%: `#F44336` (red)
- Background (empty bar): `#333333`
- Border: 1px `#666666`
- Damage flash: bar briefly flashes white when HP drops, then transitions to new value with a 0.3s tween

### FP Bar (top-left, below HP bar)

- Position: `(16, 40)` screen pixels
- Same dimensions as HP bar
- Fill color: `#3F9FFF` (blue)
- Drain flash: brief cyan flash when a power is used
- Regen visual: subtle shimmer animation while regenerating (can skip for MVP)

### Active Power Indicator (bottom-right)

- Position: `(1180, 660)` screen pixels
- Shows: icon of current power + cooldown arc overlay
- Cooldown arc: circular sweep animation (0 → full as cooldown expires)
- Key binding reminder: `[Q]` text above icon
- `[E]` hint below with "Hold for wheel" text (small, 6px font)

---

## Force Power Wheel

Opened by holding `E`. Centered on screen.

```
                   [Force Lift]
                       ↑
   [Mind Trick]  ←   [CENTER]   →  [Force Push]
                       ↓
                   [Force Pull]

          [Slow Time]         [Force Sight]
```

**Layout:** 6 powers arranged in a hexagonal radial pattern around a center point.

**Visual design per slot:**
- Icon: 32×32 power icon from `ui_force_wheel.png`
- Locked powers: greyed out with a lock icon overlay
- Hovered power: highlighted border, name displayed in center
- Current selected power: slightly larger icon

**Selection:** Mouse position (or right stick on gamepad) determines which sector is highlighted. Release `E` to confirm selection.

**FP display:** Each power icon shows its FP cost as a small number below the icon.

---

## Pause Menu

Pushed as a `PauseScene` on top of `GameplayScene` when Escape is pressed. The game world is still rendered underneath (blurred or darkened).

```
┌─────────────────────────────┐
│         PAUSED              │
│                             │
│   ▶  Resume                 │
│      Journal                │
│      Lightsaber             │
│      Settings               │
│      Quit to Title          │
└─────────────────────────────┘
```

Navigation: arrow keys or D-pad. Confirm: Enter / A button.

### Journal Submenu

Lists all collected Journal Pages, grouped by chapter. Unread pages are bolded.

### Lightsaber Submenu

Shows collected Lightsaber Parts. Player can select hilt appearance (cosmetic only). Live preview of Roan's lightsaber render.

### Settings Submenu

```
Music Volume    [████████░░]  64
SFX Volume      [██████████]  100
Fullscreen      [ OFF ]
Resolution      [ 1280×720 ▼ ]
```

Changes apply immediately and are written to the save file.

---

## Title Screen

```
┌────────────────────────────────────────────────────────────────────────────────┐
│                                                                                │
│                       ROAN NOVACHEZ                                            │
│                   AND THE SITH HOLOCRON                                        │
│                                                                                │
│                   ▶  New Game                                                  │
│                      Continue         (greyed if no save)                     │
│                      Settings                                                  │
│                      Quit                                                      │
│                                                                                │
│                       [animated background: stars, slow parallax]              │
└────────────────────────────────────────────────────────────────────────────────┘
```

---

## Dialog Box (Comic Panel)

Used for cutscenes and in-world dialogue. Rendered as a `DialogScene` pushed over the gameplay scene.

### Layout

```
┌─────────────────────────────────────────────────────┐
│  [CHARACTER PORTRAIT]   CHARACTER NAME               │
│                        "Dialog text appears here     │
│                         one character at a time,     │
│                         typewriter style."           │
│                                                      │
│                                    [Press F to advance]│
└─────────────────────────────────────────────────────┘
```

- **Portrait:** 64×64 character sprite (head/bust crop from character sprite sheet)
- **Name plate:** Bold text, character's color-coded accent
- **Text area:** Typewriter effect — characters appear one by one at ~40 chars/second
- **Advance prompt:** Blinks at bottom-right when text is complete
- Pressing `F` (or A button) during typewriter effect: skip to full text
- Pressing `F` when text is complete: advance to next line or close dialog

### Comic Panel Cutscenes

Full-screen or partial-screen PNG strips for major moments (prologue rescue, chapter transitions). These are pre-authored images.

A `CutscenePlayer` class displays them in sequence:
- Fade in panel PNG → wait for input → fade out → next panel
- Audio: ambient cutscene music plays during panels
- Skip: holding `F` for 1s skips to end of cutscene

---

## Collectible Pickup Notification

When a collectible is picked up, a small notification slides in from the right edge:

```
                                    ┌────────────────────┐
                                    │ 📄 Journal Found    │
                                    │ "Jedi Outpost Log"  │
                                    └────────────────────┘
```

- Slides in over 0.3s, displays for 2.5s, slides out over 0.3s
- Stacks vertically if multiple collected quickly
- Does not pause gameplay

---

## Damage Numbers (Optional Enhancement)

When an enemy takes damage, a small number floats upward from the hit position and fades out over 0.8s. Color-coded:

| Damage Source | Color |
|---|---|
| Normal attack | White |
| Force power | Blue |
| Critical (backstab) | Yellow |
| Force vulnerability bonus | Cyan |

This is optional — implement if it feels good during playtesting.

---

## Death Screen

When Roan's HP reaches 0:
1. `death` animation plays
2. Screen fades to black over 1.5s
3. Text fades in: "You have fallen."
4. After 2s: two options appear:
   - "Restore from Meditation Point" — reload from last save
   - "Quit to Title"

---

## Chapter Title Card

When transitioning between chapters, a brief title card appears:

```
(black screen, fading in)

  CHAPTER 1

  Arrival

(fades to gameplay)
```

Duration: 3 seconds total (0.5s fade in, 2s display, 0.5s fade out).

---

## Progress Saved Notification

After resting at a Meditation Point, a brief notification:

```
                              ┌────────────────┐
                              │  Progress saved │
                              └────────────────┘
```

Appears at bottom-center, fades out after 2 seconds.

---

## HUD Rendering Order

1. Game world (background → midground → foreground → entities → overlay)
2. Force Echo visions (full-screen overlay, semi-transparent)
3. Force Sight mode (full-screen desaturation shader / overlay)
4. HUD elements (HP bar, FP bar, power indicator)
5. Pickup notifications
6. Damage numbers
7. Dialog box (if active)
8. Pause menu (if active)
9. Cutscene panels (if active)
10. Fade-to-black overlay (scene transitions)
