# CPSC-585-Game-Project

## To Run
Double click the file in the Bread/ directory called "BreadySetGo!!!.exe" 

## Controls
**W**: Drive forwards

**A**: Drive left

**S**: Drive backwards

**D**: Drive right

## Features Included
- Rendered models for ground, vehicle and collectable ingredients
- Basic keyboard controls
- Basic lighting
- PhysX integration
  - Working driving model
  - Working collision detection between different scene actors
- Music and sound effects
  - Effects for collection and completion of goal
- Tethered camera following the first player
- A single recipe to drive around and collect (4 ingredients)
  - Collection is tracked by the ImGUI profiler on the screen, completion is shown to be registered there as well

## IMPORTANT NOTES:
Please don't drag the window when the game is running. PhysX gives us a fun bug where the RigidDynamic actors fly around the world and the car sometimes clips through the ground.
