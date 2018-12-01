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

### Distinguishing Shapes
To get the camera to recognize different shapes, we first mounted it on the robot such that the image would be at a consistent level.  If a red or blue treasure was recognized, the Verilog code we wrote would then run a segment to check the shape by checking two specific rows as indicated in the image below by the dotted lines.  Comparing the average color of the bottom row helped us determine the shape.

* If the average color of the top row was more blue, and the average color of the bottom row was more blue, then the shape was a square.
* If the average color of the top row was less blue, and the average color of the bottom row was less blue, then the shape was a diamond.
* If the average color of the top row was less blue, and the average color of the bottom row was more blue, then the shape was a triangle.
* Else, if none of these cases, then the treasure registers as a false read and does not send a signal to the Arduino to avoid losing points.

To fine tune our system, we played with the thresholds which set what "more blue" and "less blue" looked like and added a mirrored code for reading the red shapes.

![Shapes](https://snag.gy/9ZPIOE.jpg)

### Improving Performance

One issue we ran into was inconsistencies in the location and depth of the shape.  This meant that sometimes the shape was very large and close to the camera and other times it was further away and much smaller, making thresholding difficult.  To fix this, we planned to do edge detection.  At the beginning of each row, the FPGA would recognize the first colored pixel and the last colored pixel.  Comparing the distances, the threshold would be scaled appropriately in order to have a accurate recognition of the shape.  

Unfortunately, we were not able to get the full functionality working by today.  Our triangle code works decently but it proved difficult to fully get the recognition of the square and diamond.


### Code
~~~
`define SCREEN_WIDTH 176
`define SCREEN_HEIGHT 144

///////* DON'T CHANGE THIS PART *///////
module DE0_NANO(
	CLOCK_50,
	GPIO_0_D,
	GPIO_1_D,
	KEY
);

//=======================================================
//  PARAMETER declarations
//=======================================================
localparam RED = 8'b111_000_00;
localparam GREEN = 8'b000_111_00;
localparam BLUE = 8'b000_000_11;

//=======================================================
//  PORT declarations
//=======================================================

//////////// CLOCK - DON'T NEED TO CHANGE THIS //////////
input 		          		CLOCK_50;

//////////// GPIO_0, GPIO_0 connect to GPIO Default //////////
output 		    [33:0]		GPIO_0_D;
//////////// GPIO_0, GPIO_1 connect to GPIO Default //////////
input 		    [33:20]		GPIO_1_D;
input 		     [1:0]		KEY;

///// PIXEL DATA /////
reg [7:0]	pixel_data_RGB332 ;
///// READ/WRITE ADDRESS /////
reg [14:0] X_ADDR = 0;
reg [14:0] Y_ADDR = 0;
wire [14:0] WRITE_ADDRESS;
reg [14:0] READ_ADDRESS; 

assign WRITE_ADDRESS = X_ADDR + Y_ADDR*(`SCREEN_WIDTH);

///// VGA INPUTS/OUTPUTS /////
wire 			VGA_RESET;
wire [7:0]	VGA_COLOR_IN;
wire [9:0]	VGA_PIXEL_X;
wire [9:0]	VGA_PIXEL_Y;
wire [7:0]	MEM_OUTPUT;
wire			VGA_VSYNC_NEG;
wire			VGA_HSYNC_NEG;
reg			VGA_READ_MEM_EN;
reg  [7:0]  PIXEL_COLOR;

assign GPIO_0_D[5] = VGA_VSYNC_NEG;
assign VGA_RESET = ~KEY[0];

///// I/O for Img Proc /////
wire [5:0] RESULT;

/* WRITE ENABLE */
reg W_EN;


///////* CREATE ANY LOCAL WIRES YOU NEED FOR YOUR PLL *///////

///////* INSTANTIATE YOUR PLL HERE *///////
///////* INSTANTIATE YOUR PLL HERE *///////
sweetPLL	sweetPLL_inst (
	.inclk0 ( CLOCK_50 ),
	.c0 ( c0_sig ), // 24 MHz
	.c1 ( c1_sig ), // 25 MHz
	.c2 ( c2_sig ) // 50 MHz
	);

///////* M9K Module *///////
Dual_Port_RAM_M9K mem(
	.input_data(pixel_data_RGB332),
	.w_addr(WRITE_ADDRESS),
	.r_addr(READ_ADDRESS),
	.w_en(W_EN),
	.clk_W(c2_sig),
	.clk_R(c1_sig),
	.output_data(MEM_OUTPUT)
);

///////* VGA Module *///////
VGA_DRIVER driver (
	.RESET(VGA_RESET),
	.CLOCK(c1_sig),
	//.PIXEL_COLOR_IN(COLOR),
	.PIXEL_COLOR_IN(VGA_READ_MEM_EN ? MEM_OUTPUT : BLUE),
	.PIXEL_X(VGA_PIXEL_X),
	.PIXEL_Y(VGA_PIXEL_Y),
	.PIXEL_COLOR_OUT({GPIO_0_D[9],GPIO_0_D[11],GPIO_0_D[13],GPIO_0_D[15],GPIO_0_D[17],GPIO_0_D[19],GPIO_0_D[21],GPIO_0_D[23]}),
   .H_SYNC_NEG(GPIO_0_D[7]),
   .V_SYNC_NEG(VGA_VSYNC_NEG)
);

///////* Image Processor *///////
IMAGE_PROCESSOR proc(
	.PIXEL_IN(MEM_OUTPUT),
	.CLK(c1_sig),
	.VGA_PIXEL_X(VGA_PIXEL_X),
	.VGA_PIXEL_Y(VGA_PIXEL_Y),
	.VGA_VSYNC_NEG(VGA_VSYNC_NEG),
	.RESULT(RESULT)
);


assign GPIO_0_D[25] = RESULT[0];
assign GPIO_0_D[27] = RESULT[1];
assign GPIO_0_D[29] = RESULT[2];
assign GPIO_0_D[31] = RESULT[3];
assign GPIO_0_D[33] = RESULT[4];
assign GPIO_0_D[32] = RESULT[5];


reg [2:0] red;   // 3 bits
reg [2:0] green; // 3 bits
reg [1:0] blue;  // 2 bits
wire [7:0] color;
assign color[7:5] = red;
assign color[4:2] = green;
assign color[1:0] = blue;

// Downsampling variables
assign D0 = GPIO_1_D[20];
assign D1 = GPIO_1_D[21];

assign D2 = GPIO_1_D[22];
assign D3 = GPIO_1_D[23];
assign D4 = GPIO_1_D[24];
assign D5 = GPIO_1_D[25];
assign D6 = GPIO_1_D[26];

assign D7 = GPIO_1_D[27];
assign PCLK = GPIO_1_D[28];
assign HREF = GPIO_1_D[29];
assign VSYNC = GPIO_1_D[30];

assign GPIO_0_D[0] = c0_sig;



always @ (VGA_PIXEL_Y, VGA_PIXEL_X) begin
		READ_ADDRESS = (VGA_PIXEL_X + VGA_PIXEL_Y*`SCREEN_WIDTH);
		if(VGA_PIXEL_X>(`SCREEN_WIDTH-1) || VGA_PIXEL_Y>(`SCREEN_HEIGHT-1))begin
				VGA_READ_MEM_EN = 1'b0;
		end
		else begin
				VGA_READ_MEM_EN = 1'b1;
		end
end

reg flag = 0;
reg reach2byte = 0; //not in use?
assign GPIO_0_D[1] = flag;
reg is_image_started = 1'b1;
reg is_new_row = 1'b0;
reg is_new_byte = 1'b1;
reg [7:0] COLOR = BLUE; //not in use?

reg prev_href = 1'b0;
reg prev_vsync = 1'b0;


always @ (posedge PCLK) begin
   // Handling when an image frame starts or ends
   if (VSYNC & ~HREF & ~prev_href & ~prev_vsync) begin // Image TX on falling edge started

		  X_ADDR = 0;
	     Y_ADDR = 0;
		  flag = 1'b0; //reset flag


		  
	end
	
	// Handle when Camera sends a new row of images
	else begin
		
   	if (~HREF & prev_href) begin // row TX ends on falling edge, must be a new row (HREF == 1'b0)
		     X_ADDR = 0;
		     Y_ADDR = Y_ADDR + 1; 
	   end
	   else if (HREF) begin  // new row TX on rising edge

			 if (~flag) begin
			     W_EN = 1'b0;

				  flag = 1'b1;
				  X_ADDR = X_ADDR;
				  Y_ADDR = Y_ADDR;
				   //red 3bit
			     pixel_data_RGB332[7:5] = {D3, D2, D1};//{D3, D2, D1};//{1'b0, 1'b0, 1'b0};///{D3, D2, D1};{1'b1, 1'b1, 1'b1};
			 end
			 else begin

				  flag = 1'b0;
				  
				  pixel_data_RGB332[4:0] ={D7, D6, D3, D2, D1}; /// {D7, D7,D7, D3, D2};{D7, D6, D5, D3, D1};//{1'b0, 1'b0, 1'b0,1'b0,1'b0}; // Now write the color to memory (since write address has been setup)
					X_ADDR = X_ADDR + 1;
				  Y_ADDR = Y_ADDR;

			end 
		 end
	end
	X_ADDR = X_ADDR;
	Y_ADDR = Y_ADDR;
	prev_href = HREF;
	prev_vsync = VSYNC;
	W_EN = 1'b1;
end

endmodule 
~~~
