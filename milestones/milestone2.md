# Milestone 2

## Objectives
* successfully circle an arbitrary set of walls (e.g. through right hand wall following)
* detect other robots on the field
* combine line following, robot detection, and wall detection - show what the robot is thinking with LEDs

## Implementation

### Hardware Additions

![New hardware additions](https://snag.gy/wW9XNH.jpg)
To enable wall following, we needed to incorporate two short range IR sensors to detect distance to walls.  These sensors were placed at the front of the robot and on the right side of the robot to perform right hand wall following.  

We did some initial testing using the analogInOutSerial code and collected the following data:
* When a wall was close, the analog out reading was around 110
* When a wall was not close, the analog out reading was around 30

From these values, we chose a cutoff detection threshold at 50 moving forward.

In addition, we also mounted our optical circuitry so that our robot can detect other robots.  For now it is sitting on other breadboards and secured with a piece of duct tape.  In the future, we might make a PCB and some 3D printed bracketing to secure the piece.

### Right Hand Wall Following Algorithm

To first understand Right Hand Wall Following (RHWF), we watched some YouTube videos to see it in action.  Those can be found here and [here](https://www.youtube.com/watch?v=U4N7bvGnByQ).  Next, we moved our robot into place and reasoned out the logic, which can be represented with the pseudocode below:

```
if (no wall on Right){
   turn right 90 degrees
   move forward one square
}else if (no wall ahead){
   move forward one square
}else{
   turn left 90 degrees
}
```

Following these steps, the robot can get out of any maze eventually by traversing the wall in a priority of turn right, go striaght, turn left.   

To debug this algorithm, we first used Serial.println to see what logic the robot would perform next.  Using this and manually placing our robot at various points in the maze, we were able to confirm that our algorithm worked. 

## Adding Line Following

After developing the above algorithm, the next problem we needed to solve was to have our robot traverse between different intersections of lines, or what we called "nodes".  To travel between each node, we utilized our line sensors and line following from Lab 1.  We first made subfunctions for moving forward one node, turning right, and turning left.  

* In the move forward command, called coast(), we run our line following command which adjusts the robot based on the line readings.  However, we added a break statement that triggers when both sensors read white, indicating that the robot has arrived at a node.  Then the robot moves forward a small amount such that the center of the robot is right above the node so that both wheels can be used for turning for quicker movement.

* In the turnLeft() command, the robot spins left until the right sensor reads white.

* In the turnRight() command, the robot spins right until the left sensor reads white.

Combining these subfunctions, with the above algorithm, we were able to have our robot successfully navigte the maze.  Our code looks like the following:

~~~
 frontWallValue = map(analogRead(frontWall), 0, 1023, 0, 255); 
  rightWallValue = map(analogRead(rightWall), 0, 1023, 0, 255); 
  if (rightWallValue < 50){ //no right wall
    turnRight();
    coast();
  } else if (frontWallValue < 50){ //no front wall
    coast();
  }else{
    turnLeft();
  }
~~~

<iframe width="560" height="315" src="https://www.youtube.com/embed/6lR5Z0Hitc8" frameborder="0" allow="autoplay; encrypted-media" allowfullscreen></iframe>

## Detecting Other Robots

We recycled our optical hardware and software from lab 2 in implementing detection of other robots.  We mounted our hardware and adjusted the pins accordingly.  Then, we added a simple if statement stating that if a robot is detected, the robot will wait and turn on a yellow LED on board.  Otherwise, it will follow the maze normally.  We tested with the full implementation and it worked well!  

One thing we plant to change in the future is putting the robot detection before the coast() command so that a robot will first scan the node it plans to move to before moving.  Another barrier we had to overcome during working on the milestone was hardware issues with the motor and battery.  We replaced our left stepper motor and plan to remake a USB connector to the battery pack.

After we had our robot working, we tried several other maze configurations and our robot was able to successfully navigate them all!

### Video
<iframe width="560" height="315" src="https://www.youtube.com/embed/2NvHqhuAE28" frameborder="0" allow="autoplay; encrypted-media" allowfullscreen></iframe>

### Code
~~~
#define LOG_OUT 1 // use the log output function
#define FFT_N 256 // set to 256 point fft

#include <FFT.h> // include the library

int robot = 0;   //another robot detected
int start = 0;   //robot ready to go

#include <Servo.h>
Servo left;
Servo right; 

const int leftOut = A1;
const int rightOut = A0;
const int frontWall = A3;
const int rightWall = A2; 
int leftOutValue=0;
int rightOutValue=0;
int frontWallValue=0;
int rightWallValue=0;
int counter = 0;

void setup() {
  Serial.begin(115200); // use the serial port
  pinMode(13, INPUT);
  pinMode(3, OUTPUT);
  pinMode(12, OUTPUT);
  left.attach(6);
  right.attach(5);  
}

void forward() {
  left.write(180);
  right.write(0);
  
}

void still() {
  left.write(90);
  right.write(90);
}

void turnRight(){
  left.write(100);
  right.write(100);
  delay(300);
  while(leftOutValue > 170){
    left.write(100);
    right.write(100);
    leftOutValue = map(analogRead(leftOut), 0, 1023, 0, 255);
    rightOutValue = map(analogRead(rightOut), 0, 1023, 0, 255);
  }
  left.write(90);
  right.write(90);
}

void turnLeft(){

  left.write(80);
  right.write(80);
  delay(300);
  while(rightOutValue > 170) {
    left.write(80);
    right.write(80);
    leftOutValue = map(analogRead(leftOut), 0, 1023, 0, 255);
    rightOutValue = map(analogRead(rightOut), 0, 1023, 0, 255);
  }
  left.write(90);
  right.write(90);
}

void coast(){
  while (1){
    leftOutValue = map(analogRead(leftOut), 0, 1023, 0, 255);
    rightOutValue = map(analogRead(rightOut), 0, 1023, 0, 255);
    if (leftOutValue < 170 && rightOutValue < 170){
      still();
      break;
    }else if (rightOutValue < 170 ){
      left.write(100);
      right.write(90);
    }else if(leftOutValue < 170){
      left.write(90);
      right.write(80);
    }else{
      forward();
    }
  }
  forward();
  delay(500);
}

void loop() {

    optical();
    
    if (robot ==1){ //another robot detected
      digitalWrite(12, HIGH);
      still();
      delay(500); 
       
    } else {
      digitalWrite(12, LOW);
      frontWallValue = map(analogRead(frontWall), 0, 1023, 0, 255); 
      rightWallValue = map(analogRead(rightWall), 0, 1023, 0, 255); 
      if (rightWallValue < 50){ //no right wall
        turnRight();
        coast();
      } else if (frontWallValue < 50){ //no front wall
        coast();
      }else{
        turnLeft();
      }  
    }    
}

void optical(){
  cli();
    for (int i = 0 ; i < 512 ; i += 2) { //read from IR
      fft_input[i] = analogRead(A4);  // use analogRead to lower sampling frequency
      fft_input[i+1] = 0;
    }
    fft_window();
    fft_reorder();
    fft_run();
    fft_mag_log();
    sei();

    if (fft_log_out[154 ] > 20){ //threshold on IR sesnsor
      robot = 1;  
    } else {
      robot = 0;  
    }
    Serial.println(fft_log_out[154]);
}

~~~

