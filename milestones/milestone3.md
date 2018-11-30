# Milestone 3: Maze Exploration

### Objectives
* Robot maze exploration using search algorithm (DFS, BFS, Dijkstra, etc.)
* Navigate a maze of size 4x5
* Update the maze on the GUI

## Implementation

### Chassis Upgrade
We created a new chassis to house all of our electronics using a lasercut design.  The wheels and sensors are housed inside and the shell prevents the wiring from getting jostled out of place.  Additionally, we budgeted space for all of the sensors, the camera, and the FPGA.
![New Chassis](https://snag.gy/YD89Ih.jpg)

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

### Videos
<iframe width="560" height="315" src="https://www.youtube.com/embed/mRZY59_xBxk" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

This video is of our prototype for our chassis in acrylic.  The maze is still solved though.
<iframe width="560" height="315" src="https://www.youtube.com/embed/Doj1n4cUl8s" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

This video includes our GUI in it.  Unfortunately the new version of the GUI does not work on our computers so we had to use the old version which still has trouble updating consistently.  Regardless, our robot was able to correctly communicate the maze to the compuater.  
<iframe width="560" height="315" src="https://www.youtube.com/embed/NCDlGhC8zqY" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

### Full Code

~~~

#include <StackArray.h>
#include <Servo.h>
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

Servo left;
Servo right;
const int leftOut = A4;
const int rightOut = A5;
const int frontWall = A3;
const int rightWall = A2;
const int leftWall = A1;
int leftOutValue = 0;
int rightOutValue = 0;
int frontWallValue = 0;
int rightWallValue = 0;
int leftWallValue = 0;
int counter = 0;
int start = 2;
int compass = 0; //north = 0, east = 1, south = 2, west = 3
int north, south, east, west;
int counter1 = 0;
int branch = 0;
int prev = 0;
int numPaths;
int next;

//StackArray <int> stack;
StackArray <int> branches;
int visited[81];

int a, b, c;

RF24 radio(9, 10);

const uint64_t pipes[2] = { 0x000000002ELL, 0x000000002FLL };

typedef enum { role_ping_out = 1, role_pong_back } role_e;
// The debug-friendly names of those roles
const char* role_friendly_name[] = { "invalid", "Ping out", "Pong back"};
// The role of the current running sketch
role_e role = role_ping_out;

word loc = 80; //0001001 INITIAL loc
word explored = 0;
word treasure = 0; // 101 blue tirangle
word robot = 0; //no robot
word walls = 0; //north, east, south, west

void radioSetup()
{
  printf_begin();
  radio.begin();
  // optionally, increase the delay between retries & # of retries
  radio.setRetries(15, 15);
  radio.setAutoAck(true);
  // set the channel
  radio.setChannel(0x50);
  // set the power
  // RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_MED=-6dBM, and RF24_PA_HIGH=0dBm.
  radio.setPALevel(RF24_PA_HIGH);
  //RF24_250KBPS for 250kbs, RF24_1MBPS for 1Mbps, or RF24_2MBPS for 2Mbps
  radio.setDataRate(RF24_250KBPS);

  radio.setPayloadSize(8);

  //  if ( role == role_ping_out )
  //  {
  //    radio.openWritingPipe(pipes[0]);
  //    radio.openReadingPipe(1,pipes[1]);
  //  }
  //  else
  //  {
  //    radio.openWritingPipe(pipes[1]);
  //    radio.openReadingPipe(1,pipes[0]);
  //  }
  radio.startListening();

  radio.printDetails();
  //role = role_ping_out;
  radio.openWritingPipe(pipes[0]);
  radio.openReadingPipe(1, pipes[1]);
}

void transmit()
{
  loc = loc % 81;
  explored = 1;
  treasure = 0;
  robot = 0;
  walls = 0;

  frontWallValue = map(analogRead(frontWall), 0, 1023, 0, 255);
  rightWallValue = map(analogRead(rightWall), 0, 1023, 0, 255);
  leftWallValue = map(analogRead(leftWall), 0, 1023, 0, 255);

  switch (compass) {
    case 0: //facing north
      if (leftWallValue > 40) walls = 1 ; //west
      if (frontWallValue > 40) walls = walls | 1 << 3; //north
      if (rightWallValue > 40) walls = walls | 1 << 2; //east
      //      Serial.println("north");
      break;
    case 1: //facing east
      if (leftWallValue > 40) walls = 1 << 3 ; //north
      if (frontWallValue > 40) walls = walls | 1 << 2; //east
      if (rightWallValue > 40) walls = walls | 1 << 1; //south
      //      Serial.println("east");
      break;
    case 2: //facing south
      if (leftWallValue > 40) walls = 1 << 2 ; //east
      if (frontWallValue > 40) walls = walls | 1 << 1; //south
      if (rightWallValue > 40) walls = walls | 1; //west
      //      Serial.println("south");
      break;
    case 3: //facing west
      if (leftWallValue > 40) walls = 1 << 1 ; //south
      if (frontWallValue > 40) walls = walls | 1; //west
      if (rightWallValue > 40) walls = walls | 1 << 3; //north
      //      Serial.println("west");
      break;
  }

  if (role == role_ping_out) {
    radio.stopListening();
    printf("Now sending ...");

    word package = explored << 15 | walls << 11 | treasure << 8 | robot << 7 | loc;

    bool ok = radio.write(&package, sizeof(package) );
    if (ok)
      printf("ok...");
    else
      printf("failed.\n\r");

    // Now, continue listening
    radio.startListening();
    // Wait here until we get a response, or timeout (250ms)
    unsigned long started_waiting_at = millis();
    bool timeout = false;
    while ( ! radio.available() && ! timeout )

      if (millis() - started_waiting_at > 500 )
        timeout = true;
    // Describe the results
    if ( timeout )
    {
      printf("Failed, response timed out.\n\r");
    }
    else
    {
      // Grab the response, compare, and send to debugging spew
      int confirm;
      if (radio.read( &confirm, sizeof(int) ))
      { printf("Got confirming response:");
        Serial.println(confirm);
      }

    }
    //delay(2000);
  }
}

void forward() {
  left.write(180);
  right.write(0);
  delay(10);

}

void still() {
  left.write(90);
  right.write(90);
}

void turnRight() {
  compass = (compass + 1);
  if (compass == 4) compass = 0;
  left.write(100);
  right.write(100);
  delay(475);
  left.write(90);
  right.write(90);
  delay(100);
  transmit();

}

void turnLeft() {
  compass = ((compass - 1) % 4);
  if (compass == -1) compass = 3;
  left.write(80);
  right.write(80);
  delay(475);
  left.write(90);
  right.write(90);
  delay(100);
  transmit();
}

void coast() {
  switch (compass) {
    case (0): //north
      loc = loc - 9;
      break;
    case (1): //east
      loc = loc + 1;
      break;
    case (2): //south
      loc = loc + 9;
      break;
    case (3): //west
      loc = loc - 1;
      break;
  }

  while (1) {
    leftOutValue = map(analogRead(leftOut), 0, 1023, 0, 255);
    rightOutValue = map(analogRead(rightOut), 0, 1023, 0, 255);

    if (leftOutValue < 220 && rightOutValue < 220) {
      still();
      break;
    } else if (rightOutValue < 220) {
      left.write(100);
      right.write(90);
    } else if (leftOutValue < 220) {
      left.write(90);
      right.write(80);
    } else {
      forward();
    }
  }
  left.write(180);
  right.write(0);
  delay(250);
  still();
  delay(100);
  visited[loc] = 1;
  Serial.print("loc = ");
  Serial.print(loc);
  Serial.print("\t prev = ");
  Serial.print(prev);
  Serial.print("\t branch = ");
  Serial.print(branch);
  //Serial.print("\t stack = ");
  //Serial.println(stack);
  //transmit();
}

//returns false if front tile has wall or has been visited
bool checkFront() {
  frontWallValue = map(analogRead(frontWall), 0, 1023, 0, 255);
  rightWallValue = map(analogRead(rightWall), 0, 1023, 0, 255);
  leftWallValue = map(analogRead(leftWall), 0, 1023, 0, 255);
  if (frontWallValue > 50) return false; // wall
  if (compass == 0 && visited[loc - 9] == 1) return false; //check north if facing north etc. etc. etc.
  if (compass == 1 && visited[loc + 1] == 1) return false;
  if (compass == 2 && visited[loc + 9] == 1) return false;
  if (compass == 3 && visited[loc - 1] == 1) return false;

  return true;
}

//returns false if left tile has wall or has been visited
bool checkLeft() {
  frontWallValue = map(analogRead(frontWall), 0, 1023, 0, 255);
  rightWallValue = map(analogRead(rightWall), 0, 1023, 0, 255);
  leftWallValue = map(analogRead(leftWall), 0, 1023, 0, 255);
  if (leftWallValue > 50) return false; // wall
  if (compass == 0 && visited[loc - 1] == 1) return false; //check west if facing north etc. etc. etc.
  if (compass == 1 && visited[loc - 9] == 1) return false;
  if (compass == 2 && visited[loc + 1] == 1) return false;
  if (compass == 3 && visited[loc + 9] == 1) return false;
  return true;
}

//returns false if right tile has wall or has been visited
bool checkRight() {
  frontWallValue = map(analogRead(frontWall), 0, 1023, 0, 255);
  rightWallValue = map(analogRead(rightWall), 0, 1023, 0, 255);
  leftWallValue = map(analogRead(leftWall), 0, 1023, 0, 255);
  if (rightWallValue > 50) return false; // wall
  if (compass == 0 && visited[loc + 1] == 1) return false; // check east if facing north etc. etc. etc.
  if (compass == 1 && visited[loc + 9] == 1) return false;
  if (compass == 2 && visited[loc - 1] == 1) return false;
  if (compass == 3 && visited[loc - 9] == 1) return false;
  return true;
}

void checkBranch() {
  numPaths = 0;

  if (checkFront() == true) numPaths++;
  if (checkLeft() == true) numPaths++;
  if (checkRight() == true) numPaths++;
  still();
  if (numPaths > 1) {
    branches.push(loc);
    //    digitalWrite(2, HIGH);
    //    delay(500);
    //    digitalWrite(2, LOW);
  }

  numPaths = 0;
}

void depth() {
  checkBranch();
  coast();
  while (1) {
    frontWallValue = map(analogRead(frontWall), 0, 1023, 0, 255);
    rightWallValue = map(analogRead(rightWall), 0, 1023, 0, 255);
    leftWallValue = map(analogRead(leftWall), 0, 1023, 0, 255);

    checkBranch();

    if (checkFront() == true) { //no front wall
      coast();
    } else if (checkRight() == true) { //no right wall, but has front wall
      turnRight();
    } else if (checkLeft() == true) { //no left wall, has front and right wall
      turnLeft();
    } else {
      turnLeft();
      turnLeft();
      coast();
      break;
    }
  }
}


//moves back to previous branch
void prevBranch() {
  next = branches.pop(); //trying to get to this location

  while (1) {
    frontWallValue = map(analogRead(frontWall), 0, 1023, 0, 255);
    rightWallValue = map(analogRead(rightWall), 0, 1023, 0, 255);
    leftWallValue = map(analogRead(leftWall), 0, 1023, 0, 255);

    if (loc == next) {
      break;
    }

    if (frontWallValue < 50) { //no front wall
      coast();
    } else if (leftWallValue < 50) { //no left wall, but has front wall
      turnLeft();
      coast();
    } else if (rightWallValue < 50) { //no right wall, but has front and left wall
      turnRight();
      coast();
    } else { //shouldnt really get here :(
      break;
    }
  }
}
void setup() {
  pinMode(2, OUTPUT);//LED
  pinMode(6, OUTPUT);//LED
  pinMode(7, OUTPUT);
  pinMode(4, OUTPUT);

  radioSetup();

  left.attach(3);
  right.attach(5);
  Serial.begin(57600);
  still();
  transmit();
  visited[loc] = 1;
  //    digitalWrite(2, HIGH); //LED on

}

void loop() {
  depth();  // go to the depth of a path

  while (branches.isEmpty() == false) {

    prevBranch(); // go to last visited node with another option

    if (checkFront()) {
      depth();
    } else if (checkRight()) {
      turnRight();
      depth();
    } else if (checkLeft()) {
      turnLeft();
    } else { //no options at branch
      delay(100);
    }

  }
  digitalWrite(2, HIGH);

  still();
  while (1); //finished exploring so stop
}


~~~



