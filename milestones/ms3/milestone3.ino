

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

StackArray <int> stack;
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

word loc = 40; //0001001 INITIAL loc
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
  Serial.println("in the set up");
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
      Serial.println("north");
      break;
    case 1: //facing east
      if (leftWallValue > 40) walls = 1 << 3 ; //north
      if (frontWallValue > 40) walls = walls | 1 << 2; //east
      if (rightWallValue > 40) walls = walls | 1 << 1; //south
      Serial.println("east");
      break;
    case 2: //facing south
      if (leftWallValue > 40) walls = 1 << 2 ; //east
      if (frontWallValue > 40) walls = walls | 1 << 1; //south
      if (rightWallValue > 40) walls = walls | 1; //west
      Serial.println("south");
      break;
    case 3: //facing west
      if (leftWallValue > 40) walls = 1 << 1 ; //south
      if (frontWallValue > 40) walls = walls | 1; //west
      if (rightWallValue > 40) walls = walls | 1 << 3; //north
      Serial.println("west");
      break;
  }

  Serial.println(loc);

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
  delay(725);
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
  delay(725);
  left.write(90);
  right.write(90);
  delay(100);
  transmit();
}

void coast() {
  Serial.println("coast");
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

    if (leftOutValue < 150 && rightOutValue < 150) {
      still();
      break;
    } else if (rightOutValue < 150 ) {
      left.write(100);
      right.write(90);
    } else if (leftOutValue < 150) {
      left.write(90);
      right.write(80);
    } else {
      forward();
    }
  }
  left.write(180);
  right.write(0);
  delay(450);
  still();
  delay(100);
  visited[loc] = 1;
  transmit();
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
  digitalWrite(2, HIGH); //LED on
  still();
  transmit();
  stack.push(loc);

}

//returns false if front tile has wall or has been visited
bool checkFront() {
  if (frontWallValue > 50) return false;
  if (compass == 0 && visited[loc - 9] == 1) return false;
  if (compass == 1 && visited[loc + 1] == 1) return false;
  if (compass == 2 && visited[loc + 9] == 1) return false;
  if (compass == 3 && visited[loc - 1] == 1) return false;
  return true;
}

//returns false if left tile has wall or has been visited
bool checkLeft() {
  if (leftWallValue > 50) return false;
  if (compass == 0 && visited[loc - 1] == 1) return false;
  if (compass == 1 && visited[loc - 9] == 1) return false;
  if (compass == 2 && visited[loc + 1] == 1) return false;
  if (compass == 3 && visited[loc + 9] == 1) return false;
  return true;
}

//returns false if right tile has wall or has been visited
bool checkRight() {
  if (rightWallValue > 50) return false;
  if (compass == 0 && visited[loc + 1] == 1) return false;
  if (compass == 1 && visited[loc + 9] == 1) return false;
  if (compass == 2 && visited[loc - 1] == 1) return false;
  if (compass == 3 && visited[loc - 9] == 1) return false;
  return true;
}


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

void loop() {
  depth();  // go to the depth of a path
  while (branches.isEmpty() == false) {
    prevBranch(); // go to last visited node with another option
    if (checkFront) {
      depth();
    } else if (checkRight) {
      turnRight();
      depth();
    } else if (checkLeft) {
      turnLeft();
    } else { //no options at branch
      delay(10);
    }
  }
  digitalWrite(2, HIGH);//light up an LED
  while(1);
}



