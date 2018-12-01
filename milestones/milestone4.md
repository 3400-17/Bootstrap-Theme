# Milestone 4: Treasure Detection

### Objectives
* Robot which can detect when there are/are not treasures
* Robot which can successfully distinguish between red and blue treasures
* Robot which can successfully distinguish a square, triangle, and diamond shape

# Implementation

### Mounting the Hardware
To mount our FPGA and Camera, we lasercut a new wooden chasis, with the wheels hidden inside our structure.  The box design is also exactly 5.5 inches tall, allowing for the IR hat to be mounted at the right level.  The Camera has a slot that it fits in in the front of the robot with screw holes as well.  The FPGA fits on the edge of the box, next to one of the batteries.  The pins we need for image processing are exposed on the top and the power pins are exposed as well.  These additions are shown below:


![mounting](https://snag.gy/TG87bP.jpg)

### Treasure Detection and Distinguishing Colors
Working from our prior progress with lab 4, we improved our color detection by playing with the thresholds more and making a more clear image by rewiring our camera and playing more with seting of registers.  A video is shown below:

<iframe width="560" height="315" src="https://www.youtube.com/embed/oWXQA4cXbi8" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

For testing purposes, we attached extra lights onto our system and connected it to the computer to program the FPGA.  Resultingly, the entire system did not fit into our box but our camera was still mounted properly.  This is shown below:
![camera](https://snag.gy/rGZ8pC.jpg)

To count the number of red and blue pixels more easily, we use threshold values to filter out the noise pixels values apart from the red blud color from treasure. 
~~~
if(RED < 4'b0011 && GREEN < 4'b0011 && BLUE > 4'b0 && X_ADDR > 15'd30 && Y_ADDR > 15'd15) begin 		
	 pixel_data_RGB332[7:0] = 8'b00000011; end
else if(RED > 4'b0000 && GREEN < 4'b0101 && BLUE < 4'b0011 && X_ADDR > 15'd30 && Y_ADDR > 15'd15) begin
	pixel_data_RGB332[7:0] = 8'b11100000; end
else begin pixel_data_RGB332[7:0] = 8'b11111111; end
~~~

### multi-frame color detection
In the image processor, we don't only count the number of pixels in each frame but also count the color of frames for a period of ten, so the result will be updated once each ten frames.

#### this is to count color of pixels 
~~~
if (PIXEL_IN== 8'b11111111)begin countNULL=countNULL+1; end
else begin 
	if(PIXEL_IN[7:5] > 3'b000   ) begin 
		countRED = countRED + 16'd1; 
	end 
	// Usually, we have brighter red, so the red value threshold is higher.
	 else if(PIXEL_IN[1:0] > 3'b000 ) begin 
		countBLUE = countBLUE + 16'd1; 
	end 
end
~~~
#### this is to count color of frames each ten frames
~~~

if(VGA_VSYNC_NEG == 1'b0 && lastsync == 1'b1) begin //negedge VSYNC 
	if(toggle==4'd10) begin //update frame color registers and reset  frame color counter 
	   r1<=red_frame;
		b1<=blue_frame;
		n1<=null_frame;
		blue_frame<=0;
		red_frame<=0;
		null_frame<=0;
		toggle<=0;
		end

	else begin
		toggle<=toggle+1;
		/*----------------------- Testing lor ---------------------------*/
		if(countRED > countBLUE  && countRED>16'd5000) begin  
			red_frame<=red_frame+1;
			blue_frame<=blue_frame;
			null_frame<=null_frame;

		end 
		else if(countBLUE > countRED && countBLUE>16'd5000) begin 
			blue_frame<=blue_frame+1;
			red_frame<=red_frame;
			null_frame<=null_frame;

		end 
		else  begin //if(countNULL > countBLUE +16'd1000 && countNULL > countRED + 16'd1000)
			null_frame<=null_frame+1;
			blue_frame<=blue_frame;
			red_frame<=red_frame;
		end
	end
	if(r1 > b1 && r1>=n1) begin  
		reg_result[3] <= 1'b1; //isRed
		reg_result[4] <= 1'b0;
		reg_result[5] <= 1'b0;
	end

	else if(b1 > r1 && b1>n1) begin 
		reg_result[4] <= 1'b1; //isBlue
		reg_result[3] <= 1'b0;
		reg_result[5] <= 1'b0;
		end

	else begin 
		reg_result[5] <= 1'b1; //null
		reg_result[3] <= 1'b0;
		reg_result[4] <= 1'b0;
		end 

	countBLUE = 16'b0; 
	countRED = 16'b0; 
	countNULL = 16'b0; 

	end 
	lastsync = VGA_VSYNC_NEG; 
	lastY = VGA_PIXEL_Y;

end

~~~




### Distinguishing Shapes
To get the camera to recognize different shapes, we first mounted it on the robot such that the image would be at a consistent level.  If a red or blue treasure was recognized, the Verilog code we wrote would then run a segment to check the shape by checking two specific rows as indicated in the image below by the dotted lines.  Comparing the average color of the bottom row helped us determine the shape.

* If the average color of the top row was more blue, and the average color of the bottom row was more blue, then the shape was a square.
* If the average color of the top row was less blue, and the average color of the bottom row was less blue, then the shape was a diamond.
* If the average color of the top row was less blue, and the average color of the bottom row was more blue, then the shape was a triangle.
* Else, if none of these cases, then the treasure registers as a false read and does not send a signal to the Arduino to avoid losing points.

To fine tune our system, we played with the thresholds which set what "more blue" and "less blue" looked like and added a mirrored code for reading the red shapes.

#### here is how we detect blue lines
~~~ 
if ( VGA_PIXEL_Y == 6'd42) begin
				BLUE_LINE_0 = BLUE_LINE_0 + 1'b1;
				end
			if ( VGA_PIXEL_Y == 7'd72) begin
				BLUE_LINE_1 = BLUE_LINE_1 + 1'b1;
				end
			if ( VGA_PIXEL_Y == 7'd102) begin
				BLUE_LINE_2 = BLUE_LINE_2 + 1'b1;
				end
~~~
#### here is how we detect red lines
~~~
			if ( VGA_PIXEL_Y == 6'd42) begin
				RED_LINE_0 = RED_LINE_0 + 1'b1;
				end
			if ( VGA_PIXEL_Y == 7'd72) begin
				RED_LINE_1 = RED_LINE_1 + 1'b1;
				end
			if ( VGA_PIXEL_Y == 7'd102) begin
				RED_LINE_2 = RED_LINE_2 + 1'b1;
				end
		end 
~~~

![Shapes](https://snag.gy/9ZPIOE.jpg)

### Improving Performance

One issue we ran into was inconsistencies in the location and depth of the shape.  This meant that sometimes the shape was very large and close to the camera and other times it was further away and much smaller, making thresholding difficult.  To fix this, we planned to do edge detection.  At the beginning of each row, the FPGA would recognize the first colored pixel and the last colored pixel.  Comparing the distances, the threshold would be scaled appropriately in order to have a accurate recognition of the shape.  

Unfortunately, we were not able to get the full functionality working by today.  Our triangle code works decently but it proved difficult to fully get the recognition of the square and diamond.



