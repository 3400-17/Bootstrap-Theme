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
const int leftWall = A1;
int leftOutValue=0;
int rightOutValue=0;
int frontWallValue=0;
int rightWallValue=0;
int leftWallValue=0;
int counter = 0;
int robot = 0;
int start = 0;
int compass = 0; //north = 0, east = 1, south = 2, west = 3
int location = 80;

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
  compass = (compass+1)%4;
  left.write(100);
  right.write(100);
  delay(725);
  left.write(90);
  right.write(90);
  
}

void turnLeft(){
  compass = (compass-1)%4;
  left.write(80);
  right.write(80);
  delay(725);
  left.write(90);
  right.write(90);
}

void coast(){
  switch (compass) {
  case 0://north
    location = location - 9;
    break;
  case 1://east
    location = location + 1;
    break;
  case 2: //south
    location = location + 9;
    break;
  case 3: //west
    location = location - 1;
    break;
  }
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
  left.write(180);
  right.write(0);
  delay(450);
  still();
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
//    Serial.println("start");
//    for (byte i = 0 ; i < FFT_N/2 ; i++) { 
//      Serial.println(fft_log_out[i]); // send out the data
//    }

    Serial.println(fft_log_out[82]);
    
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
//    Serial.println("start");
//    for (byte i = 0 ; i < FFT_N/2 ; i++) { 
//      Serial.println(fft_log_out[i]); // send out the data
//    }

    Serial.println(fft_log_out[4]);
    
    if (fft_log_out[4]>160){
      start=1;
    }
  ADCSRA = a; // set the adc to free running mode
  ADMUX = b; // use adc0
  DIDR0 = c; //
}

void setup() {
  pinMode(2, OUTPUT);//LED
  pinMode(6, OUTPUT);//LED
  pinMode(7, OUTPUT);
  left.attach(3);
  right.attach(5);   
  Serial.begin(115200); // use the serial port
  audio();
  digitalWrite(2,LOW);//LED off
  digitalWrite(7, LOW); //sound MUX
  while(start==0){
    audio();
    delay(10);
  }
  digitalWrite(7,HIGH); //optical MUX
  digitalWrite(2, HIGH); //LED on
  

}

void loop() {
//  coast();
  frontWallValue = map(analogRead(frontWall), 0, 1023, 0, 255); 
  rightWallValue = map(analogRead(rightWall), 0, 1023, 0, 255); 
  leftWallValue = map(analogRead(leftWall), 0, 1023, 0, 255);
  if (rightWallValue < 50){ //no right wall
    turnRight();
    coast();
  } else if (frontWallValue < 50){ //no front wall
    coast();
  }else{
    turnLeft();
  }     
}
