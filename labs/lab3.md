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

### Hardware

We first plug the radio into the Arduinos using the special printed circuit boards. The radio is wired into the 3.3V pin on the Arduino. We do the same circuit set up for both Arduinos, as one of them will simulate the robot, and one of them will simulate the base station. 

![Radio circuit](https://snag.gy/Rs1Zdq.jpg)

### Software 

We programmed the both of the Arduinos. We chosed one of them being the transmitter and ther other as the receiver. After the codes are loaded on the board, we typed "T" in the serial monitor, and put the Arduino in the tramitter mode. In a successful transmission, the received should send a confirmation message to the transmitter, confirming that it has received the message. 

[Serial Output] (INSERT LINK)

### Maze Information Representation

Since the memory space on the Arduino is limited, we need to make the data as effcient in space as possible. The data structure is the following:

* explored: 1 bit, if the block is explored
* walls: 4 bits, if the robots has walls around it
* treasure: 3 bits, have 2 different colors, and four different shapes
* robots: 1 bit, if there another robot in the neighborhood
* location: 7 bits, representing the location in the maze

![Maze Information](https://snag.gy/EgoWxF.jpg)

![Data Structure](https://snag.gy/tlwBLu.jpg)

### GUI Transmission
We first tested the data transmission with sample data to ensure the radio is working as expected. 


### Code

# Robot Team
### Materials Used:
* Previous Robot
* Audio module
* Optical module
* Protoboard and circuit components

### Goals
* Integrate audio, optical, wall following, radio, line following on a single system

### Hardware:
To make our parts more manageable, we decided to mount our systems on a more compact and robust protoboard. We soldered two 358 op amp ICs and a multiplexer for future use as well.  The protoboard is shown below:

### Software: 
To integrate the FFT based audio and optical recognition, we had to modify code a bit in order to prevent interference with the timers controlling the servo motors.  To do this, every time we used the FFT library, we saved the timer variables in placeholder variables as so:

~~~
  a = ADCSRA;
  b = ADMUX;
  c = DIDR0;
~~~

After the FFT was complete, the values for ADCRA, ADMUX, and DIDR0 were restored such that the servos could operate well.

Since the audio portion only needs to be run once, we implemented this by putting a while loop in the setup portion of the code.  As for the optical part, we have to check as the robot moves forward.  As a result, the IR sensor is checked every time forward is called such that the robot does not run into another robot.

### Video:


### Code: 
~~~
#define LOG_OUT 1 // use the log output function
#define FFT_N 256 // set to 256 point fft

#include <FFT.h> // include the library
#include <Servo.h>
Servo left;
Servo right; 
const int leftOut = A4;
const int rightOut = A5;
const int frontWall = A3;
const int rightWall = A2; 
int leftOutValue=0;
int rightOutValue=0;
int frontWallValue=0;
int rightWallValue=0;
int counter = 0;
int robot = 0;
int start = 0;

int a, b, c;

void forward() {
  left.write(180);
  right.write(0);
  delay(100);
  still();
  optical();
    while (robot==1){
      delay(500);
      optical();
    }
}

void still() {
  left.write(90);
  right.write(90);
}

void turnRight(){
  left.write(100);
  right.write(100);
  delay(725);
  left.write(90);
  right.write(90);
}

void turnLeft(){

  left.write(80);
  right.write(80);
  delay(725);
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
void optical(){
  still();
  a = ADCSRA;
  b = ADMUX;
  c = DIDR0;
  ADCSRA = 0xe5; // set the adc to free running mode
  ADMUX = 0x40; // use adc0
  DIDR0 = 0x01; // turn off the digital input for adc0
    for (int i = 0 ; i < 512 ; i += 2) { // save 256 samples
      while(!(ADCSRA & 0x10)); // wait for adc to be ready
      ADCSRA = 0xf5; // restart adc
      byte m = ADCL; // fetch adc data
      byte j = ADCH;
      int k = (j << 8) | m; // form into an int
      k -= 0x0200; // form into a signed int
      k <<= 6; // form into a 16b signed int
      fft_input[i] = k; // put real data into even bins
      fft_input[i+1] = 0; // set odd bins to 0
    }
    fft_window(); // window the data for better frequency response
    fft_reorder(); // reorder the data before doing the fft
    fft_run(); // process the data in the fft
    fft_mag_log(); // take the output of the fft
    sei(); 
    if (fft_log_out[82]>67){
      digitalWrite(2, HIGH);
      robot=1;
    }else{
      robot=0;
      digitalWrite(2,LOW);
    }
  ADCSRA = a; // set the adc to free running mode
  ADMUX = b; // use adc0
  DIDR0 = c; //
}

void audio(){
  still();
  a = ADCSRA;
  b = ADMUX;
  c = DIDR0;
  ADCSRA = 0xe5; // set the adc to free running mode
  ADMUX = 0x41; // use adc0
  DIDR0 = 0x01; // turn off the digital input for adc0
    for (int i = 0 ; i < 512 ; i += 2) { // save 256 samples
      while(!(ADCSRA & 0x10)); // wait for adc to be ready
      ADCSRA = 0xf5; // restart adc
      byte m = ADCL; // fetch adc data
      byte j = ADCH;
      int k = (j << 8) | m; // form into an int
      k -= 0x0200; // form into a signed int
      k <<= 6; // form into a 16b signed int
      fft_input[i] = k; // put real data into even bins
      fft_input[i+1] = 0; // set odd bins to 0
    }
    fft_window(); // window the data for better frequency response
    fft_reorder(); // reorder the data before doing the fft
    fft_run(); // process the data in the fft
    fft_mag_log(); // take the output of the fft
    sei();
    if (fft_log_out[4]>150){
      start=1;
    }
  ADCSRA = a; // set the adc to free running mode
  ADMUX = b; // use adc0
  DIDR0 = c; //
}

void setup() {
  pinMode(2, OUTPUT);
  pinMode(6, OUTPUT);
  left.attach(3);
  right.attach(5);   
  Serial.begin(115200); // use the serial port
  audio();
  digitalWrite(6,LOW);
  while(start==0){
    audio();
    delay(10);
  }
  digitalWrite(6, HIGH);

}

void loop() {
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
~~~

# Code for maze below

We defined our maze as a matrix of 9 by 9 words. Each word is 16 bits, which is sufficient for all our information needed. 
~~~
define maze 
word maze[9][9] 
~~~

The following code is for transmitting the information. The transmitter is sent as ping_out, and send over the information as defined below. The information is then shifted to cancatenate into a 16 bit word to transmit over radio channel. Then it start listening for the confirmation message from the receiver and print out the success or failure of the transmission. 

~~~
sender 
send update 


void loop(void)
{
  //
  // Ping out role.  Repeatedly send the current time
  //

  if (role == role_ping_out)
  {
    // First, stop listening so we can talk.
    radio.stopListening();

    // Take the time, and send it.  This will block until complete
    unsigned long time = millis();
    printf("Now sending ...");
    word loc=9; //0001001
    word explored=0;
    word treasure=5;// 101 blue tirangle
    word robot=0; //no robot 
    word walls=12;//1100 north &south
   
    word package=explored<<15|walls<<11|treasure<<8|robot<<7|loc;

    
    // Take the time, and send it.  This will block until complete

    bool ok = radio.write( package,sizeof(word) );

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
      if (millis() - started_waiting_at > 200 )
        timeout = true;

    // Describe the results
    if ( timeout )
    {
      printf("Failed, response timed out.\n\r");
    }
    else
    {
      // Grab the response, compare, and send to debugging spew
      unsigned long got_time;
      radio.read( &got_time, sizeof(unsigned long) );

      // Spew it
      printf("Got response %lu, round-trip delay: %lu\n\r",got_time,millis()-got_time);
    }

    // Try again 1s later
    delay(1000);
  }
~~~
For the receiver, the code is the following. The code first set the role of the Arduino as receiver, and check if the data transmission has finished. Once it has confirmed that the data transmission is over, the receiver will switch roles to transmit a confirmation message. 
~~~
receiver's update 
 if ( role == role_pong_back )
  {
    // if there is data ready
    if ( radio.available() )
    {
      // Dump the payloads until we've gotten everything
      word package;
      bool done = false;
      while (!done)
      {
        // Fetch the payload, and see if this was the last one.
        done = radio.read( package, sizeof(package) );

        // Spew it
        printf("Got payload ...",package);
       

        // Delay just a little bit to let the other unit
        // make the transition to receiver
        delay(20);

      }

      // First, stop listening so we can talk
      radio.stopListening();
      
      // Send the final one back.
      radio.write( 6, sizeof(int) );
      printf("Sent response.\n\r");

      // Now, resume listening so we catch the next packets.
      radio.startListening();
    }
  }
~~~
The following code are the code for sender. It adds power and time settings in addition to the framwork for sender as shown before. 

~~~
//
sender code

/*
 Copyright (C) 2011 J. Coliz <maniacbug@ymail.com>
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

/**
 * Example for Getting Started with nRF24L01+ radios.
 *
 * This is an example of how to use the RF24 class.  Write this sketch to two
 * different nodes.  Put one of the nodes into 'transmit' mode by connecting
 * with the serial monitor and sending a 'T'.  The ping node sends the current
 * time to the pong node, which responds by sending the value back.  The ping
 * node can then see how long the whole cycle took.
 */

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

//
// Hardware configuration
//

// Set up nRF24L01 radio on SPI bus plus pins 9 & 10

RF24 radio(9,10);

//
// Topology
//

// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0x000000002ELL, 0x000000002FLL };

//
// Role management
//
// Set up role.  This sketch uses the same software for all the nodes
// in this system.  Doing so greatly simplifies testing.
//

// The various roles supported by this sketch
typedef enum { role_ping_out = 1, role_pong_back } role_e;

// The debug-friendly names of those roles
const char* role_friendly_name[] = { "invalid", "Ping out", "Pong back"};

// The role of the current running sketch
role_e role = role_pong_back;

void setup(void)
{
  //
  // Print preamble
  //

  Serial.begin(57600);
  printf_begin();
  printf("\n\rRF24/examples/GettingStarted/\n\r");
  printf("ROLE: %s\n\r",role_friendly_name[role]);
  printf("*** PRESS 'T' to begin transmitting to the other node\n\r");

  //
  // Setup and configure rf radio
  //

  radio.begin();

  // optionally, increase the delay between retries & # of retries
  radio.setRetries(15,15);
  radio.setAutoAck(true);
  // set the channel
  radio.setChannel(0x50);
  // set the power
  // RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_MED=-6dBM, and RF24_PA_HIGH=0dBm.
  radio.setPALevel(RF24_PA_MAX);
  //RF24_250KBPS for 250kbs, RF24_1MBPS for 1Mbps, or RF24_2MBPS for 2Mbps
  radio.setDataRate(RF24_250KBPS);

  // optionally, reduce the payload size.  seems to
  // improve reliability
  //radio.setPayloadSize(8);

  //
  // Open pipes to other nodes for communication
  //

  // This simple sketch opens two pipes for these two nodes to communicate
  // back and forth.
  // Open 'our' pipe for writing
  // Open the 'other' pipe for reading, in position #1 (we can have up to 5 pipes open for reading)

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

  //
  // Start listening
  //

  radio.startListening();

  //
  // Dump the configuration of the rf unit for debugging
  //

  radio.printDetails();
}
word walls=1;//1100 north &south
void loop(void)
{
  //
  // Ping out role.  Repeatedly send the current time
  //

  if (role == role_ping_out)
  {
    // First, stop listening so we can talk.
    radio.stopListening();

    // Take the time, and send it.  This will block until complete
    printf("Now sending ...");
    word loc=9; //0001001
    word explored=0;
    word treasure=5;// 101 blue tirangle
    word robot=0; //no robot 
    
    walls=walls+1;
    word package=explored<<15|walls<<11|treasure<<8|robot<<7|loc;

    
    // Take the time, and send it.  This will block until complete

    bool ok = radio.write(&package,sizeof(package) );

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
      if(radio.read( &confirm, sizeof(int) ))
      {printf("Got confirming response %d",confirm);
      }
      Serial.println();
      // Spew it
      
      
    }

    // Try again 1s later
    delay(2000);
  }

  //
  // Pong back role.  Receive each packet, dump it out, and send it back
  //

  if ( role == role_pong_back )
  {
    // if there is data ready
    if ( radio.available() )
    {
      // Dump the payloads until we've gotten everything
      unsigned long got_time;
      bool done = false;
      while (!done)
      {
        // Fetch the payload, and see if this was the last one.
        done = radio.read( &got_time, sizeof(unsigned long) );

        // Spew it
        printf("Got payload %lu...",got_time);

        // Delay just a little bit to let the other unit
        // make the transition to receiver
        delay(20);

      }

      // First, stop listening so we can talk
      radio.stopListening();

      // Send the final one back.
      radio.write( &got_time, sizeof(unsigned long) );
      printf("Sent response.\n\r");

      // Now, resume listening so we catch the next packets.
      radio.startListening();
    }
  }

  //
  // Change roles
  //

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
// vim:cin:ai:sts=2 sw=2 ft=cpp

## Robot Team

Michael and Natan

### Materials used:
* Robot
* Decoy
* 660Hz tone generator
* Walls to make the following maze setup


### Goals
We made the robot go through the maze on the line and following the wall. We also implemented IR hat detection to simulate the process of running into another robot in the maze. 

![Maze Diagram](https://snag.gy/YankGH.jpg)

### Hardware
We implemented an integrated circuit with functionalities from lab 2, as shown in the following diagram. The board allows the robot to pick up 660Hz frequency, and distinguish between the decoys and robots. 
![IR detection circuit](https://snag.gy/kgXlA9.jpg)

### Software

### Code

### Integration
After the two teams has completed their parts, we integrated the two systems together. Our robot is able to run through the maze autonomously, starting on 660Hz tone, and updating the GUI on the screen as it runs. The robot stops if there is another robot, but continues to run if there is a decoy. 

### Video




/*
 Copyright (C) 2011 J. Coliz <maniacbug@ymail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

/**
 * Example for Getting Started with nRF24L01+ radios.
 *
 * This is an example of how to use the RF24 class.  Write this sketch to two
 * different nodes.  Put one of the nodes into 'transmit' mode by connecting
 * with the serial monitor and sending a 'T'.  The ping node sends the current
 * time to the pong node, which responds by sending the value back.  The ping
 * node can then see how long the whole cycle took.
 */

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

//
// Hardware configuration
//

// Set up nRF24L01 radio on SPI bus plus pins 9 & 10

RF24 radio(9,10);

//
// Topology
//

// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0x000000002ELL, 0x000000002FLL };

//
// Role management
//
// Set up role.  This sketch uses the same software for all the nodes
// in this system.  Doing so greatly simplifies testing.
//

// The various roles supported by this sketch
typedef enum { role_ping_out = 1, role_pong_back } role_e;

// The debug-friendly names of those roles
const char* role_friendly_name[] = { "invalid", "Ping out", "Pong back"};

// The role of the current running sketch
role_e role = role_pong_back;

void setup(void)
{
  //
  // Print preamble
  //

  Serial.begin(57600);
  printf_begin();
//  printf("\n\rRF24/examples/GettingStarted/\n\r");
//  printf("ROLE: %s\n\r",role_friendly_name[role]);
//  printf("*** PRESS 'T' to begin transmitting to the other node\n\r");
//
//  
  // Setup and configure rf radio
  //

  radio.begin();

  // optionally, increase the delay between retries & # of retries
  radio.setRetries(15,15);
  radio.setAutoAck(true);
  // set the channel
  radio.setChannel(0x50);
  // set the power
  // RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_MED=-6dBM, and RF24_PA_HIGH=0dBm.
  radio.setPALevel(RF24_PA_MAX);
  //RF24_250KBPS for 250kbs, RF24_1MBPS for 1Mbps, or RF24_2MBPS for 2Mbps
  radio.setDataRate(RF24_250KBPS);

  // optionally, reduce the payload size.  seems to
  // improve reliability
  //radio.setPayloadSize(8);


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

  //
  // Start listening
  //

  radio.startListening();

  //
  // Dump the configuration of the rf unit for debugging
  //

  radio.printDetails();
}

void loop(void)
{
  
  delay(1000);
  //
  // Ping out role.  Repeatedly send the current time
  //

  if (role == role_ping_out)
  {
    // First, stop listening so we can talk.
    radio.stopListening();

    // Take the time, and send it.  This will block until complete
    unsigned long time = millis();
    printf("Now sending %lu...",time);
    bool ok = radio.write( &time, sizeof(unsigned long) );

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
      if (millis() - started_waiting_at > 200 )
        timeout = true;

    // Describe the results
    if ( timeout )
    {
      printf("Failed, response timed out.\n\r");
    }
    else
    {
      // Grab the response, compare, and send to debugging spew
      unsigned long got_time;
      radio.read( &got_time, sizeof(unsigned long) );

      // Spew it
      printf("Got response %lu, round-trip delay: %lu\n\r",got_time,millis()-got_time);
    }

    // Try again 1s later
    delay(1000);
  }

  //
  // Pong back role.  Receive each packet, dump it out, and send it back
  //

  if ( role == role_pong_back )
  {
    Serial.println("reset");
    // if there is data ready
    if ( radio.available() )
    {
      // Dump the payloads until we've gotten everything
      word got_update;
      bool done = false;
      while (!done)
      {
        // Fetch the payload, and see if this was the last one.
        done = radio.read( &got_update, sizeof(word) );
        // Spew it
        printf("Got payload ...");

        // Delay just a little bit to let the other unit
        // make the transition to receiver
        word explored=got_update>>15;
        word walls=got_update>>11&15; //  1111
        word treasure=got_update>>8&8; //  111;
        word robot=got_update>>7&1;
        word loc=got_update&127; //127 is 1111111
        word x=loc%9;
        int y=loc/9-0.5;
        char c=',';
        Serial.print(x);
        Serial.print(c);
        Serial.print(y);
        Serial.print(c);
        Serial.print("north=");
        Serial.print(walls&8>>3);
        Serial.print(c);
        Serial.print("south=");
        Serial.print(walls&4>>2);
        Serial.print(c);
        Serial.print("west=");
        Serial.print(walls&2>>1);
        Serial.print(c);
        Serial.print("east=");
        Serial.print(walls&1);
        Serial.print("\n");
        
        
        
       delay(20);
        
      }
  
      // First, stop listening so we can talk
      radio.stopListening();

      // Send the final one back.
      int confirm=6;
      radio.write( &confirm, sizeof(int) );
      printf("Sent confirm.\n\r");

      // Now, resume listening so we catch the next packets.
      radio.startListening();
    }
  }

  //
  // Change roles
  //

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
// vim:cin:ai:sts=2 sw=2 ft=cpp

                        
                        
                        
 sender's code
                   
/*
 Copyright (C) 2011 J. Coliz <maniacbug@ymail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

/**
 * Example for Getting Started with nRF24L01+ radios.
 *
 * This is an example of how to use the RF24 class.  Write this sketch to two
 * different nodes.  Put one of the nodes into 'transmit' mode by connecting
 * with the serial monitor and sending a 'T'.  The ping node sends the current
 * time to the pong node, which responds by sending the value back.  The ping
 * node can then see how long the whole cycle took.
 */

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

//
// Hardware configuration
//

// Set up nRF24L01 radio on SPI bus plus pins 9 & 10

RF24 radio(9,10);

//
// Topology
//

// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0x000000002ELL, 0x000000002FLL };

//
// Role management
//
// Set up role.  This sketch uses the same software for all the nodes
// in this system.  Doing so greatly simplifies testing.
//

// The various roles supported by this sketch
typedef enum { role_ping_out = 1, role_pong_back } role_e;

// The debug-friendly names of those roles
const char* role_friendly_name[] = { "invalid", "Ping out", "Pong back"};

// The role of the current running sketch
role_e role = role_pong_back;

    word loc=9; //0001001
    word explored=0;
    word treasure=5;// 101 blue tirangle
    word robot=0; //no robot 
    word walls=12;//1100 north &south
void setup(void)
{
  //
  // Print preamble
  //

  Serial.begin(57600);
  printf_begin();
  printf("\n\rRF24/examples/GettingStarted/\n\r");
  printf("ROLE: %s\n\r",role_friendly_name[role]);
  printf("*** PRESS 'T' to begin transmitting to the other node\n\r");

  //
  // Setup and configure rf radio
  //

  radio.begin();

  // optionally, increase the delay between retries & # of retries
  radio.setRetries(15,15);
  radio.setAutoAck(true);
  // set the channel
  radio.setChannel(0x50);
  // set the power
  // RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_MED=-6dBM, and RF24_PA_HIGH=0dBm.
  radio.setPALevel(RF24_PA_HIGH);
  //RF24_250KBPS for 250kbs, RF24_1MBPS for 1Mbps, or RF24_2MBPS for 2Mbps
  radio.setDataRate(RF24_250KBPS);

  // optionally, reduce the payload size.  seems to
  // improve reliability
  //radio.setPayloadSize(8);

  //
  // Open pipes to other nodes for communication
  //

  // This simple sketch opens two pipes for these two nodes to communicate
  // back and forth.
  // Open 'our' pipe for writing
  // Open the 'other' pipe for reading, in position #1 (we can have up to 5 pipes open for reading)

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

  //
  // Start listening
  //

  radio.startListening();

  //
  // Dump the configuration of the rf unit for debugging
  //

  radio.printDetails();
}

void loop(void)
{
  //
  // Ping out role.  Repeatedly send the current time
  //

  if (role == role_ping_out)
  {
    // First, stop listening so we can talk.
    radio.stopListening();

    // Take the time, and send it.  This will block until complete
    printf("Now sending ...");
    walls=walls+1;
    loc=loc+1;

  
   
    word package=explored<<15|walls<<11|treasure<<8|robot<<7|loc;

    
    // Take the time, and send it.  This will block until complete

    bool ok = radio.write(&package,sizeof(package) );

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
      if (millis() - started_waiting_at > 200 )
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
      {printf("Got confirming response");
      }
      // Spew it
      
      
    }

    // Try again 1s later
    delay(1000);
  }

  //
  // Pong back role.  Receive each packet, dump it out, and send it back
  //

  if ( role == role_pong_back )
  {
    // if there is data ready
    if ( radio.available() )
    {
      // Dump the payloads until we've gotten everything
      unsigned long got_time;
      bool done = false;
      while (!done)
      {
        // Fetch the payload, and see if this was the last one.
        done = radio.read( &got_time, sizeof(unsigned long) );

        // Spew it
        printf("Got payload %lu...",got_time);

        // Delay just a little bit to let the other unit
        // make the transition to receiver
        delay(20);

      }

      // First, stop listening so we can talk
      radio.stopListening();

      // Send the final one back.
      radio.write( &got_time, sizeof(unsigned long) );
      printf("Sent response.\n\r");

      // Now, resume listening so we catch the next packets.
      radio.startListening();
    }
  }

  //
  // Change roles
  //

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
// vim:cin:ai:sts=2 sw=2 ft=cpp          
