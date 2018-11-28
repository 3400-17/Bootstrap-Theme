# Milestone 4: Treasure Detection

### Objectives
* Robot which can detect when there are/are not treasures
* Robot which can successfully distinguish between red and blue treasures
* Robot which can successfully distinguish a square, triangle, and diamond shape

# Implementation

### New Chasis
To mount our FPGA and Camera, we lasercut a new wooden chasis, with the wheels hidden inside our structure.  The box design is also exactly 5.5 inches tall, allowing for the IR hat to be mounted at the right level.  

### Treasure Detection and Distinguishing Colors
Working from our prior progress with lab 4, we improved our color detection by playing with the thresholds more and making a more clear image by rewiring our camera and playing more with seting of registers.  

### Distinguishing Shapes
To get the camera to recognize different shapes, we first mounted it on the robot such that the image would be at a consistent level.  If a red or blue treasure was recognized, the Verilog code we wrote would then run a segment to check the shape by checking two specific rows as indicated in the image below by the dotted lines.  Comparing the average color of the bottom row helped us determine the shape.

* If the average color of the top row was more blue, and the average color of the bottom row was more blue, then the shape was a square.
* If the average color of the top row was less blue, and the average color of the bottom row was less blue, then the shape was a diamond.
* If the average color of the top row was less blue, and the average color of the bottom row was more blue, then the shape was a triangle.
* Else, if none of these cases, then the treasure registers as a false read and does not send a signal to the Arduino to avoid losing points.

To fine tune our system, we played with the thresholds which set what "more blue" and "less blue" looked like and added a mirrored code for reading the red shapes.

![Shapes](https://snag.gy/9ZPIOE.jpg)

### Video

### Code
