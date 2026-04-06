# Objective:

- LilGuy has set out on a Lil Venture across three increasingly dangerous worlds!
- Your objective is to reach the flagpole at the end of each level without running out of lives.
- Watch out for enemies - touching them will cost a life
- If you fall off the map in Levels 2 or 3, you'll respawn at the start with a life deducted.
- Lose all three lives and it's game over.

# Tech stuff:

I had a lot of fun building out the entity-component architecture for this project. The `Entity` base class handles physics, map collision, rendering, and animation, and all enemies and the player derive from it. I decided not to resolve enemy-player collisions, and instead let the player pass through enemies but take damage.

The enemy AI is implemented through a pure virtual `AIActivate` method that each subclass overrides. The `Enemy` base class defines several AI actions, and the `AIActivate` method acts as the state mechanism between them. The three enemy AIs are:

- `Wanderer` paces back and forth, flipping on walls and edges.
- `Follower` tracks the player within a certain range.
- `Jumper` triggers a jump on a configurable interval.

The life system persists across all three scenes as a global, and is rendered in screen space outside of `BeginMode2D` so it stays fixed to the HUD. The flagpole subclasses `Entity` and uses the existing animation atlas system: idle state shows the lowered flag, and the walk state animates it raising.

I also added a non-solid tile system to the `Map` class using a `std::set<int>` of tile indices, which lets decorative tiles render without affecting collision. The camera uses smooth lerp-based panning with boundary clamping.

# Controls:

- Press `A` to move left
- Press `D` to move right
- Press `W` to jump
- Press `1`, `2`, or `3` to skip directly to a level
- Press `Enter` on the title, death, or win screen to continue
- Press `Q` to quit

---

### Additional Notes:

- The enemy speed and follow range values could always use more tuning.
- Still lots of files with repeated code, despite my best attempts to organize. Could be cleaned up significantly.
- Credit to Kenney for the sprites and tileset.
