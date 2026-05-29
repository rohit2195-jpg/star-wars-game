# Save System

## Overview

The save system serializes game progress to a JSON file on disk. No database, no cloud — a single local file. Loading is instantaneous.

Save is triggered by:
1. Resting at a **Meditation Point** (manual)
2. Entering a **checkpoint trigger** in the level (automatic, silent)
3. Collecting a key item (automatic, silent)

---

## Save File Location

```
macOS:   ~/Library/Application Support/StarWarsGame/save.json
Linux:   ~/.local/share/StarWarsGame/save.json
Windows: %APPDATA%\StarWarsGame\save.json
```

The path is resolved at runtime using SDL's `SDL_GetPrefPath("RoanStudios", "StarWarsGame")` which handles platform differences automatically.

---

## Save File Format

```json
{
  "version": 1,
  "timestamp": "2024-01-15T14:32:00Z",

  "player": {
    "currentLevel": "ch1_arrival",
    "spawnPointId": "meditation_02",
    "facingDirection": "right",
    "hp": 180,
    "maxHp": 200,
    "fp": 75,
    "maxFp": 100
  },

  "progression": {
    "chapter": 1,
    "unlockedPowers": ["force_push", "force_pull"],
    "upgrades": {
      "force_push": ["extended_push"],
      "force_pull": [],
      "force_lift": [],
      "mind_trick": [],
      "slow_time": [],
      "force_sight": []
    },
    "doubleJumpUnlocked": false,
    "wallJumpUnlocked": true
  },

  "collectibles": {
    "holocronFragments": ["prologue_power_room", "ch1_crater_fields"],
    "journalPages": ["prologue_archive_a"],
    "lightsaberParts": ["ch1_sensor_tower"],
    "forceEchoes": []
  },

  "discoveredMeditationPoints": [
    "prologue_meditation_01",
    "ch1_meditation_01"
  ],

  "defeatedBosses": [],

  "settings": {
    "musicVolume": 64,
    "sfxVolume": 100,
    "fullscreen": false,
    "resolutionWidth": 1280,
    "resolutionHeight": 720
  }
}
```

---

## Key Design Decisions

**Why one file (no slots)?**  
Single-player passion project. Multi-slot saves can be added later if needed. Keeping it simple.

**Why JSON (not binary)?**  
Human-readable. Easy to debug. Easy to edit manually. Performance is fine — the file is tiny and only read/written at save points, never mid-frame.

**Why store collectibles by ID (not count)?**  
Using unique string IDs per collectible prevents save corruption if the level is edited and collectible counts change. A collectible with ID `ch1_crater_holocron` is always that specific pickup — order and count don't matter.

**Respawn behavior:**  
On load, Roan is placed at `spawnPointId` with the HP/FP stored in the save. This means if you rested at a Meditation Point with low HP, you respawn with low HP. Resting fully restores HP/FP before saving (see Progression doc), so this is always full HP unless a checkpoint was hit in combat.

---

## Save/Load C++ Interface

```cpp
class SaveSystem {
public:
    bool save(const GameState& state);
    bool load(GameState& stateOut);
    bool hasSaveFile();
    void deleteSave();

private:
    std::string getSavePath();   // uses SDL_GetPrefPath
};

struct GameState {
    std::string currentLevel;
    std::string spawnPointId;
    int         hp, maxHp;
    int         fp, maxFp;
    int         chapter;

    std::vector<std::string> unlockedPowers;
    std::map<std::string, std::vector<std::string>> upgrades;
    bool        doubleJumpUnlocked;

    std::set<std::string> collectedHolocrons;
    std::set<std::string> collectedJournals;
    std::set<std::string> collectedSaberParts;
    std::set<std::string> collectedEchoes;

    std::set<std::string> discoveredMeditationPoints;
    std::set<std::string> defeatedBosses;

    int musicVolume, sfxVolume;
    bool fullscreen;
    int resWidth, resHeight;
};
```

**JSON library:** Use **nlohmann/json** (single-header, `third_party/json.hpp`). Serialize/deserialize `GameState` directly using its structured binding support.

---

## Save on Meditation Point

When Roan interacts with a Meditation Point:
1. Meditation animation plays (`meditate` clip, 6 frames)
2. HP and FP are restored to current max
3. `GameState` is populated from the current game state
4. `SaveSystem::save()` writes to disk
5. "Progress saved" brief UI indicator appears (2 seconds, fades out)
6. All enemies in the current level segment respawn

---

## New Game / Title Screen Flow

```
Title Screen
  ├── "Continue" — only shown if save file exists
  │       └── SaveSystem::load() → GameplayScene with loaded state
  └── "New Game"
          └── Delete existing save → fresh GameState → Prologue scene
```

---

## Save Corruption Handling

If `SaveSystem::load()` fails (malformed JSON, missing fields, wrong version):
1. Log the error
2. Present a dialog: "Save file could not be loaded. Start a new game?"
3. On confirm: delete corrupted file, start fresh
4. On cancel: return to title screen

**Version field:** Increment `"version"` in the JSON schema whenever the save format changes incompatibly. The loader checks the version first and falls back to "start new game" if mismatched rather than trying to parse an old format.
