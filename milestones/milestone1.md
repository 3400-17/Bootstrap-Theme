# Milestone 1: Following Lines 

## Objectives
The goal of milestone 1 was to have our robot be able to follow a line and also navigate a figure 8 on the grid.

## Following a Line
### Hardware Modifications
#### Adddition of Line Sensors
<img src="https://snag.gy/Tgkhm4.jpg">

In order to implement line following, the first step would be to add sensors to detect the line.  We used the provided IR line sensors and attached two the the front of our robot.  

We chose to have two sensors at the front of the robot to read either side of the line as the robot traveled.  This way, the sensors could tell immediately if the robot was off course.  When experimenting with the readings of the line sesnsor, we used the InOutSerial example provided by Arduino.  After mapping the reading to a 0 to 255 scale, we found that white was 180 and black was 230.

However, we thought that this difference bewteen black and white was not large enough.  Our solution was to move the sensors closer to the ground.  We were careful to have the sensors as close as possible without interfering with movement in order to have good measurements.  This yielded much better readings with black being 200 and white being 90. 

#### Power Systems Modification

<img src="https://snag.gy/cnLdbg.jpg">

One other problem we had with our robot was the servos.  We were comparing our speed of travel to other teams' robots and ours was running signficantly slower.  We first thought it was a servo issue and we tried other servos, encountering similar results.  Next, we talked to a TA who mentioned that the current limit of the Arduino might be limiting the power to the motors.  To solve this problem, we routed the leads from our battery directly into the breadboard rails rather than into the Arduino.  This way, the Arduino was powered through the rails and the motors and sensors could draw needed current directly from the battery rather than through the Arduino.  This fixed our power problem.

### Algorithm
![line following algorithm](https://snag.gy/wWkfDB.jpg)

The algorithm for line following is not complicated.  There are three cases:

1. The robot is on track.  

Becasue the robot is traveling along the line, both sensors read white (assuming line is black).  The robot continues traveling straight.

2. The robot is veering to the right.

As shown in the diagram above, the left sensor reads black while the right reads white.  The robot corrects itself by stopping the left wheel to turn left until the left sensor no longer reads black.

3. The robot is veering to the left. 

As shown in the diagram above, the right sensor reads black while the left reads white.  The robot corrects itself by stopping the right wheel to turn right until the right sensor no longer reads black.

### Code
The code first initializes the servos and assigns pins to sensors.  In the loop, the sensor readings are first gathered and then the above algorithm is executed every 50 mSec.  

~~~
#include <Servo.h>
Servo left;
Servo right; 

const int leftOut = A0;
const int rightOut = A1;
int leftOutValue=0;
int rightOutValue=0;

void setup() {
  left.attach(10);
  right.attach(11);  
  Serial.begin(9600);
  
}

// the loop function runs over and over again forever
void loop() {
  leftOutValue = map(analogRead(leftOut), 0, 1023, 0, 255);
  rightOutValue = map(analogRead(rightOut), 0, 1023, 0, 255);
  
   //black is high, white is low
  
  if (leftOutValue < 233 ){
    left.write(100);
    right.write(90);
  }else if(rightOutValue < 233){
    left.write(90);
    right.write(80);
  }else{
    forward();
  }
  delay(50);
  
}

void forward() {
  left.write(180);
  right.write(0);
  
}

~~~

### Video
**video**


## Figure 8

### Algorithm
The first step we needed to take in implementing a figure 8 was being able to detect a junction.  This was done by adding a new case to the code above.  When both sensors sense a line beneath them, that means the robot is at a junction.

The next step was determining how to act when the robot reached a junction.  To do this, we used a state algorithm modeled with the diagram below.  If we use a state variable to keep track of where on the 8 the robot currently is, we can know what the next move at a junction needs to be. Looking at the diagram we can see that the robot first needs to turn right three times, then go straight, then turn left three times, then go straight, and repeat.  Each time the robot comes to an intersection, it will do the required action and then increase state by 1.  When state reaches 9, it can then be reset back to 1.  The states are as follows:

1. left
2. left
3. left
4. left
5. right
6. right
7. right
8. right

![Figure 8 States](https://snag.gy/GOX8al.jpg)

Our first attempts with turning were simply using a delay.  If we could have the robot turn 90 degrees exactly while being uninterrupted, then the robot would have "latched on" to a perpendicular line. 

### Code
~~~
#include <Servo.h>
Servo left;
Servo right; 

const int leftOut = A0;
const int rightOut = A1;
int leftOutValue=0;
int rightOutValue=0;
int counter = 0;

void setup() {
  left.attach(10);
  right.attach(11);  
  Serial.begin(9600);
  
}

// the loop function runs over and over again forever
void loop() {
  leftOutValue = map(analogRead(leftOut), 0, 1023, 0, 255);
  rightOutValue = map(analogRead(rightOut), 0, 1023, 0, 255);
   //black is high, white is low
   
  if (leftOutValue < 190 && rightOutValue < 190){ //when encounter cross
    if (counter < 4){ //the first four stages turn left
      left.write(100);
      right.write(90);
      delay(1460);
      counter = counter + 1;
    }else{ //the next four stages turn right
      left.write(90);
      right.write(80);
      delay(1460); // a random number Natan came up with for delay
      counter = counter + 1;
      if (counter == 8){
        counter = 0; 
        //after eight stages, the robot is resetted to the first stage
      }
    }  
  }
  if (leftOutValue < 190 ){ //only left sensor senses the line, robot heads to the right
    left.write(100);
    right.write(90);
  }else if(rightOutValue < 190){ //only right sensor senses the line, robot heads to the left
    left.write(90);
    right.write(80);
  }else{
    forward();
  }
  delay(10);
  
}

void forward() {
  left.write(180);
  right.write(0);
  
}

void back() {
  right.write(95);
  left.write(85);
  
}

void turn() {
  right.write(95);
  left.write(95);
}
~~~
### Video
<iframe width="560" height="315" src="https://www.youtube.com/embed/4iOya06wi0Q" frameborder="0" allow="autoplay; encrypted-media" allowfullscreen></iframe>
