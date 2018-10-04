# Milestone 2

## Objectives
* successfully circle an arbitrary set of walls (e.g. through right hand wall following)
* detect other robots on the field
* combine line following, robot detection, and wall detection - show what the robot is thinking with LEDs

## Implementation

### Hardware Additions

** Need a picture here of line sensors on robot**

To enable wall following, we needed to incorporate two short range IR sensors to detect distance to walls.  These sensors were placed at the front of the robot and on the right side of the robot to perform right hand wall following.  

We did some initial testing using the analogInOutSerial code and collected the following data:
* When a wall was close, the analog out reading was around 110
* When a wall was not close, the analog out reading was around 30

From these values, we chose a cutoff detection threshold at 50 moving forward.

### Right Hand Wall Following Algorithm

To first understand Right Hand Wall Following (RHWF), we watched some YouTube videos to see it in action.  Those can be found here and [here](https://www.youtube.com/watch?v=U4N7bvGnByQ).  Next, we moved our robot into place and reasoned out the logic, which can be represented with the pseudocode below:

```
if (no wall on Right){
   turn right 90 degrees
   move forward one square
}else if (no wall ahead){
   move forward one square
}else{
   turn left 90 degrees
}
```

Following these steps, the robot can get out of any maze eventually by traversing the wall in a priority of turn right, go striaght, turn left.   

To debug this algorithm, we first used Serial.println to see what logic the robot would perform next.  Using this and manually placing our robot at various points in the maze, we were able to confirm that our algorithm worked. 

## Adding Line Following

We can utilize

## Detecting Other Robots

### Code

### Video

