# Final Robot Design

## Introduction

The goal throughout a semester was to create an intelligent physical system that can collect information from its environment to determine its next actions.  The system we created took form of a maze mapping robot able to utilize a maze solving algorithm, transfer information to a GUI, detect other robots, and find treasures in the forms of colorful shapes on the wall.  On top of this, the robot is able to start on a 660 Hz tone and utilize white lines on the ground to stay on course and determine its position.  

## Physical Design
Our design was made with lasercut wood pieces and took the form of a box measuring approximately 140 mm x 120 mm x 140 mm (length width height).  We were able to hide our wheels in the bottom of the design and embed most of the sensors within the compartments.  The following image shows our designs for the laser cutter.  We utilized screw joints to connect the lasercut pieces together 3 dimensionally and cut out appropriate holes for mounting components. The top of the robot can be removed to expose the "brains" of the robot.

![DXF](https://snag.gy/G24k6O.jpg)

This design made our system more robust, hid some of the electronic components, and just simply looked good.  We also painted our final model black to give it a finishing look.  The three pins on top are to attach the IR hat and the IR sensor peaks from the crack in the front to detect other robots.  

![robot](https://snag.gy/3gWulz.jpg)

As seen in the next picture, the back of the robot has a hinge to access the important electronics including replacing/plugging in new batteries, uploading to the arduino and checking on the wheels and line sensors. On the bottom back there is also a LED strip which gives the robot a floating appearance when moving.
![hinge](https://snag.gy/bQR1JT.jpg)

Looking down from the top of the robot, we can also see that the lid can be removed revealing more components.  The FPGA is mounted to the side along with the camera.  One battery sits on the top layer of the robot while another one sits on the base layer on top of the batteries.  Two power strips are utilized for each power source.  One power supply powers the servos and LED strip.  The other supply powers the arduino, wall sensors, line sensors, robot detection, microphone and multiplexer.  The arduino is screwed to the middle plate so that it is secured and will not be jostled and our protoboard containing IR detection and microphone sits on top of the arduino.  

![top view](https://snag.gy/TG87bP.jpg)

#### Pin Assignments:

* D2: Color Detection bit 0
* D3: Left motor PWM
* D4: Color Detection bit 1
* D5: Right motor PWM
* D6: Color Detection bit 2
* D7: Multiplexer control
* D8: Start button
* D9-13: Radio
* A0: FFT funtions (microphone and IR)
* A1: left wall sensor
* A2: right wall sensor
* A3: front wall sensor
* A4: left line sensor
* A5: right line sensor

#### Protoboards:
After testing our parts for functionality, we then worked to make our parts more manageable. We decided to mount our systems on a more compact and robust protoboard. We soldered two 358 op amp ICs and a multiplexer.  We then assembled our amplifier and filter circuitry and connected it such the output was controlled by the MUX. The protoboard is shown below:

![FFT](https://snag.gy/eZTkPi.jpg)

In addition, we also made a protoboard for the camera, solifying the connections to the FPGA.  This made the data to and from the camera much more stable and mechanically secured the FPGA since the camera was screwed in to the front of the robot.

![camera](https://snag.gy/DPOCFn.jpg)

## Line Following
One of our first tasks was implementing line following using the line sensors provided in lab.  To test the sensors, we started off using the Arduino example code sketch AnalogRead.ino.  This gave us insight on the differences in reading between a black surface and white surface.  With this threshold in mind, we looked closely at an effective algorithm.  We only utilized two line sensors due to simplicity and functionality.  

There are three cases with the line following:
* both sensors read no line, meaning that the robot is on track and can keep moving forward
* left sensor reads a line, meaning that the robot needs to adjust by turning left 
* right sensor reads a line, meaning that the robot needs to adjust by turning right 

![line following](https://snag.gy/wWkfDB.jpg)

Furthermore, if both line sensors read a line, it would indicate that the robot had reached an intersection, in which it could perform a new command.
#### Videos
Line Following
<iframe width="560" height="315" src="https://www.youtube.com/embed/o3cih4OgYb0" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

Figure 8
<iframe width="560" height="315" src="https://www.youtube.com/embed/4iOya06wi0Q" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

#### Code
~~~
 leftOutValue = map(analogRead(leftOut), 0, 1023, 0, 255);
    rightOutValue = map(analogRead(rightOut), 0, 1023, 0, 255);

    if (leftOutValue < thresh && rightOutValue < thresh) {
      break;
    } else if (rightOutValue < thresh) {
      left.write(100);
      right.write(90);
    } else if (leftOutValue < thresh) {
      left.write(90);
      right.write(80);
    } else {
      forward();
    }
~~~

## Wall Sensing
The next addition we made to our robot was attaching wall sensors.  To start, we once again utilized the analogRead sketch to determine thresholds.  For our purposes in detecting a wall about 4 inches away on a short range IR sensor, the threshold value of around 50 worked well.  Once we could sense the presence of a wall, we first tried to implement right hand wall following.

#### Right Hand Wall Following
To first understand Right Hand Wall Following (RHWF), we watched some YouTube videos to see it in action. Those can be found here and here. Next, we moved our robot into place and reasoned out the logic, which can be represented with the pseudocode below:

~~~
if (no wall on Right){
   turn right 90 degrees
   move forward one square
}else if (no wall ahead){
   move forward one square
}else{
   turn left 90 degrees
}
~~~

Following these steps, the robot can get out of any maze eventually by traversing the wall in a priority of turn right, go striaght, turn left.

To debug this algorithm, we first used Serial.println to see what logic the robot would perform next. Using this and manually placing our robot at various points in the maze, we were able to confirm that our algorithm worked.

<iframe width="560" height="315" src="https://www.youtube.com/embed/6lR5Z0Hitc8" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

#### DFS
After getting wall following and recognition working, we then implemented a depth first search for traversing a larger and more complex maze.  We chose to start with depth first search (DFS) because it was the easiest to grasp and implement and we could build on it to make a more efficient algorithm moving forward. To understand depth first search, we first drew out several diagrams to show what the robot would think. DFS first explores one path fully from the root node, noting any alternative paths along the way. When it hits a dead end, the robot backtracks back to an intersecion with a path branch in it and checks to see if any of the alternative paths are unexplored. If so, it then seaerches the entire depth of that branch repeating the process again. The robot continues this loop until there are no more alternative branches to explore.

In graph format, we can follow the above rules and execute DFS such as the following: 
![DFS](https://snag.gy/qMwF4m.jpg)
However in our tile maze format, it is a little bit more complex as backtracking and remembering your previous path complicates things. To remember our path, we have a stack that pushes every square that we travel through. When we backtrack, locations are popped off of the stack. In addition, we also have to store branches as we travel. Branches are defined by squares that have multiple different directions that the robot can move in. These branches are also stored in a stack such that we can go back to the most recent branch by simply popping it and comparing it with the popped item from the travel stack.

To operate, the robot first moves as far as it can with priority being straight (least amount of time), turn right, turn left, respectively until it encounters a dead end. When it finds a dead end, it then moves back to the last branch. If the branch has tiles that can be explored adjacent to it, the robot searches to the depth of that branch and then comes back. If there is no feasible paths for the robot to explore, it then pops the next branch off of the branch stack and travels back again. This happens until there are no more items in the branch stack at which there is no more squares in the maze to feasibly explore. Lastly, we also have an array called visited which has length 81 (9x9) such that each location has an explored or unexplored status (1 = visited, 0 = unvisited). This will track the robot’s progress and will serve as the robot’s comparator for minimizing driving over explored squares.

An example of this described navigation is shown in the video we created below:
<iframe width="560" height="315" src="https://www.youtube.com/embed/JFkDh5BSens" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

The first thing we added to the robot was functions to check the front, left, and right side of the robot. The three functions simply return a boolean value: true if there is no wall in respective location and if respective location is unvisited, false otherwise. These three functions can be used in going to the deepest depth of a path and for deciding if a branch can be further explored. The checkFront function is implemented as follows (others are similar):
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
~~~

Next, we implemented a function that fully explores one path on the maze until it hits a dead end. The logic is pretty simple. The first priority is to go straight (mainly because it takes the least time) and as long as checkFront returns true, the robot will continue straight. If this is not an option, the robot will then check for right and left, respectively. While it is moving, it also marks down each node that is a branch by checking if there are other paths to take. If it is a branch, it pushes the location into the branch stack and continues. Additionally as it travels, it also marks each new location down as explored by setting the respective index in the visited array to a 1. This happens until it detects that it cannot move forward, right, or left anymore indicating a dead end and breaking out of the function.

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
Backtracking was also tricky to implement. The function pops the location from the branches stack and then moves into a while loop comparing the robots current location with the branch it is trying to move to. During this loop the robot popos the previous location from the location stack and compares it to its current location. Based on our system in which locations are numbered in a looping manner, if a square is 9 less than your current location, it is north of you; if a square is 9 more than your current location, it is south of you; if a square is 1 less than your location, it is west of you; if a square is 1 more than your location, it is east of you. With this in mind, we implemented several case statements in which the robot would find out which direction to move in and then turn to face that direction baed on its current compass reading (direction its front is facing). It would then move to the new location and once again check if the location is equal to the branch location; if not, it pops the next location from the location stack and repeats. Our code is outlined below.

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
With our subfunctions, our loop was actually pretty simple. We ran depth first to start the robot off. The remainder of the loop simply runs our backtracking function and then checks for unexplored paths on branches in a while loop until there are no more branches to explore. When the robot finishes, it turn on an LED and stops moving.

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
  digitalWrite(2, HIGH);//light up an LED, finished exploring
  //clear memory, turn around, and restart
}
~~~

<iframe width="560" height="315" src="https://www.youtube.com/embed/mRZY59_xBxk" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>


## Completing FPGA Color and Shape Detection
<iframe width="560" height="315" src="https://www.youtube.com/embed/R9hgISPDOK4" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

## Final Problems
#### Memory Issues

#### Fixing the Radio

#### Fixing the GUI


## Testing
Here in our final days before the competition, we see the robot starting on a 660 Hz sound, beginning to navigate the maze, and updating the GUI correctly.

<iframe width="560" height="315" src="https://www.youtube.com/embed/IKgoeByTBfw" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

## Competition Day

## Results and Conclusion
