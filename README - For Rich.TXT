
Key bindings:
--------------------------------------------------------------------------------------------------
	- When inside the Menu, press [1] or [2] to select a level;
	- [R] resets each level;
	- [B] moves to the position of the ball with it in the center of the screen;
	- [T] toggles the Quad tree debug mode;
	- [P] toggles the Pathfinding debug mode;
	- When inside the Victory screen, press [M] to go back to the Menu.

Features:
--------------------------------------------------------------------------------------------------
	- 2 levels, selectable from the Menu when the game starts. Can easily include more since the level generation is data-driven but I was lazy to design new levels :(
	- The first one has two moving obstacles which will kill the player on collision, and two sand pits which affect the friction of the player when on top of them.
	- The second level contains a maze and a Nyan cat textured robot (:D) which uses the A* pathfinding algorithm to calculate the shortest route to the player. 
	  It will also kill the player on collision. (Maybe that's why I like dogs more...)
	- The ball can only be hit when it's at rest (just like in real golf, yey).
	- The counter in the top left corner of the screen keeps track of how many hits the player has committed and lights up in a different colour the more hits you make. (tbh, it's a shame if you get to the red...)
	- There are 3 FSMs: for the Game (i.e. menu, levels and end screen), for the Moving Obstacles and for the Robot.
	- The game features AABB <-> Sphere collision volumes on a grid-based map and uses Broad and Narrow phase collision detection.
	- Has integrated Linear and Angular velocity;
	- A server and a client are spawned and the highscore is being calculated (the lowest value among the players is the best one, right?) and kept track of server-side. It is also being output in the console.
	
	=== TLDR; ===
	- From the Part A Guidelines, I have 2/3 and from the Part B, I have the first, as well as partially the second one.