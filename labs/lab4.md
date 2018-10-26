
# Lab 4: FPGA and Shape Detection
## Teams

**Team 1**: (Arduino) Michael and Siming

**Team 2**: (FPGA) Marcela, Zoe and Natan

## Objectives

* Detect basic shapes froma camera input
* Send camera information to the Arduino


# Arduino Team

Michael and Siming 

### Materials used:
* DEO-Nano FPGA
* Arduino Uno
* OV7670 digital camera

### Goals
The goals for the Arduino team are wiring the camera, setting up its registers with the Arduino, and reading in treasure data from the FPGA. 

[Hardware connections](https://snag.gy/nLMzXN.jpg)

### Setting up 24 MHz clock and PLL
The first thing we implemented was the 24 MHz clock using the PLL introduction in the lab description.  After initializing the PLL and inserting the module into the main block of code, we connected the 50 MHz clock into the input of the module, and connected a wire from the 24 MHz output to GPIO pin 6.  Following the hardware spec sheet, we were able to find the according pin on the FPGA and measure the output from the clock signal, as shown below.  By doing this, we were able to confirm the 24 MHz signal.

![24 MHz clock output](https://i.imgur.com/Fbc30YR.png)

### Communicating with the FPGA
How to encode the what we see in the camera to the Arduino. 
camera connects to GPIO, 8 outputs, feeds straight

a block to strip away bits

a block to detect treasure

### Wiring the camera
To wire the camera, we needed the Arduino and the FPGA.  The Arduino would set up the camera and initialize the registers over I2C protocol using the SCL and SDA pins (A4 and A5).  The FPGA would receive the camera data and also provide a 24 MHz clock.  These are detailed in the schematic provided:

![Hardware](https://cei-lab.github.io/ece3400-2018/images/Lab3CameraWiringDiagram.png)

Following the schematic (SIOC <-> SCL, SIOD <-> SDA, MCLK <-> XCLK), we then completed our wiring.  The 10k resistors are pullups to 3.3 V and we made sure to disable the internal 5V pullups on the Arduino to ensure that the FPGA would be receiving the correct voltage.  After the TAs checked our work, we continued to the next step.  Below is our wiring:

![Wiring the camera](https://i.imgur.com/8RRD6vV.jpg?1)

### Software
To test the register setup, we used the provided Arduino code with some of our own modifications.  The first thing we had to find was the slave write register number.  From the data sheet, we found that its address number is 0x42, or 01000010 in binary.  However, Arduino's wire library automatically appends the last bit depending on if it is trying to read or write so we removed the least significant bit, giving 00100001, or 0x21, which we defined as OV7670_I2C_ADDRESS.  

After this, we then implemented a readback command to check that all the registers had been written to correctly.  To do this, we added a single line of code in the end of the write command:

~~~
Serial.println(read_register_value(start));
~~~

This let us check the registers and as shown below, we were able to write them correctly as the hexcode matches our serial output data.

![Success!](https://i.imgur.com/fgq4QV8.png)

### Data Structure for Sending/Receiving FPGA Information
Next, we set up our data structure for how the FPGA would communicate what the camera sees to the Arduino, as diagrammed below.  We chose using 3 digital pins: the first two would represent the presence of a treasure and its shape, and the last bit would communicate the color of the treasure. 

![data structure](https://i.imgur.com/RdsITlB.png)

Using this structure, we can communicate all cases for the treasure:

| 000         | 001         | 010          | 011           | 100        | 101         | 110         | 111          |
|-------------|-------------|--------------|---------------|------------|-------------|-------------|--------------|
| no treasure | no treasure | red triangle | blue triangle | red square | blue square | red diamond | blue diamond |

### Voltage Divider

![Voltage Divider](https://i1.wp.com/randomnerdtutorials.com/wp-content/uploads/2015/09/voltage-divider-circuit.png?resize=408%2C151&ssl=1)

### Code

# FPGA team

Marcela, Zoe, and Natan

### Materials Used:

* DEO- Nano FPGA
* VGA adapter and cord

### Goals

The goals for the FPGA team are to write test data into the board's embedded memory and connect it to the VGA driver, to display it on the computer monitor. 

### PLL

Like the Arduino team we also followed the PLL instructions before starting the lab and connected each clock line to the module that needed it: 24MHz for the camera, 25MHz for the VGA module and RAM's read cycles, and 50MHz for RAM's write cycles.

### Setup

To test the display, we assigned the WRITE_ADDRESS wire the value of READ_ADDRESS+1. We decided to deal with any failing edge cases this could entail after testing our connection to the VGA driver worked and the monitor displayed our test data as desired. We set the input data to the RAM to always be red in RGB 332, i.e. 8'b111_000_00. We also set W_EN, the write-enable signal for the RAM, to be high only while the address requested by the VGA driver was inside the pixel range, defined by the parameters SCREEN_HEIGHT and SCREEN_WIDTH, using the provided loop that changes the VGA_READ_MEM_EN (see code below).

#### Test WRITE_ADDRESS and RAM input data assignments
```assign WRITE_ADDRESS = 1'd1 + READ_ADDRESS;```
```///// PIXEL DATA /////
reg [7:0]	pixel_data_RGB332 = 8'b111_000_00;
```

#### Setting W_EN
```
///////* Update Read & Write Addresses *///////
always @ (VGA_PIXEL_X, VGA_PIXEL_Y) begin
		READ_ADDRESS = (VGA_PIXEL_X + VGA_PIXEL_Y*`SCREEN_WIDTH);
		//WRITE_ADDRESS = (1'd1 + VGA_PIXEL_X + VGA_PIXEL_Y*`SCREEN_WIDTH);
		if(VGA_PIXEL_X>(`SCREEN_WIDTH-1) || VGA_PIXEL_Y>(`SCREEN_HEIGHT-1))begin
				VGA_READ_MEM_EN = 1'b0;
				W_EN = 1'b0;
		end
		else begin
				VGA_READ_MEM_EN = 1'b1;
				W_EN =  1'b1;
		end
end
```


