# Lab 1: Introduction to Arduino and Robot Assembly

### Objectives

* Learn how to use the Arduino Uno and the Arduino IDE.
* Construct a simple Arduino program with multiple components and the Arduino Uno
* Assemble robot and complete a simple, autonomous task. 456


### Teams
**Team 1**: Zoe, Michael

**Team 2**: Natan, Marcela, Siming


## Internal Blink

We downloaded Arduino IDE from [this link.](https://www.arduino.cc/en/Main/Software)

Using the "Blink" code in File>Examples>1.Basics> Blink of Arduino IDE, we could make the internal LED blink. 

<iframe width="560" height="315" src="https://www.youtube.com/embed/n9FKL0pYl8Y" frameborder="0" allow="autoplay; encrypted-media" allowfullscreen></iframe>
 

The code to making the internal LED blink is the following:

~~~
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second
}
~~~

This example helped us understand how an Arduino program worked.  The setup() portion of the code is run once at the initialization of the program and then the loop() portion of the code is run continuously. 

## External Blink

The external LED was connected in series with a 1 kΩ resistor from pin 8 to ground on the Arduino. We modified the internal blink code to flash the LED from pin 8 rather than the built in LED. 

~~~
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(8, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(8, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(8, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second
}
~~~
The video of the external LED blinking is shown the video below:

<iframe width="560" height="315" src="https://www.youtube.com/embed/UREEzOB6Taw" frameborder="0" allow="autoplay; encrypted-media" allowfullscreen></iframe>

This example helped us gain understanding of the I/O pins and how to use them.

## Potentiometer Serial Read

The potentiometer was used to divide voltage, and therefore control the associated electronics in the circuit. The potentiometer works by toggling the resistance as the knob is twisted.  This is shown in the following schematic:

<img src="https://docs.google.com/uc?id=0B1r9QYTd8YNrTmRwODRBZjV1OGs">

The circuit setup is like the following:

![potentiometer](https://snag.gy/1jWVIz.jpg)

We then moditied the 'sensorPin' to be the zeroth pin of the analog pins (AO), and set the sensor value to be 0. 
We also had to initialize the serial port with a Serial.begin() call.

~~~
int sensorPin = A0;
int sensorValue=0;

void setup() {

  Serial.begin(9600);
}
~~~
We print the sensor value on the screen using a `Serial.print` call to print the analogg capture on the sensor pin:
~~~
// the loop function runs over and over again forever
void loop() {
  sensorValue = analogRead(sensorPin);
  Serial.print(sensorValue);
  Serial.println(" ");
  delay(500);                     

}
~~~
Running the code printed out the analog value to the serial monitor. It worked as expected. Here is a video of the setup. 

<iframe width="560" height="315" src="https://www.youtube.com/embed/hpu6lJtAxwc" frameborder="0" allow="autoplay; encrypted-media" allowfullscreen></iframe>

There are 6 different analog input pins ranging from A0 to A5.  

## Potentiometer to LED

We then used the potentiometer to control the brightness the LED. As we rotated the potentiometer, the LED changed brightness:

<iframe width="560" height="315" src="https://www.youtube.com/embed/4isUPZo6t5A" frameborder="0" allow="autoplay; encrypted-media" allowfullscreen></iframe>

This was done with a simulated analog output through the digital output pins enabled with pulse width modulation (PWM). 
PWM is when the digital signal is toggled high and low at specific duty cycles.  This is shown below: 

![PWM](https://upload.wikimedia.org/wikipedia/commons/4/49/Pwm_5steps.gif)

The code for controlling LED intensity is the following:
~~~
int sensorPin = A0;
int sensorValue=0;
void setup() {
  pinMode(9, OUTPUT);
  Serial.begin(9600);
}

// the loop function runs over and over again forever
void loop() {
  sensorValue = analogRead(sensorPin);
  Serial.print(sensorValue);
  Serial.println(" ");
  analogWrite(9, sensorValue/4);   // turn the LED on (HIGH is the voltage level)
  delay(5);                       // wait for a second
}
~~~
An image of the circuit is the following:

![pot_LED](https://snag.gy/vRAMbm.jpg)


## Potentiometer to Servo
We then mapped the values from the potentiometer to control the servo. The Parallax continuous rotation servo takes in values from a PWM signal ranging from 0-180.  At a value of 90, the servo is not moving; increasing toward 180 the servo speeds up in one direction, and decreasing to 0 the servo speeds up in the other direction.  To use the servos, we included the Servo.h library and assigned pin 9, a PWM pin, to the motor.

The circuit setup is shown in the following image:

![pot_servo](https://snag.gy/eOuvxS.jpg)

One thing to note is our use of the map() function, mapping the analog input from 0 to 1023 to a more useful range between 0 to 180 such that the motors could function properly. 

~~~
#include <Servo.h> //include the servo library
Servo myservo; 

int sensorPin = A0;
int sensorValue=0;
void setup() {
  myservo.attach(9);
  Serial.begin(9600);
  
}

void loop() {
  sensorValue = analogRead(sensorPin);   //read the value of the potentiometer
  sensorValue = map(sensorValue, 0, 1023, 0, 180);  //map the reading between 0 and 180
  myservo.write(sensorValue);   //sets the servo position according to the scaled value 
  delay(15);                       // wait 
}
~~~

The video is a demonostration of using the rotation of potentiometer to control the servo:

<iframe width="560" height="315" src="https://www.youtube.com/embed/AC5Cre2YxCQ" frameborder="0" allow="autoplay; encrypted-media" allowfullscreen></iframe>
 
We had to troubleshoot our motors by using a screwdriver to calibrate them.  We did this by setting the motor to an output of 90 and adjusted the screw in the back such that the motor was not moving.  This exercise taught us how the Parallax servo motors worked, which was useful knowledge moving forward.

## Robot Assembly and Driving in a Square

We took cues from the lab instruction and past team websites, and we assembled our robot.  We used a base plate and attached two servo motors to power the main wheels.  We attached a ball bearing third point of contact to the ground.  We then attached the breadboard and Arduino to the top of the plate and wired the motors to the arduino.  The final robot is shown below. 

![assembled robot](https://snag.gy/3An1TO.jpg)


The next thing we had to think about was how we would power the necessary peripherals. To power two servos from our 5V battery pack, we spliced a USB cable such that we were able to get the individual power and ground wires. We hooked up the wires to a arduino-compatible power jack and mounted the battery pack on the bottom of the base plate.  With everything assembled, our robot was ready to be programmed.

To drive out robot, we wrote the code for our robot to move forward, move backward, and turn right.  To move in a square, we programmed the robot to drive forward to 2 seconds, turn for 1.35 seconds (to turn 90 degrees), and then repeat. 

~~~
#include <Servo.h>
Servo left;
Servo right; 

int sensorPin = A0; //assign sensor to pin A0
int sensorValue=0; //set sensorvalue to 0 to initialize it

void setup() {
  left.attach(10); //attach the left servo variable to pin10
  right.attach(9);  //attach the right servo variable to pin9
  Serial.begin(9600); 
  
}

// the loop function runs over and over again forever
void loop() {
  forward(); 
  delay(2000);
  turn();
  delay(1350);
  
}

void forward() {
  left.write(95); //write a value to the servo, 90 being no movement
  right.write(85); 
  
}

void back() {
  right.write(95);
  left.write(85);
  
}

void turn() {
  right.write(95); 
  left.write(90);
}
~~~
The final robot functioned as we expected, and was able to run in a square. A video of our robot working autonomously is shown below:

<iframe width="560" height="315" src="https://www.youtube.com/embed/Wx0h-h_tRTE" frameborder="0" allow="autoplay; encrypted-media" allowfullscreen></iframe>


## Work Distribution

We worked in our teams together through each of the basic arduino assignments (blinking LED, potentiometer, motor).

As we got to building the robot, team 1 focused more on the code for the robot and team 2 focused on the construction of the robot.  


