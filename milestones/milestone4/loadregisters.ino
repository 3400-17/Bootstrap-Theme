#include <Wire.h>


 

///////// Main Program //////////////

void setup() {

  Wire.begin();

  Serial.begin(9600);

  Serial.println("Beginning");

 

  OV7670_write_register(COM7 , 0x80);

  delay(100);

  set_color_matrix();
    OV7670_write_register(0x12, 0x80); //COM7, reset registers, QCIF format, RGB
    OV7670_write_register(0x12, 0x0e); //COM7, reset registers, QCIF format, RGB
    /OV7670_write_register(0x0c, 0x08); //COM3, enable scaling
    OV7670_write_register(0x3e, 0x08); //COM14, scaling parameter can be adjusted
    OV7670_write_register(0x14, 0x01); //COM9, automatic gain ceiling, freeze AGC/AEC
    OV7670_write_register(0x40, 0xd0); //COM15, 565 Output
    OV7670_write_register(0x42, 0x0c); //COM17, color bar test c/0
    OV7670_write_register(0x11, 0xc0); //CLKRC, internal clock is external clock
    OV7670_write_register(0x1e, 0x30); //vertical flip and mirror enabled 

   OV7670_write_register(0x8c , 0x02);

  //OV7670_write_register(0x01, 0x90);//adjust blue gain
  //OV7670_write_register(0xBE, 0x90);//black level comp (blue)
  //OV7670_write_register(0x55, 0x30);//brightness?
  read_key_registers();

}

 

void loop(){

}

 

 

///////// Function Definition //////////////

void write_key_registers(){


}

void read_key_registers(){ 

}

 

byte read_register_value(int register_address){

  byte data = 0;

  Wire.beginTransmission(OV7670_I2C_ADDRESS);

  Wire.write(register_address);

  Wire.endTransmission();

  Wire.requestFrom(OV7670_I2C_ADDRESS,1);

  while(Wire.available()<1);

  data = Wire.read();

  return data;

}

 

String OV7670_write(int start, const byte *pData, int size){

    int n,error;

    Wire.beginTransmission(OV7670_I2C_ADDRESS);

    n = Wire.write(start);

    if(n != 1){

      return "I2C ERROR WRITING START ADDRESS";  

    }

    n = Wire.write(pData, size);

    if(n != size){

      return "I2C ERROR WRITING DATA";

    }

    error = Wire.endTransmission(true);

    if(error != 0){

      return String(error);

    }

    return "no errors :)";

}

 

String OV7670_write_register(int reg_address, byte data){

  return OV7670_write(reg_address, &data, 1);

}

 

void set_color_matrix(){

    OV7670_write_register(0x4f, 0x80);

    OV7670_write_register(0x50, 0x80);

    OV7670_write_register(0x51, 0x00);

    OV7670_write_register(0x52, 0x22);

    OV7670_write_register(0x53, 0x5e);

    OV7670_write_register(0x54, 0x80);

    OV7670_write_register(0x56, 0x40);

    OV7670_write_register(0x58, 0x9e);

    OV7670_write_register(0x59, 0x88);

    OV7670_write_register(0x5a, 0x88);

    OV7670_write_register(0x5b, 0x44);

    OV7670_write_register(0x5c, 0x67);

    OV7670_write_register(0x5d, 0x49);

    OV7670_write_register(0x5e, 0x0e);

    OV7670_write_register(0x69, 0x00);

    OV7670_write_register(0x6a, 0x40);

    OV7670_write_register(0x6b, 0x0a);

    OV7670_write_register(0x6c, 0x0a);

    OV7670_write_register(0x6d, 0x55);

    OV7670_write_register(0x6e, 0x11);

    OV7670_write_register(0x6f, 0x9f);

    OV7670_write_register(0xb0, 0x84);

}
