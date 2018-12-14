
Key bindings:
--------------------------------------------------------------------------------------------------
	- When inside the Menu, press [1] or [2] to select a level;
	- [R] resets each level;
	- [B] moves to the position of the ball with it in the center of the screen;
	- [T] toggles the Quad tree debug mode;
	- [P] toggles the Pathfinding debug mode (and a ray from the robot to the player on level 2);
	- When inside each of the Levels or the Victory screen, press [M] to go back to the Menu.

Features:
--------------------------------------------------------------------------------------------------
	- 2 levels, selectable from the Menu when the game starts. Can easily include more since the level generation is data-driven but I was lazy to design new levels :(
	- The first one has two moving obstacles which will kill the player on collision, and two sandpits which affect the friction of the player when situated on top of them.
	- The second level contains a maze and a robot which uses the A* pathfinding algorithm to calculate the shortest route to the player when they are inside its detection range. The robot will also kill the player on collision.
	- Apart from that, the pathfinding as a whole is enhanced by triggering a state where the robot starts chasing the player directly when in direct view of them (via ray casting), ignoring the A* algorithm. Disclaimer: this may not work properly all the time :D
	- The ball can only be hit when it's at rest (just like in real golf, yey).
	- The counter in the top left corner of the screen keeps track of how many hits the player has committed and lights up in a different colour the more hits you make. (I just thought it looked cool :)
	- There are 3 FSMs: for the Game (i.e. menu, levels and end screen), for the Moving Obstacles and one for the Robot.
	- The game features AABB <-> AABB and AABB <-> Sphere collision volumes on a grid-based map and uses Broad and Narrow phase collision detection.
	- Has integrated Linear and Angular velocity;
	- A server and a client are spawned and the highscore is being calculated (the lowest value among the player scores is the best one, right?) and kept track of server-side. It is also being output in the console.
	- The networked game can support multiple clients being connected to it but doesn't update player positions/orientations yet. 
	- NOTE: If you want to change between the single-player and the multiplayer version, the only thing that needs to be done is to change the 'isNetworkedGame' in definitions.h
	You might also want to adjust the robot speed (and/or detection range) depending on the machine, I guess :)
	
=== TLDR; ===
	- From the Part A Guidelines, I have 2/3 and from the Part B, I have the first, as well as partially the second one (something like a multiplayer game in alternate realities?).