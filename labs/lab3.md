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

We first plugged the radio into the Arduinos using the special PCBs. The radio is wired into the 3.3V pin on the Arduino, or through a 3.3V supply if the Arduino is a knockoff. We do the same circuit set up for both Arduinos, as one of them will simulate the robot, and the other  simulate the base station. 

![Radio circuit](https://snag.gy/Rs1Zdq.jpg)

### Software 

We programmed the both of the Arduinos. We chosed one of them being the transmitter and ther other as the receiver. After the codes are loaded on the board, we typed "T" in the serial monitor, and put the Arduino in the tramitter mode. In a successful transmission, the received should send a confirmation message to the transmitter, confirming that it has received the message. 

[Serial Output] (INSERT LINK)

### Data Structure for Sending/Receiving Maze Information

Since the memory space on the Arduino is limited, we need to make the data as effcient in space as possible. The data structure is the following:

* explored: 1 bit, if the block is explored
* walls: 4 bits, if the robots has walls around it
* treasure: 3 bits, have 2 different colors, and four different shapes
* robots: 1 bit, if there another robot in the neighborhood
* location: 7 bits, representing the location in the maze

![Maze Information](https://snag.gy/EgoWxF.jpg)

![Data Structure](https://snag.gy/tlwBLu.jpg)

### Wireless Transmission
We first tested the data transmission with sample data to ensure the radio is working as expected. 

                  
### Updating the GUI from a virtual robot 
The Arduino will send a message to the GUI just as it prints data to the serial monitor of the Arduino IDE. The GUI opens up a website that receives updates from the robot in real time. 
                  
### Code for Radio

### Code for GUI
                  
                  

                  
                  
                  
                  
                  
                  

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
<iframe width="560" height="315" src="https://www.youtube.com/embed/Orb9eo7mNDc" frameborder="0" allow="autoplay; encrypted-media" allowfullscreen></iframe>
