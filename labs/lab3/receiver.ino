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


RF24 radio(9,10);


// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0x000000002ELL, 0x000000002FLL };

// The various roles supported by this sketch
typedef enum { role_ping_out = 1, role_pong_back } role_e;

// The debug-friendly names of those roles
const char* role_friendly_name[] = { "invalid", "Ping out", "Pong back"};

role_e role = role_pong_back;

int walls=0;
void setup(void)
{
//  printf("reset");
//  delay(1000);
// 
 

  Serial.begin(57600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  printf_begin();
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

  //radio.printDetails();
}

void loop(void)
{
 

  //
  // Pong back role.  Receive each packet, dump it out, and send it back
  //

  if ( role == role_pong_back )
  {
 
    //Serial.println("reset");
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
      
      // First, stop listening so we can talk
      radio.stopListening();

      // Send the final one back.
      int confirm=6;
      if (radio.write( &confirm, sizeof(confirm) )){
         //printf("Sent confirm.\n\r");
         }
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
