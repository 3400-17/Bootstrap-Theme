`define SCREEN_WIDTH 176
`define SCREEN_HEIGHT 144
`define NUM_BARS 3
`define BAR_HEIGHT 48

module IMAGE_PROCESSOR (
	PIXEL_IN,
	CLK,
	VGA_PIXEL_X,
	VGA_PIXEL_Y,
	VGA_VSYNC_NEG,
	RESULT
);


//=======================================================
//  PORT declarations
//=======================================================
input	[7:0]	PIXEL_IN;
input 		CLK;

input [9:0] VGA_PIXEL_X;
input [9:0] VGA_PIXEL_Y;
input			VGA_VSYNC_NEG;

output [5:0] RESULT;
reg [5:0] reg_result;
assign RESULT = reg_result;

reg [15:0] countBLUE; 
reg [15:0] countRED; 
reg [15:0] countNULL; 
reg [2:0] R_THRESHOLD = 3'b100;
reg [2:0] B_THRESHOLD = 3'b100;
reg [15:0] FRAME_THRESHOLD = 16'd15000;
reg [15:0] R_CNT_THRESHOLD = 16'd6000; 
reg [15:0] B_CNT_THRESHOLD = 16'd6000; 	// 6000 for square (bigger area), 3000 for smaller triangle (smaller area)
reg lastsync = 1'b0; 
reg [7:0] lastY = 8'b0; //not used?

// Blue edge Points
reg [7:0] BLUE_LINE_0 = 8'b0;
reg [7:0] BLUE_LINE_1 = 8'b0;
reg [7:0] BLUE_LINE_2 = 8'b0;
reg [7:0] BLUE_CNT_LINE = 8'b0;
// Red edge Points
reg [7:0] RED_LINE_0 = 8'b0;
reg [7:0] RED_LINE_1 = 8'b0;
reg [7:0] RED_LINE_2 = 8'b0;
reg [7:0] RED_CNT_LINE = 8'b0;

reg BLUE_T;
reg RED_T;
reg BLUE_D;
reg RED_D;
reg BLUE_S;
reg RED_S;

always @(posedge CLK) begin 
	if(VGA_VSYNC_NEG) begin 
		// If the camera is read as RGB323 (For more blue value) No need for green
		// PIXEL_IN[2:0] = Blue
		// PIXEL_IN[7:5] = Red
	
		// The difference of comparing [blue] with [red + 2'b10] and [red + 2'b01] is that [red + 2'b10] might exceed 3 bit. Thus, blue (3 bits) will always be larger than 4'b1000 or larger, 
		//	Blue usually has dark blue instead of bright, therefore the blue value threshold has to be smaller
		if(PIXEL_IN[1:0] > 2'b01 && PIXEL_IN[1:0] > (PIXEL_IN[7:5] + 3'b001)  ) begin 	// && PIXEL_IN[2:0] > (PIXEL_IN[7:5] + 2'b10)
			countBLUE = countBLUE + 16'd1; 
			
			if ( VGA_PIXEL_Y == 6'd42) begin
				BLUE_LINE_0 = BLUE_LINE_0 + 1'b1;
				end
			if ( VGA_PIXEL_Y == 7'd72) begin
				BLUE_LINE_1 = BLUE_LINE_1 + 1'b1;
				end
			if ( VGA_PIXEL_Y == 7'd102) begin
				BLUE_LINE_2 = BLUE_LINE_2 + 1'b1;
				end
				
			end 
		else begin 
			countNULL = countNULL + 16'd1; 
			end
		
		// Usually, we have brighter red, so the red value threshold is higher.
		if(PIXEL_IN[7:5] > 3'b001 && PIXEL_IN[7:5] > (PIXEL_IN[1:0] + 2'b01) ) begin 
			countRED = countRED + 16'd1; 
			
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
		else begin 
			countNULL = countNULL + 16'd1; 
			end 
			
		
		/*--------------------- Testing Treasure Shape -------------------------*/
		// reg_result[2] : Triangle
		// reg_result[1] : Square
		// reg_result[0] : Diamand
		BLUE_D = BLUE_LINE_1 > BLUE_LINE_0 && BLUE_LINE_1 > BLUE_LINE_2;
		RED_D = RED_LINE_1 > RED_LINE_0 && RED_LINE_1 > RED_LINE_2;
		
		BLUE_T = BLUE_LINE_1 > BLUE_LINE_0 && BLUE_LINE_2 > BLUE_LINE_1;
		RED_T = RED_LINE_1 > RED_LINE_0 && RED_LINE_2 > RED_LINE_1;
		
		//BLUE_S = BLUE_LINE_2 > BLUE_LINE_0 && BLUE_LINE_2 > BLUE_LINE_1;
		//RED_S = RED_LINE_2 > RED_LINE_0 && RED_LINE_2 > RED_LINE_1;
 		
		if (BLUE_LINE_0 > 5'd12 && BLUE_LINE_1 > 5'd22 && BLUE_LINE_2 > 5'd22 ) begin
			if (BLUE_D) begin
				reg_result[0] = 1'b1; //DIAMOND
				reg_result[1] = 1'b0; 
				reg_result[2] = 1'b0;
				end
			else if (BLUE_T) begin
				reg_result[0] = 1'b0; 
				reg_result[1] = 1'b0;
				reg_result[2] = 1'b1; //T	
				end
			else begin
				reg_result[0] = 1'b0; 
				reg_result[1] = 1'b1; //S
				reg_result[2] = 1'b0;	
				end
		end
		else if (RED_LINE_0 > 5'd12 && RED_LINE_1 > 5'd22 && RED_LINE_2 > 5'd22 ) begin
			if (RED_D) begin
				reg_result[0] = 1'b1; //DIAMOND
				reg_result[1] = 1'b0; 
				reg_result[2] = 1'b0;
				//reg_result[3] = 1'b1; //RED
				//reg_result[4] = 1'b0; 
				//reg_result[5] = 1'b0;
				end
			else if (RED_T) begin
				reg_result[0] = 1'b0; 
				reg_result[1] = 1'b0;
				reg_result[2] = 1'b1; //T
				//reg_result[3] = 1'b1; //RED
				//reg_result[4] = 1'b0; 
				//reg_result[5] = 1'b0;		
				end
			else begin
				reg_result[0] = 1'b0; 
				reg_result[1] = 1'b1; //S
				reg_result[2] = 1'b0;
				//reg_result[3] = 1'b1; //RED
				//reg_result[4] = 1'b0;
				//reg_result[5] = 1'b0;		
				end
		end
		else 
				reg_result[0] = 1'b0; 
				reg_result[1] = 1'b0;
				reg_result[2] = 1'b0;
				//reg_result[3] = 1'b0;
				//reg_result[4] = 1'b0;
				//reg_result[5] = 1'b1; //null	
		end
		
		/*-------------------- Testing Treasure Shape -------------------------*/
		
		
	//end ?
	
	if(VGA_VSYNC_NEG == 1'b0 && lastsync == 1'b1) begin //negedge VSYNC 
		
		/*----------------------- Testing color ---------------------------*/
		if(countRED > countBLUE+ 16'd4000 && countRED>countNULL && countRED>16'd4000) begin //FRAME_THRESHOLD > countRED && 
			reg_result[3] <= 1'b1; //isRed
			reg_result[4] <= 1'b0;
			reg_result[5] <= 1'b0;
			end 
		else if(countBLUE > countRED-16'd1000  && countBLUE>countNULL-16'd1000) begin 
			reg_result[4] <= 1'b1; //isBlue
			reg_result[3] <= 1'b0;
			reg_result[5] <= 1'b0;
			
			end 
	//// Even if countBLUE is bigger than B_CNT_THRESHOLD, the reg_result[2] is still set to true. 
		else if(countNULL > countBLUE +16'd100 && countNULL > countRED + 16'd100) begin 
			reg_result[5] <= 1'b1; //null
			reg_result[3] <= 1'b0;
			reg_result[4] <= 1'b0;
			end 
//		else begin
//			reg_result[4] <= 1'b1; //isBlue
//			reg_result[3] <= 1'b0;
//			reg_result[5] <= 1'b0;
		//end
		/*----------------------- Testing color ---------------------------*/
		
		countBLUE = 16'b0; 
		countRED = 16'b0; 
		countNULL = 16'b0; 
		//reg_result = 3'b0;
		BLUE_LINE_0 = 8'b0;
		BLUE_LINE_1 = 8'b0;
		BLUE_LINE_2 = 8'b0;
		RED_LINE_0 = 8'b0;
		RED_LINE_1 = 8'b0;
		RED_LINE_2 = 8'b0;
		end 
	lastsync = VGA_VSYNC_NEG; 
	lastY = VGA_PIXEL_Y;
end 


endmodule
