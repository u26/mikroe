

#include "MTCH6102.h"

MTCH6102::MTCH6102() {}

boolean MTCH6102::begin(uint8_t i2caddr, uint8_t reset_pin) {

  pinMode(reset_pin,OUTPUT);

  Wire.begin();
  Wire.setClock(400000);
  
  _i2caddr = i2caddr;
  _reset_pin = reset_pin;

  reset();

  return true;  
}

void MTCH6102::reset() {

  digitalWrite(_reset_pin, LOW);
  delay(100);
  digitalWrite(_reset_pin, HIGH);
}


uint8_t MTCH6102::readReg(uint8_t reg) {

    uint8_t dt=0;
    
    Wire.beginTransmission(_i2caddr);
    Wire.write(reg); // register to read
    Wire.endTransmission(false); 

    Wire.requestFrom(_i2caddr, 1, false); // read a uint8_t
    if (Wire.available()) {
        dt = Wire.read();
    }
    Wire.endTransmission(true); 
    return dt;
  
//  Wire.beginTransmission(MTCH6102_I2CADDR_DEFAULT);
//  Wire.write(reg); // register to read
//  Wire.endTransmission();
//  
//  // read a byte
//  while (Wire.requestFrom(MTCH6102_I2CADDR_DEFAULT, 1) != 1);
//  return ( Wire.read());
}

void MTCH6102::writeReg(uint8_t reg, uint8_t value) {

  Wire.beginTransmission(_i2caddr);
  Wire.write((uint8_t)reg); // register to read
  Wire.write((uint8_t)(value));
  Wire.endTransmission();
}

void MTCH6102::setMode(uint8_t mode) {

  writeReg(MTCH6102_MODE, mode);
}

void MTCH6102::showCoreRamMemoryMap() {

  uint8_t data = 0;
  data = readReg(MTCH6102_FWMAJOR);
  Serial.print("MAJOR: ");
  Serial.println(data,HEX);

  data = readReg(MTCH6102_FWMINOR);
  Serial.print("MINOR: ");
  Serial.println(data,HEX);

  data = readReg(MTCH6102_APPIDH);
  Serial.print("APPIDH: ");
  Serial.println(data,HEX);

  data = readReg(MTCH6102_APPIDL);
  Serial.print("APPIDL: ");
  Serial.println(data,HEX);

  data = readReg(MTCH6102_APPIDL);
  Serial.print("APPIDL: ");
  Serial.println(data,HEX);

  data = readReg(MTCH6102_CMD);
  Serial.print("CMD: ");
  Serial.println(data,HEX);
  
  data = readReg(MTCH6102_MODE_TOUCH);
  Serial.print("MODE: ");
  Serial.println(data,HEX);
}
