# Intro:

I had quite a bit of fun designing this game and thought about many different objectives that would involve a moving platform and a lander of some sort:

1. One idea I really liked was a UFO getting close to, but not landing on the surface of the "moon" to beam up a cow and returning to a home ship. Unfortunately, I couldn't find a good enough UFO sprite with a thrusting animation for this idea, and a "home" ship with a flat surface to land on. If I ever revisit this project with better artistic skills, that is what I will implement.

2. The second idea was landing on a flagpole and raising up a US flag, however, again, I could not find a good sprite for a space station to land on, nor did it make sense why the flag pole was already there.

3. The current and final "mission" idea involves grazing a moving spaceship to "steal" its flag, instead of landing on one. The mission brief is as follows:

# Mission:

- The "enemy" legion has landed on the moon before us!
- Your objective is to steal their flag from the top of their spaceship, while avoiding crashing into it.
- Afterwards, safely land on their flag on the moon, replacing it with ours.
- Be careful not to:
    - Run out of fuel
    - Land with a horizontal or vertical speed greater than 15
    - Land with an angle greater than 10 degrees
    - Land on their flag before stealing the one on the spaceship
    - Land anywhere else on the surface

# Tech stuff:

I enjoyed experimenting with the physics of the lander, and the constants for gravity, thrusting, and fuel rate could always use more tweaking in play tests. Currently, the game is difficulty but certainly winnable.

The map of the moon's surface is hardcoded, but I added a function to the `Map` class that determines the surface level of tiles on a map and creates a vector of their positions. That is how I randomize the position of the flag.

The spaceship moves along a sinusoidal wave with a randomized starting position, amplitude, and wavelength. I was able to achieve near-constant speed movement along the wave using some math.

I also implemented sound for the first time and came up with a function that lerps the volume of the thrusting sound so it doesn't start or stop abruptly.

# Controls:

- Press `W` to thrust
- Press `A` to turn left
- Press `D` to turn right
- Press `P` on game boot to start the game, and again at any time to pause the game
- Press `R` at any point to reset the game state to the initial state
- Press `Q` to quit

---

### Additional Notes:

- The design on the "enemy" flag is a bit hard to see because of the size on the game. I hope you get a laugh at who I chose as the "enemy" when inspecting the assets.
- Credit to Marisa for helping me attach the flag images to a flagpole sprite
