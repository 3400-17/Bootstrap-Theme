# Lab 3: System Integration and Radio Communication
## Teams

**Team 1**: (radio) Zoe, Siming, and Marcela

**Team 2**: (robot) Michael and Natan

## Objectives

* Integrate components from previous labs
* Send maze information to base station
* Navigate maze autonomously

# Radio Team

Zoe, Marcela, and Siming 

### Materials used:
* 2 Nordic nRF24L01, Transceivers
* 2 Arduino Unos
* 2 radio breakout boards with headers

### Goals
The goals for the radio team were to allow the robot to transmit maze information to the base station and display all the information by GUI. 
* send wireless information over the radio
* simulates a robot moving through a maze on the extra Arduino
* design a protocol for data sent between the robot and the base station, and try to send the virutal robot updates via this link
* ensure the base station correspond with the GUI

### Data Structure for Sending/Receiving Maze Information

Since the memory space on the Arduino is limited, we need to make the data as memory effcient as possible. The data structure is the following:

* explored: 1 bit, if the block is explored
* walls: 4 bits, if the robots has walls around it
* treasure: 3 bits, have 2 different colors, and four different shapes
* robots: 1 bit, if there another robot in the neighborhood
* location: 7 bits, representing the location in the maze

![Maze Information](https://snag.gy/EgoWxF.jpg)

![Data Structure](https://snag.gy/tlwBLu.jpg)

### Hardware

We first plugged the radio into the Arduinos using the special PCBs. The radio is wired into the 3.3V pin on the Arduino, or through a 3.3V supply if the Arduino is a knockoff. We do the same circuit set up for both Arduinos, as one of them will simulate the robot, and the other  simulate the base station. 

![Radio circuit](https://snag.gy/Rs1Zdq.jpg)

### Radio Testing 

Following the instructions with the lab, we downloaded the RF24 library for the radios and changed our pipes to 2E and 2F (46 and 47 in decimal).  We set one of the Arduinos to transmit by typing "T" into the serial monitor and successfully sent a package to the receiving Arduino.  In the case of a successful transmission, the receiver would then confirm the message was received. 

After sending a successful package over, we then installed the materials for the GUI, found [here](https://github.com/backhous/ece3400-maze-gui).  To summarize, the GUI operates by taking messages from the serial monitor of the Arduino IDE in the form of x_pos, y_pos, walls where walls refers to whether a wall exists or not (ie. north=true or west=false).  In later labs, we can also tell the GUI whether a treasure is there or not.  

To get familiar with the GUI, we first sent direct GUI input using Serial.println and seeing how the GUI would react. Using the example 2x3 code provided in the repository, we then expanded the code to a 9x9 map and corrected the code to take in our particular data structure and process it into a Serial print statement to be used by the GUI.  The reciever code can be found below.

### Wireless Transmission with Data Structure

Finally, we implemented our data structure on the transmission Arduino.  This was done by bitmasking the package with the command 

~~~
    word package=explored<<15|walls<<11|treasure<<8|robot<<7|loc;
~~~

To test, we incremented the loc variable and the walls variable such that the robot would move in the GUI and display different wall configurations.  This worked well and the video is shown below:
                
<iframe width="560" height="315" src="https://www.youtube.com/embed/QJ-WtfzIMRE" frameborder="0" allow="autoplay; encrypted-media" allowfullscreen></iframe>

### Code for Sending

~~~
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

const uint64_t pipes[2] = { 0x000000002ELL, 0x000000002FLL };

typedef enum { role_ping_out = 1, role_pong_back } role_e;
const char* role_friendly_name[] = { "invalid", "Ping out", "Pong back"};
role_e role = role_pong_back;
    word loc=9; //0001001
    word explored=0;
    word treasure=5;// 101 blue tirangle
    word robot=0; //no robot 
    word walls=0;//1100 north &south
void setup(void)
{
  Serial.begin(57600);
  printf_begin();
  radio.begin();
  radio.setRetries(15,15);
  radio.setAutoAck(true);
  radio.setChannel(0x50);
  radio.setPALevel(RF24_PA_HIGH);
  radio.setDataRate(RF24_250KBPS);

  radio.setPayloadSize(8);

  if ( role == role_ping_out )
  {
    radio.openWritingPipe(pipes[0]);
    radio.openReadingPipe(1,pipes[1]);
  }
  else
  {
    radio.openWritingPipe(pipes[1]);
    radio.openReadingPipe(1,pipes[0]);
  }
  radio.startListening();

  radio.printDetails();
}
void loop(void)
{

  if (role == role_ping_out)
  {

    radio.stopListening();
    printf("Now sending ...");
   
    walls=(walls+1)%80;
    loc=loc+1;
  
   
    word package=explored<<15|walls<<11|treasure<<8|robot<<7|loc;
    
    bool ok = radio.write(&package,sizeof(package) );
    if (ok)
      printf("ok...");
    else
      printf("failed.\n\r");
    
    radio.startListening();
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
      if(radio.read( &confirm, sizeof(int) ))
      {printf("Got confirming response:");
       Serial.println(confirm);
      }
      
    }
    delay(2000);
    }

  if ( Serial.available() )
  {
    char c = toupper(Serial.read());
    if ( c == 'T' && role == role_pong_back )
    {
      printf("*** CHANGING TO TRANSMIT ROLE -- PRESS 'R' TO SWITCH BACK\n\r");

      // Become the primary transmitter (ping out)
      role = role_ping_out;
      radio.openWritingPipe(pipes[0]);
      radio.openReadingPipe(1,pipes[1]);
    }
    
  }
}
~~~

### Code for Receiving
~~~
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

RF24 radio(9,10);
const uint64_t pipes[2] = { 0x000000002ELL, 0x000000002FLL }; 
typedef enum { role_ping_out = 1, role_pong_back } role_e;
const char* role_friendly_name[] = { "invalid", "Ping out", "Pong back"};
role_e role = role_pong_back;
int walls=0;
void setup(void){
  Serial.begin(57600);
  while (!Serial)
  printf_begin();
  radio.begin();

  radio.setRetries(15,15);
  radio.setAutoAck(true);
  radio.setChannel(0x50);
  radio.setPALevel(RF24_PA_HIGH);
  radio.setDataRate(RF24_250KBPS);
  radio.setPayloadSize(8);
  if ( role == role_ping_out )
  {
    radio.openWritingPipe(pipes[0]);
    radio.openReadingPipe(1,pipes[1]);
  }
  else
  {
    radio.openWritingPipe(pipes[1]);
    radio.openReadingPipe(1,pipes[0]);
  }
  radio.startListening();
}

void loop(void)
{

  if ( role == role_pong_back )
  {
 
    if ( radio.available() )
    {
      // Dump the payloads until we've gotten everything
      word got_update;
      bool done = false;
      while (!done)
      {
        // Fetch the payload, and see if this was the last one.
        done = radio.read( &got_update, sizeof(word) );
        word explored=got_update>>15;
        word walls=got_update>>11&15; //  north,east,south,west
        word treasure=got_update>>8&8; //  111;
        word robot=got_update>>7&1;
        word loc=got_update&127; //127 is 1111111
        
        word y=loc%9;
        int x=(int)(loc/9);

        
        char c=',';
        Serial.print(x);
        Serial.print(c);
        Serial.print(y);
        //Serial.print(c);
        //north=0;east=1;south=2;west=3

        
        if ((walls&8)>>3){Serial.print(",north=true");}
        //else {Serial.print("north=false,");}
        if ((walls&4)>>2){Serial.print(",east=true");}
        //else {Serial.print("east=false,");}
        if ((walls&2)>>1){Serial.print(",south=true");}
        //else {Serial.print("south=false,");}
        if (walls&1){Serial.print(",west=true");}
        //else {Serial.print("west=false");}
        Serial.print("\n");
      }
      delay(20);
      radio.stopListening();

      int confirm=6;
      if (radio.write( &confirm, sizeof(confirm) )){
         }
      radio.startListening();
    }
  }

  if ( Serial.available() )
  {
    char c = toupper(Serial.read());
    if ( c == 'T' && role == role_pong_back )
    {
      printf("*** CHANGING TO TRANSMIT ROLE -- PRESS 'R' TO SWITCH BACK\n\r");
      role = role_ping_out;
      radio.openWritingPipe(pipes[0]);
      radio.openReadingPipe(1,pipes[1]);
    }
    else if ( c == 'R' && role == role_ping_out )
    {
      printf("*** CHANGING TO RECEIVE ROLE -- PRESS 'T' TO SWITCH BACK\n\r");

      // Become the primary receiver (pong back)
      role = role_pong_back;
      radio.openWritingPipe(pipes[1]);
      radio.openReadingPipe(1,pipes[0]);
    }
  }
}
~~~                  

# Robot Team
### Materials Used:
* Previous Robot
* Audio module
* Optical module
* Protoboard and circuit components

### Goals
* Integrate audio, optical, wall following, radio, line following on a single system

### Hardware:
First, we added an extra wall sensor such that the robot could be able to see three walls as it traveled throughout the maze. This would make mapping the walls of the maze more feasible.  We chose three instead of four becasue the wall behind the robot will always be false as that is where the robot is traveling from.  The problem with adding one more additional analog signal was that now we have six signals, 2 for the line sensors, 3 for the wall sensors, 1 for audio, and 1 for optical.  To solve this problem, we utilized a multiplexer to switch between the audio and optical functionality.  This worked well as the audio microphone only needed to be used at the very beginning of the functionality.  After sensing the 660 Hz sound, a signal from a digital pin would switch the multiplexer to disconnect the microphone and reconnect the optical sensor.  

After testing our parts for functionality, we then worked to make our parts more manageable.  We decided to mount our systems on a more compact and robust protoboard. We soldered two 358 op amp ICs and a multiplexer.  The protoboard is shown below:

![Protoboard](https://snag.gy/eZTkPi.jpg)

### Software: 
To integrate the FFT based audio and optical recognition, we had to modify code a bit in order to prevent interference with the timers controlling the servo motors.  To do this, every time we used the FFT library, we saved the timer variables in placeholder variables as so:

~~~
  a = ADCSRA;
  b = ADMUX;
  c = DIDR0;
~~~

After the FFT was complete, the values for ADCRA, ADMUX, and DIDR0 were restored such that the servos could operate well.

Since the audio portion only needs to be run once, we implemented this by putting a while loop in the setup portion of the code.  As for the optical part, we have to check as the robot moves forward.  As a result, the IR sensor is checked every time forward is called such that the robot does not run into another robot.

To integrate the microphone, we put the code in the setup loop to just run once.  When run, the code would wait for the signal sound and then switch the MUX over to the optical sensor in the same analog pin.

These changes enabled our robot to follow walls ane lines, and be able to properly start and sense other robots.

### Video:
<iframe width="560" height="315" src="https://www.youtube.com/embed/Orb9eo7mNDc" frameborder="0" allow="autoplay; encrypted-media" allowfullscreen></iframe>

### Code: 
~~~
#define LOG_OUT 1 // use the log output function
#define FFT_N 256 // set to 256 point fft

#include <FFT.h> // include the library
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
int start = 0;

int a, b, c;

void forward() {
  left.write(180);
  right.write(0);
  delay(10);
  still();
  optical();
  while (robot==1) optical();
}

void still() {
  left.write(90);
  right.write(90);
}

void turnRight() {
  left.write(100);
  right.write(100);
  delay(725);
}

void turnLeft() {
  left.write(80);
  right.write(80);
  delay(725);
}

void coast() {
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
}
void optical() {
  still();
  a = ADCSRA;
  b = ADMUX;
  c = DIDR0;
  ADCSRA = 0xe5; // set the adc to free running mode
  ADMUX = 0x40; // use adc0
  DIDR0 = 0x01; // turn off the digital input for adc0
  for (int i = 0 ; i < 512 ; i += 2) { // save 256 samples
    while (!(ADCSRA & 0x10)); // wait for adc to be ready
    ADCSRA = 0xf5; // restart adc
    byte m = ADCL; // fetch adc data
    byte j = ADCH;
    int k = (j << 8) | m; // form into an int
    k -= 0x0200; // form into a signed int
    k <<= 6; // form into a 16b signed int
    fft_input[i] = k; // put real data into even bins
    fft_input[i + 1] = 0; // set odd bins to 0
  }
  fft_window(); // window the data for better frequency response
  fft_reorder(); // reorder the data before doing the fft
  fft_run(); // process the data in the fft
  fft_mag_log(); // take the output of the fft
  sei();
  if (fft_log_out[82] > 67) {
    digitalWrite(2, HIGH);
    robot = 1;
  } else {
    robot = 0;
    digitalWrite(2, LOW);
  }
  ADCSRA = a; // set the adc to free running mode
  ADMUX = b; // use adc0
  DIDR0 = c; //
}

void audio() {
  still();
  a = ADCSRA;
  b = ADMUX;
  c = DIDR0;
  ADCSRA = 0xe5; // set the adc to free running mode
  ADMUX = 0x40; // use adc0
  DIDR0 = 0x01; // turn off the digital input for adc0
  for (int i = 0 ; i < 512 ; i += 2) { // save 256 samples
    while (!(ADCSRA & 0x10)); // wait for adc to be ready
    ADCSRA = 0xf5; // restart adc
    byte m = ADCL; // fetch adc data
    byte j = ADCH;
    int k = (j << 8) | m; // form into an int
    k -= 0x0200; // form into a signed int
    k <<= 6; // form into a 16b signed int
    fft_input[i] = k; // put real data into even bins
    fft_input[i + 1] = 0; // set odd bins to 0
  }
  fft_window(); // window the data for better frequency response
  fft_reorder(); // reorder the data before doing the fft
  fft_run(); // process the data in the fft
  fft_mag_log(); // take the output of the fft
  sei();
  if (fft_log_out[4] > 160) {
    start = 1;
  }
  ADCSRA = a; // set the adc to free running mode
  ADMUX = b; // use adc0
  DIDR0 = c; //
}

void setup() {
  radioSetup();
  pinMode(2, OUTPUT);//LED
  pinMode(6, OUTPUT);//LED
  pinMode(7, OUTPUT);
  left.attach(3);
  right.attach(5);
  Serial.begin(57600);
  digitalWrite(2, LOW); //LED off
  digitalWrite(7, LOW); //sound MUX
  while(start==0){
    audio();
    delay(10);
  }
  still();
  digitalWrite(7, HIGH); //optical MUX
  digitalWrite(2, HIGH); //LED on
  still();
}

void loop() {
  frontWallValue = map(analogRead(frontWall), 0, 1023, 0, 255);
  rightWallValue = map(analogRead(rightWall), 0, 1023, 0, 255);
  leftWallValue = map(analogRead(leftWall), 0, 1023, 0, 255);
  if (rightWallValue < 50) { //no right wall
    turnRight();
    coast();
  } else if (frontWallValue < 50) { //no front wall
    coast();
  } else {
    turnLeft();
  }
}
~~~

# Integration
After the two teams had completed their parts, we integrated the two systems together. Our robot was able to run through the maze autonomously, starting on 660Hz tone, and updating the GUI on the screen as it runs. The robot stops if there is another robot, but continues to run if there is a decoy.  

![Robot](https://snag.gy/lrKqkB.jpg)

### Programming for GUI transmission
Though setting up the radio was not too difficult, the biggest challenge we had was to be able to keep track of our location and orientation correctly and also coordinating which wall were present and which were not.  To do this, we added some additional variables called compass and location.  Compass gives the direction the robot is currently facing (0-north, 1-east, 2-south, 3-west) and location gives the tile that the robot is currently in according to our data structure (0-80).  Given an initial starting position and orientation, we can always track our robot.  If the robot moves north using the coast command, we subtract 9 from position.  Similarly, traveling south adds 9, west subtracts 1, and east adds 1.  Meanwhile, compass can be turned using the turnRight and turnLeft functions simply by incrementing or decrementing compass and using the modulo function with 4.  

Furthermore, this can be used for wall sensing.  Our code corresponds the wall sensors relative to the compass and then also correctly bitmasks them to fit the datastructure.

~~~
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
  ~~~

### Videos
<iframe width="560" height="315" src="https://www.youtube.com/embed/9kuqzpLLxKA" frameborder="0" allow="autoplay; encrypted-media" allowfullscreen></iframe>
The video does not show the decoy because the lab was so crowded.  We were sharing the power supply with other groups and did not have a power supply channel for the decoy.  However, our hardware can successfully filter out the decoy signals as seen from Lab 2.

** View from GUI **
<iframe width="560" height="315" src="https://www.youtube.com/embed/GSFPWY34c6M" frameborder="0" allow="autoplay; encrypted-media" allowfullscreen></iframe>

Here is the actual maze compared the mapped maze:
![Map!](https://snag.gy/j6FSYB.jpg)

### Code
~~~
#define LOG_OUT 1 // use the log output function
#define FFT_N 256 // set to 256 point fft

#include <FFT.h> // include the library
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
int start = 0;
int robot = 0;
int compass = 3; //north = 0, east = 1, south = 2, west = 3

int a, b, c; //variables for reseting timer values

RF24 radio(9, 10);

const uint64_t pipes[2] = { 0x000000002ELL, 0x000000002FLL };

typedef enum { role_ping_out = 1, role_pong_back } role_e;
const char* role_friendly_name[] = { "invalid", "Ping out", "Pong back"};
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
  radio.setRetries(15, 15);
  radio.setAutoAck(true);
  radio.setChannel(0x50);
  radio.setPALevel(RF24_PA_HIGH);
  radio.setDataRate(RF24_250KBPS);

  radio.setPayloadSize(8);
  radio.startListening();

  radio.printDetails();
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

    radio.startListening();
    unsigned long started_waiting_at = millis();
    bool timeout = false;
    while ( ! radio.available() && ! timeout )

      if (millis() - started_waiting_at > 500 )
        timeout = true;
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
  }
}

void forward() {
  left.write(180);
  right.write(0);
  delay(100);
  still();
}

void still() {
  left.write(90);
  right.write(90);
}

void turnRight() {
  compass = ((compass + 1) % 4);
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
    case(0): //north
      loc = loc - 9;
      break;
    case(1): //east
      loc = loc + 1;
      break;
    case(2): //south
      loc = loc + 9;
      break;
    case(3): //west
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
  transmit();
}
void optical() {
  still();
  a = ADCSRA;
  b = ADMUX;
  c = DIDR0;
  ADCSRA = 0xe5; // set the adc to free running mode
  ADMUX = 0x40; // use adc0
  DIDR0 = 0x01; // turn off the digital input for adc0
  for (int i = 0 ; i < 512 ; i += 2) { // save 256 samples
    while (!(ADCSRA & 0x10)); // wait for adc to be ready
    ADCSRA = 0xf5; // restart adc
    byte m = ADCL; // fetch adc data
    byte j = ADCH;
    int k = (j << 8) | m; // form into an int
    k -= 0x0200; // form into a signed int
    k <<= 6; // form into a 16b signed int
    fft_input[i] = k; // put real data into even bins
    fft_input[i + 1] = 0; // set odd bins to 0
  }
  fft_window(); // window the data for better frequency response
  fft_reorder(); // reorder the data before doing the fft
  fft_run(); // process the data in the fft
  fft_mag_log(); // take the output of the fft
  sei();
  //    Serial.println("start");
  //    for (byte i = 0 ; i < FFT_N/2 ; i++) {
  //      Serial.println(fft_log_out[i]); // send out the data
  //    }

  //    Serial.println(fft_log_out[82]);

  if (fft_log_out[82] > 67) {
    digitalWrite(2, HIGH);
    robot = 1;
    delay(2000);
  } else {
    robot = 0;
    digitalWrite(2, LOW);
  }
  ADCSRA = a; // set the adc to free running mode
  ADMUX = b; // use adc0
  DIDR0 = c; //
}

void audio() {
  still();
  a = ADCSRA;
  b = ADMUX;
  c = DIDR0;
  ADCSRA = 0xe5; // set the adc to free running mode
  ADMUX = 0x41; // use adc0
  DIDR0 = 0x01; // turn off the digital input for adc0
  for (int i = 0 ; i < 512 ; i += 2) { // save 256 samples
    while (!(ADCSRA & 0x10)); // wait for adc to be ready
    ADCSRA = 0xf5; // restart adc
    byte m = ADCL; // fetch adc data
    byte j = ADCH;
    int k = (j << 8) | m; // form into an int
    k -= 0x0200; // form into a signed int
    k <<= 6; // form into a 16b signed int
    fft_input[i] = k; // put real data into even bins
    fft_input[i + 1] = 0; // set odd bins to 0
  }
  fft_window(); // window the data for better frequency response
  fft_reorder(); // reorder the data before doing the fft
  fft_run(); // process the data in the fft
  fft_mag_log(); // take the output of the fft
  sei();
  //    Serial.println("start");
  //    for (byte i = 0 ; i < FFT_N/2 ; i++) {
  //      Serial.println(fft_log_out[i]); // send out the data
  //    }

  //    Serial.println(fft_log_out[4]);

  if (fft_log_out[4] > 160) {
    start = 1;
  }
  ADCSRA = a; // set the adc to free running mode
  ADMUX = b; // use adc0
  DIDR0 = c; //
}

void setup() {
  radioSetup();
  pinMode(2, OUTPUT);//LED
  pinMode(6, OUTPUT);//LED
  pinMode(7, OUTPUT);
  left.attach(3);
  right.attach(5);
  Serial.begin(57600);
  //  audio();
  digitalWrite(2, LOW); //LED off
  digitalWrite(7, LOW); //sound MUX
  //  while(start==0){
  //    audio();
  //    delay(10);
  //  }
  still();
  delay(4000);
  digitalWrite(7, HIGH); //optical MUX
  digitalWrite(2, HIGH); //LED on
  still();
  
  transmit();

}

void loop() {

  //  coast();
  counter++;
  if (counter==7) delay(3000);
  
  frontWallValue = map(analogRead(frontWall), 0, 1023, 0, 255);
  rightWallValue = map(analogRead(rightWall), 0, 1023, 0, 255);
  leftWallValue = map(analogRead(leftWall), 0, 1023, 0, 255);
//  optical();
  if (rightWallValue < 50) { //no right wall
    turnRight();
    coast();
  } else if (frontWallValue < 50) { //no front wall
    coast();
  } else {
    turnLeft();
    //    coast();
  }
}

~~~
