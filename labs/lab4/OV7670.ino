#include <Wire.h>

#define OV7670_I2C_ADDRESS 0x21 /*TODO: write this in hex (eg. 0xAB) */

void set_color_matrix(){
    Serial.println(OV7670_write_register(0x4f, 0x80));
    
    
    OV7670_write_register(0x12, 0x80); //reset registers
    
//    OV7670_write_register(0x12, 0x07); //color bar test
//    OV7670_write_register(0x42, 0x08); //color bar test

    OV7670_write_register(0x12, 0x04); // RGB output format
    OV7670_write_register(0x3e, 0x08); //scaling parameter can be adjusted

    OV7670_write_register(0x11, 0xc0); //internal clock is external clock
    OV7670_write_register(0x1e, 0x00); //vertical flip and mirror

    OV7670_write_register(0x40, 0xd0); // 565 Output
    
    
    //provided registers:
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


///////// Main Program //////////////
void setup() {
  Wire.begin();
  Serial.begin(9600);
  
  // TODO: READ KEY REGISTERS
  
  delay(100);
  
  // TODO: WRITE KEY REGISTERS
  
  //read_key_registers();

  set_color_matrix();
}

void loop(){
 }


///////// Function Definition //////////////
void read_key_registers(){
  /*TODO: DEFINE THIS FUNCTION*/
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
    
    Serial.println(read_register_value(start));
    
    return "no errors :)";
 }

String OV7670_write_register(int reg_address, byte data){
  return OV7670_write(reg_address, &data, 1);
 }
