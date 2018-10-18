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

![Radio circuit] (https://snag.gy/Rs1Zdq.jpg)

### Software 

We programmed the both of the Arduinos. We chosed one of them being the transmitter and ther other as the receiver. After the codes are loaded on the board, we typed "T" in the serial monitor, and put the Arduino in the tramitter mode. In a successful transmission, the received should send a confirmation message to the transmitter, confirming that it has received the message. 

[Serial Output] (inset link)

### Maze Information Representation

Since the memory space on the Arduino is limited, we need to make the data as effcient in space as possible. The data structure is the following:

* explored: 1 bit, if the block is explored
* walls: 4 bits, if the robots has walls around it
* treasure: 3 bits, have 2 different colors, and four different shapes
* robots: 1 bit, if there another robot in the neighborhood
* location: 7 bits, representing the location in the maze

[Maze Information] (https://snag.gy/zErULK.jpg)

[Data Structure] (https://snag.gy/tlwBLu.jpg)

### GUI Transmission
We first tested the data transmission with sample data to ensure the radio is working as expected. 


### Code

~~~
define maze 
word maze[9][9]

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
    word loc=9; //0001001
    word explored=0;
    word treasure=5;// 101 blue tirangle
    word robot=0; //no robot 
    word walls=12;//1100 north &south
   
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


~~~
## Robot Team

Michael and Natan

### Materials used:
* Robot
* Decoy
* 660Hz tone generator
* Walls to make the following maze setup


### Goals
We made the robot go through the maze on the line and following the wall. We also implemented IR hat detection to simulate the process of running into another robot in the maze. 

[Maze Diagram] (https://snag.gy/YankGH.jpg)

### Hardware

### Software

### Code

### Integration
After the two teams has completed their parts, we integrated the two systems together. Our robot is able to run through the maze autonomously, starting on 660Hz tone, and updating the GUI on the screen as it runs. The robot stops if there is another robot, but continues to run if there is a decoy. 

### Video

