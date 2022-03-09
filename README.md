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
- **X**: Handbrake
- **START**: Reset position to start

## New Features and Changes Included
- Level Design:
  - Custom 3D kitchen-themed level mesh
  - Recipes:
    - Array of 3 recipes per player, can be specifically initialized
    - Keep track of completed recipes and check progress
    - Track player's inventory
  - Collision detection for level mesh
  - Collision detection between players
    - Random item transfer between players (collision roles are not handled yet, no way to figure out who hit who)
  - Boolean to set breadmobile as AI or player
- Driving:
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
- Camera:
  - Perceived speed (changing perspective and distance from vehicle)
  - Rocket League Style camera directional influence with elastic force (right joystick when using controller)
  - Intuitive camera follow (exaggerates turning by variably offsetting the rotation)
- UI:
  - Minimap
  - Image rendering
  - Recipe item tracking
  - Speedometer
- AI:
  - Navigation mesh set over the map
  - Possible for the AI to win the game
- Rendering:
  - Improved lighting model
  - Multiple light casters (point and directional lighting)
  - Directional light shadow-mapping
- Other Changes:
  - Bugfix for objects flying when window is moved

<!-- ## IMPORTANT NOTES AND KNOWN BUGS:
- Player collision spams warnings (that don't break things yet) -->

