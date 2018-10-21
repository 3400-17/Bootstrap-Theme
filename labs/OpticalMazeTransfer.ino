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
int counter = 0;
int start = 2;
int compass = 3; //north = 0, east = 1, south = 2, west = 3
int north, south, east, west;

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
  while (1) {
    leftOutValue = map(analogRead(leftOut), 0, 1023, 0, 255);
    rightOutValue = map(analogRead(rightOut), 0, 1023, 0, 255);

    if (leftOutValue < 170 && rightOutValue < 170) {
      still();
      break;
    } else if (rightOutValue < 170 ) {
      left.write(100);
      right.write(90);
    } else if (leftOutValue < 170) {
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
  frontWallValue = map(analogRead(frontWall), 0, 1023, 0, 255);
  rightWallValue = map(analogRead(rightWall), 0, 1023, 0, 255);
  leftWallValue = map(analogRead(leftWall), 0, 1023, 0, 255);
  optical();
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
