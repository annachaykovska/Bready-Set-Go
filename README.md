# BREADY SET GO!!! (Team 1)

## To Run
Double click the file in the Bread/ directory called "BreadySetGo!!!.exe" 

## Controls (Keyboard)
- **W**: Drive forwards
- **A**: Drive left
- **S**: Drive backwards
- **D**: Drive right
- **R**: Reset position to spawn

Note: Works only when controller is unplugged

## Controls (XBox Controller, plug-n-play)
- **LT**: Brake/Reverse
- **RT**: Accelerate
- **L Thumb**: Steering
- **R Thumb**: Camera Directional Influence (Rocket League Style)
- **START**: Reset position to start

## New Features and Changes Included
- **Level Design:**
  - Custom 3D kitchen-themed level mesh
  - Recipes:
    - Array of 3 recipes per player, can be specifically initialized
    - Keep track of completed recipes and check progress
    - Track player's inventory
  - Collision detection for level mesh
  - Collision detection between players
    - Random item transfer between players (collision roles are not handled yet, no way to figure out who hit who)
  - Boolean to set breadmobile as AI or player
- **Driving:**
  - XBox controller compatible
  - Faster engine
  - Better turning
  - Responsive braking
  - Braking while accelerating
  - More intuitive/fast transition between forwards and reverse and vice versa
  - Non-linear analog-output conversion (finer movement control with small joystick movements)
  - Wheel inertia dampened for faster collision to acceleration transition
  - Other players set up as physx vehicles
  - Position respawn button set
- **Camera:**
  - Perceived speed (changing perspective and distance from vehicle)
  - Rocket League Style camera directional influence with elastic force (right joystick when using controller)
  - Intuitive camera follow (exaggerates turning by variably offsetting the rotation)
- **UI:**
  - Minimap
  - Image rendering
  - Recipe progress tracking
  - Recipe completion identification
  - Speedometer
- **AI:**
  - Navigation mesh set over the map
  - Possible for the AI to win the game
  - AI handle shortest path planning to static and dynamic targets
  - AI handles decision making about which ingredient to target next
- **Rendering:**
  - Improved lighting model
  - Directional light shadow-mapping
- **Audio:**
  - 3D engine noises of other players
  - Doppler-shifting using player's speed
- **Other Changes:**
  - Bugfix for objects flying when window is moved

## Top Priority Upcoming Features:
- Offscreen indicators + minimap integration with ingredients
- Finalized kitchen level layout with additional obstacles
- Multiple vehicle models
- Varied + unique recipes per player
- Ingredient respawning
- Recipe delivery area
- Simple powerups
- Improved AI path smoothing

## Known Feature Limitations Identified While Testing (To Be Addressed In Next Milestone):
- Collision positioning/speed prescedence not implemented to decide who loses/gains ingredients (currently AI always gains).
- A perfect balance in braking has not yet been reached. The reaction is slightly too slow, and the punchiness of the gear change feels rather abrupt.
- Only one AI is active to ensure that a win state can always be achieved.
- Level mesh has cracks and bumps that are unpleasant to drive over.
- AI can become stuck against a wall if strategically bumped by the player.
- Camera clips through walls.

## Known Bugs Identified While Testing (To Be Addressed In Next Milestone):
- Player collisions do not have an adequate cooldown (full inventory becomes depleted upon collision).
- Physx bumpiness causes a perceived camera jitter when accelerating.
- Speedometer goes beyond range when falling.
- Camera becomes obscured when going down a ramp.
- Rendered position of a vehicle on a ramp appears floating.
