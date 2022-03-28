# BREADY SET GO!!! (Team 1)

## To Run
Double click the file in the Bread/ directory called "BreadySetGo!!!.exe" 

## Controls (XBox Controller, plug-n-play)
- **LT**: Brake/Reverse
- **RT**: Accelerate
- **L Thumb**: Steering and menu selection
- **R Thumb**: Camera Directional Influence (Rocket League Style)
- **A**: Menu accept
- **X**: Handbrake
- **Y**: Vacuum ability
- **START**: Reset position to start
- **BACK**: Pause menu

## Controls (Keyboard - *Currently unsupported*)
- **W**: Drive forwards
- **A**: Drive left
- **S**: Drive backwards
- **D**: Drive right
- **R**: Reset position to spawn

Note: Keyboard controls were broken sometime during the merge-fest of the previous milestone. The functionnality is not important as it was meant for debug purposes for the most part, so we ignored the issue.

## New Features and Changes Included
- **Level Design and Gameplay:**
  - New and improved kitchen, with more verticality, and less crevices to get stuck on. 
  - New unreachable fan that casts a moving shadow on the countertop.
  - Recipes:
    - Different recipes per AI/Player each with some ingredient overlap.
  - New vacuum ability to steal ingredients from other breadmobiles.
  - Randomized location for ingredient spawns. 
  - If ingredients are pushed off the edge of the map, they respawn (violently) to avoid an unwinnable state.

- **Driving:**
  - Faster acceleration.
  - More responsive braking.
  - Speed dissipation.
  - Handbrake, for drifting. :0
  - Tighter turn radius, affected by the speed of breadmobile.

- **Camera:**
  - More exaggerated camera, FOV and rotation is now quicker.
  - FOV response is now non-linear, having a large jump part way through speeding up.
  - Capped maximum change in camera per frame, when crashing it should be less jarring.

- **UI:**
  - New start, pause and end menus, with controller navigation.
  - Minimap updated for new layout.
  - New ingredient icons.
  - Ingredient location indicators.
  - Recipe progress indicators for all Breadmobiles.
  - Vacuum ability cooldown indicator.
  - Speedometer has a capped max change (looks better with sudden changes), as well as a max rotation it can reach.

- **AI:**
  - Navigation mesh set over the map.
  - Possible for the AI to win the game.
  - Handles shortest path planning to static and dynamic targets.
  - Picks the best available target (closest position).
  - Recognizes when a player has picked up an ingredient and replans its path to chase the player accordingly.
  - Briefly parks and tries to retarget an ingredient if it is near it, but not being collected (driving circles around the target).
  - Handles basic navigation to return to the navmesh if the current location falls outside the mesh area.
  - Handles automatic respawn and replan when falling off the level.
  
- **Rendering:**
  - Improved near and far shadows for both the environment and actors within.
  - Skybox for the kitchen background.
  - More natural lighting.
  - Textures for the environment.
  - New and different models for the vehicles.
  - Floor (kitchen is no longer floating).

- **Audio:**
  - New main menu music.
  - New audio for the vacuum ability.

- **Other Changes:**
  - PhysX jitter is now removed.
  - Time step has been changed so PhysX runs consistently on varying framerates.

## Top Priority Upcoming Features:
  - Better feeling and balanced vacuum ability.
  - More varied recipes.
  - Vehicle flipping, to recover from tipping over.
  - Punishments for falling off of map.
  - Make the game even more visually compelling.

## Known Feature Limitations Identified While Testing (To Be Addressed In Next Milestone):
  - Some bridges and ramps are difficult to traverse.
  - Vacuum ability is too instant too feel fair.
  - Ingredient location indicators can be confusing when the ingredient is lower or higher than the player.

## Known Bugs Identified While Testing (To Be Addressed In Next Milestone):
- Camera becomes obscured when going down a ramp.
- There are some shadow artifacts on the back of the fridge wall, as well as the top of some ramps.
- Keyboard controls no longer entirely work.

