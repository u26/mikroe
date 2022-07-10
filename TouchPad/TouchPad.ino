
#include <Wire.h>
#include "mtch6102.h"

#include <string>

MTCH6102 mtch;

int old_x;
int old_y;

enum ST_TOUCH {
  PRESSED = 0,
  RELEASED,
};

ST_TOUCH st_touch = PRESSED;

void setup() {

  uint8_t data;

  Serial.begin(115200);
  delay(500);
//
  mtch = MTCH6102();
  mtch.begin( (uint8_t)0x25, 6);

  mtch.setMode( MTCH6102_MODE_FULL);
//  mtch.setMode( MTCH6102_MODE_TOUCH);
  
//  mtch.writeRegister(MTCH6102_NUMBEROFXCHANNELS, 0x09);
//  mtch.writeRegister(MTCH6102_NUMBEROFYCHANNELS, 0x06);
//  mtch.writeRegister(MTCH6102_MODE, MTCH6102_MODE_FULL);
//  mtch.writeRegister(HORIZONTALSWIPEDISTANCE, 0x04);
//  mtch.writeRegister(MINSWIPEVELOCITY, 0x02);
//  mtch.writeRegister(TAPDISTANCE, 0x02);
//  mtch.writeRegister(BASEPOSFILTER, 0x00);
//  mtch.writeRegister(BASENEGFILTER, 0x00);
//  mtch.writeRegister(MTCH6102_CMD, 0x20);

  mtch.showCoreRamMemoryMap();
}

void loop() {

  char buf[64];
  byte data;

  std::string gesture = "";
  data = mtch.readReg(GESTURESTATE);
  
  if (data != 0) {
    switch(data){
      case LEFT:
        gesture = "left";
        break;
      case RIGHT:      
        gesture = "right";
        break;
      case UP:      
        gesture = "up";
        break;
      case DOWN:      
        gesture = "down";
        break;
      case CLICK:      
        gesture = "click";
        break;
      case DOUBLE_CLICK:      
        gesture = "dclick";
        break;
      default:
        break;
    }
//    char buf[64];
//    sprintf(buf, "{\"gesture\":\"%s\"}", gesture.c_str());
//    Serial.println(buf);

  }

// TOUCHSTATE
  data = mtch.readReg(0x10);
//  Serial.print("TOUCHSTATE: ");
//  Serial.println(data,HEX);

  if(data & 0x01){

    uint16_t x,y = 0;
    x = mtch.readReg(0x11);
    //  Serial.print("TOUCHX: ");
    //  Serial.println(x,HEX);
    
    y = mtch.readReg(0x12);
    //  Serial.print("TOUCHY: ");
    //  Serial.println(y,HEX);
    
    // TOUCHLSB
    uint8_t lsb = mtch.readReg(0x13);
    //  Serial.print("TOUCHLSB: ");
    //  Serial.println(data,HEX);
    
    x |= (uint16_t)(0xf0 & lsb) >> 8;
    y |= (uint16_t)(0x0f & lsb);
    //  Serial.print("x: "); Serial.print(x);
    //  Serial.print("  y: "); Serial.println(y);


    if( st_touch == RELEASED ){
      
      st_touch = PRESSED;
      old_x = x;
      old_y = y;
    
    }else if( st_touch == PRESSED ){

      int dx = x - old_x;
      int dy = y - old_y;
      old_x = x;
      old_y = y;
      
      sprintf(buf, "{\"x\":%d,\"y\":%d}", -dx, -dy);
      Serial.println(buf);
    }

/*
    if( old_x==0 && old_y==0 ){
      
      old_x = x;
      old_y = y;
      
    }else{

      int dx = x-old_x;
      int dy = y-old_y;
      old_x = x;
      old_y = y;

      char buf[64];
      sprintf(buf, "{\"x\":%d,\"y\":%d, \"gesture\":\"%s\"}", dx, dy, gesture.c_str());
      Serial.println(buf);

    }
    */
  }else{

    st_touch = RELEASED;
    sprintf(buf, "{\"x\":%d,\"y\":%d}", 0, 0);
    Serial.println(buf);
  }

  delay(20);
}
