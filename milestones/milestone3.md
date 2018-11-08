# Milestone 3: Maze Exploration

### Objectives
* Robot maze exploration using search algorithm (DFS, BFS, Dijkstra, etc.)
* Navigate a maze of size 4x5
* Update the maze on the GUI

## Implementation
### DFS Concept
We chose to start with depth first search (DFS) because it was the easiest to grasp and implement and we could build on it to make a more efficient algorithm moving forward.  To understand depth first search, we first drew out several diagrams to show what the robot would think.  DFS first explores one path fully from the root node, noting any alternative paths along the way.  When it hits a dead end, the robot backtracks back to an intersecion with a path branch in it and checks to see if any of the alternative paths are unexplored.  If so, it then seaerches the entire depth of that branch repeating the process again.  The robot continues this loop until there are no more alternative branches to explore. 

In graph format, we can follow the above rules and execute DFS such as the following:
![DFS Graph](https://snag.gy/rJ4hst.jpg)

However in our tile maze format, it is a little bit more complex as backtracking and remembering your previous path complicates things.  To remember our path, we have a stack that pushes every square that we travel through.  When we backtrack, locations are popped off of the stack.  In addition, we also have to store branches as we travel.  Branches are defined by squares that have multiple different directions that the robot can move in.  These branches are also stored in a stack such that we can  go back to the most recent branch by simply popping it and comparing it with the popped item from the travel stack.  

To operate, the robot first moves as far as it can with priority being straight (least amount of time), turn right, turn left, respectively until it encounters a dead end.  When it finds a dead end, it then moves back to the last branch.  If the branch has tiles that can be explored adjacent to it, the robot searches to the depth of that branch and then comes back.  If there is no feasible paths for the robot to explore, it then pops the next branch off of the branch stack and travels back again.  This happens until there are no more items in the branch stack at which there is no more squares in the maze to feasibly explore.  An example is shown in the video we created below:

<iframe width="560" height="315" src="https://www.youtube.com/embed/JFkDh5BSens" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

### Check Direction Function
The first thing we added to the robot

###
