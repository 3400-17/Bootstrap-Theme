# Milestone 3: Maze Exploration

### Objectives
* Robot maze exploration using search algorithm (DFS, BFS, Dijkstra, etc.)
* Navigate a maze of size 4x5
* Update the maze on the GUI

## Implementation
### DFS Concept
We chose to start with depth first search (DFS) because it was the easiest to grasp and implement and we could build on it to make a more efficient algorithm moving forward.  To understand depth first search, we first drew out several diagrams to show what the robot would think.  DFS first explores one path fully from the root node, noting any alternative paths along the way.  When it hits a dead end, the robot backtracks back to an intersecion with a path branch in it and checks to see if any of the alternative paths are unexplored.  If so, it then seaerches the entire depth of that branch repeating the process again.  The robot continues this loop until there are no more alternative branches to explore. 

In graph format, we can follow the above rules and execute DFS such as the following:
![DFS Graph, Source: HackerEarth](https://snag.gy/qMwF4m.jpg)

However in our tile maze format, it is a little bit more complex as backtracking and remembering your previous path complicates things.  To remember our path, we have a stack that pushes every square that we travel through.  When we backtrack, locations are popped off of the stack.  In addition, we also have to store branches as we travel.  Branches are defined by squares that have multiple different directions that the robot can move in.  These branches are also stored in a stack such that we can  go back to the most recent branch by simply popping it and comparing it with the popped item from the travel stack.  

To operate, the robot first moves as far as it can with priority being straight (least amount of time), turn right, turn left, respectively until it encounters a dead end.  When it finds a dead end, it then moves back to the last branch.  If the branch has tiles that can be explored adjacent to it, the robot searches to the depth of that branch and then comes back.  If there is no feasible paths for the robot to explore, it then pops the next branch off of the branch stack and travels back again.  This happens until there are no more items in the branch stack at which there is no more squares in the maze to feasibly explore.  Lastly, we also have an array called visited which has length 81 (9x9) such that each location has an explored or unexplored status (1 = visited, 0 = unvisited).  This will track the robot's progress and will serve as the robot's comparator for minimizing driving over explored squares.

An example of this described navigation is shown in the video we created below:

<iframe width="560" height="315" src="https://www.youtube.com/embed/JFkDh5BSens" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

### Check Direction Function
The first thing we added to the robot was functions to check the front, left, and right side of the robot.  The three functions simply return a boolean value: true if there is no wall in respective location and if respective location is unvisited, false otherwise.  These three functions can be used in going to the deepest depth of a path and for deciding if a branch can be further explored.  They are implemented as follows:

~~~
//returns false if front tile has wall or has been visited
bool checkFront() {
  if (frontWallValue > 50) return false; // wall
  if (compass == 0 && visited[loc - 9] == 1) return false; //check north if facing north etc. etc. etc.
  if (compass == 1 && visited[loc + 1] == 1) return false;
  if (compass == 2 && visited[loc + 9] == 1) return false;
  if (compass == 3 && visited[loc - 1] == 1) return false;
  return true;
}

//returns false if left tile has wall or has been visited
bool checkLeft() {
  if (leftWallValue > 50) return false; // wall
  if (compass == 0 && visited[loc - 1] == 1) return false; //check west if facing north etc. etc. etc.
  if (compass == 1 && visited[loc - 9] == 1) return false;
  if (compass == 2 && visited[loc + 1] == 1) return false;
  if (compass == 3 && visited[loc + 9] == 1) return false;
  return true;
}

//returns false if right tile has wall or has been visited
bool checkRight() {
  if (rightWallValue > 50) return false; // wall
  if (compass == 0 && visited[loc + 1] == 1) return false; // check east if facing north etc. etc. etc.
  if (compass == 1 && visited[loc + 9] == 1) return false;
  if (compass == 2 && visited[loc - 1] == 1) return false;
  if (compass == 3 && visited[loc - 9] == 1) return false;
  return true;
}
~~~

### Depth Function
Next, we implemented a function that fully explores one path on the maze until it hits a dead end.  The logic is pretty simple.  The first priority is to go straight (mainly because it takes the least time) and as long as checkFront returns true, the robot will continue straight. If this is not an option, the robot will then check for right and left, respectively.  While it is moving, it also marks down each node that is a branch by checking if there are other paths to take.  If it is a branch, it pushes the location into the branch stack and continues.  Additionally as it travels, it also marks each new location down as explored by setting the respective index in the visited array to a 1. This happens until it detects that it cannot move forward, right, or left anymore indicating a dead end and breaking out of the function.
~~~
void depth() {
  while (1) {
    frontWallValue = map(analogRead(frontWall), 0, 1023, 0, 255);
    rightWallValue = map(analogRead(rightWall), 0, 1023, 0, 255);
    leftWallValue = map(analogRead(leftWall), 0, 1023, 0, 255);

    if (checkFront) { //no front wall
      if (rightWallValue < 50 || leftWallValue < 50) branches.push(loc);
      coast();
      stack.push(loc);
    } else if (checkRight) { //no right wall, but has front wall
      if (leftWallValue < 50 && visited[loc] == 0) branches.push(loc);
      turnRight();
    } else if (checkLeft) { //no left wall, has front and right wall
      turnLeft();
    } else {
      break;
    }
  }
}
~~~

### Backtracking - the prevBranch Function
Backtracking was also tricky to implement.  The function pops the location from the branches stack and then moves into a while loop comparing the robots current location with the branch it is trying to move to.  During this loop the robot popos the previous location from the location stack and compares it to its current location.  Based on our system in which locations are numbered in a looping manner, if a square is 9 less than your current location, it is north of you; if a square is 9 more than your current location, it is south of you; if a square is 1 less than your location, it is west of you; if a square is 1 more than your location, it is east of you.  With this in mind, we implemented several case statements in which the robot would find out which direction to move in and then turn to face that direction baed on its current compass reading (direction its front is facing).  It would then move to the new location and once again check if the location is equal to the branch location; if not, it pops the next location from the location stack and repeats.  Our code is outlined below. 

~~~
//moves back to previous branch
void prevBranch() {
  branch = branches.pop();
  while (loc != branch) { //need to move back to the branch location
    prev = stack.pop();
    if ((loc - prev) == 1) { //need to move W
      switch (compass) {
        case (0): //north
          turnLeft();
          coast();
          break;
        case (1): //east
          turnRight();
          turnRight();
          coast();
          break;
        case (2): //south
          turnRight();
          coast();
          break;
          break;
        case (3): //west
          coast();
          break;
      }
    } else if ((loc - prev) == 9) { //need to move N
      switch (compass) {
        case (0): //north
          coast();
          break;
        case (1): //east
          turnLeft();
          coast();
          break;
        case (2): //south
          turnRight();
          turnRight();
          coast();
          break;
          break;
        case (3): //west
          turnRight();
          coast();
          break;
      }

    } else if ((loc - prev) == -9) { //need to move S
      switch (compass) {
        case (0): //north
          turnRight();
          turnRight();
          coast();
          break;
        case (1): //east
          turnRight();
          coast();
          break;
        case (2): //south
          coast();
          break;
          break;
        case (3): //west
          turnLeft();
          coast();
          break;
      }
    } else { //need to move E
      switch (compass) {
        case (0): //north
          turnRight();
          coast();
          break;
        case (1): //east
          coast();
          break;
        case (2): //south
          turnLeft();
          coast();
          break;
          break;
        case (3): //west
          turnRight();
          turnRight();
          coast();
          break;
      }
    }
  }
}
~~~
### Loop
With our subfunctions, our loop was actually pretty simple.  We ran depth first to start the robot off.  The remainder of the loop simply runs our backtracking function and then checks for unexplored paths on branches in a while loop until there are no more branches to explore.  When the robot finishes, it turn on an LED and stops moving.

~~~
void loop() {
  depth();  // go to the depth of a path
  while (branches.isEmpty() == false) {
    prevBranch(); // go to last visited node with another option
    //check if any paths from branch are feasible
    if (checkFront) {
      depth();
    } else if (checkRight) {
      turnRight();
      depth();
    } else if (checkLeft) {
      turnLeft();
      depth();
    } else { //no options at branch so go to next branch
      delay(10);
    }
  }
  digitalWrite(2, HIGH);//light up an LED
  while(1); //finished exploring so stop
}
~~~

### Video
