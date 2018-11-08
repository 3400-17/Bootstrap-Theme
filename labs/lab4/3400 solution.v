always @(posedge PCLK) begin
	if(VSYNC & ~last_sync) begin //posedge vsync
		Y = 10'b0;
		X = 10'b0;
		CYCLE = 1'b0;
	end
	else if (~HREF & last_href) begin //negedge href
		Y = Y + 10'b1;
		X = 10'b0;
		CYCLE = 1'b0;
	end
	else begin 
		Y = Y;
		if (HREF) begin
			if (CYCLE == 1'b0) begin
				TEMP[7:0] = DATA; //444
				CYCLE = 1'b1;
				W_EN = 1'b0;
				X=X;
				PIXEL_COLOR[7:5] = DATA [3:1];
			end
			else begin
				TEMP[15:8] = DATA; //444
				PIXEL_COLOR = {TEMP[15:13], TEMP[11:9], TEMP[3:2]};
				W_EN = 1'b1;
				CYCLE = 1'b0;
				X = X + 1'b1;
			end
		end else begin
			X = 10'b0;
		end
	end
	last_sync = VSYNC;
	last_href = HREF
end
endmodule 



///////////// PORT DECLARATIONS/////////////
input 	[7:0] 	PIXEL_IN;
input		CLK;

input	[9:0]	VGA_PIXEL_X;
input 	[9:0] 	VGA_PIXEL_Y
input		VSYNC;
input		HREF;

output	[2:0] 	RESULT;
reg		RESULT;
reg	[15:0]	countBLUE;
reg	[15:0]	countRED;
reg	[15:0]	countNULL;
reg	[15:0]	R_CNT_THRESHOLD = 16'd20000;
reg	[15:0]	B_CNT_THRESHOLD = 16'd20000;
reg lastsync = 1'b0;

always @(posedge CLK) begin
	if (HREF) begin
		if (PIXEL_IN == 8'b0) begin countBLUE = countBLUE + 16'd1; end
		else if (PIXEL_IN[7:5] > 3'b010) begin countRED = countRED + 16'b1; end
		else begin countNULL = countNULL + 16'd1; end
	end
	if (VSYNC == 1'b1 && lastsync == 1'b0) begin //posedge VSYNC
		if (countBLUE > B_CNT_THRESHOLD) begin RESULT = 3'b111; end
		else if (countRED > R_CNT_THRESHOLD) begin RESULT = 3'b110; end
		else begin RESULT = 3'b000; end
	if (VSYNC == 1'b0 && lastsync == 1'b1) begin //negedge VSYNC
		countBLUE = 16'b0;
		countRED = 16'b0;
		countNULL = 16'b0;
	end
	lastsync = VSYNC
end
 
endmodule
