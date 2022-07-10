
#ifndef MTCH6102_H
#define MTCH6102_H

#include "Arduino.h"
#include <Wire.h>

// The default I2C address
#define MTCH6102_I2CADDR_DEFAULT   0x25

#define MTCH6102_FWMAJOR           (uint8_t)0x00
#define MTCH6102_FWMINOR           (uint8_t)0x01
#define MTCH6102_APPIDH            (uint8_t)0x02
#define MTCH6102_APPIDL            (uint8_t)0x03
#define MTCH6102_CMD               (uint8_t)0x04
#define MTCH6102_MODE              (uint8_t)0x05
#define MTCH6102_MODE_TOUCH   0010
#define MTCH6102_MODE_FULL    0011
#define MTCH6102_MODE_GESTURE 0001
#define MTCH6102_MODE_RAW     0100
#define MTCH6102_MODE_STANDBY 0000

#define MTCH6102_NUMBEROFXCHANNELS 0x20
#define MTCH6102_NUMBEROFYCHANNELS 0x21
#define MTCH6102_TOUCHSTATE        0x10
#define MTCH6102_TOUCHX            0x11
#define MTCH6102_TOUCHY            0x12
#define MTCH6102_TOUCHLSB          0x13

#define GESTURESTATE                0x14
#define CLICK           (uint8_t)0x10 
#define CLICK_HOLD      (uint8_t)0x11 
#define DOUBLE_CLICK    (uint8_t)0x20 
#define DOWN            (uint8_t)0x31 
#define DOWN_HOLD       (uint8_t)0x32 
#define RIGHT           (uint8_t)0x41 
#define RIGHT_HOLD      (uint8_t)0x42 
#define UP              (uint8_t)0x51 
#define UP_HOLD         (uint8_t)0x52 
#define LEFT            (uint8_t)0x61 
#define LEFT_HOLD       (uint8_t)0x62 

#define GESTUREDIAG                 0x15

#define BASEPOSFILTER               0x2F
#define BASENEGFILTER               0x30
#define BASEFILTERTYPE              0x33
#define BASEFILTERSTRENGTH          0x34


#define HORIZONTALSWIPEDISTANCE     0x37
#define VERTICALSWIPEDISTANCE       0x38
#define SWIPEHOLDBOUNDARY           0x39
#define TAPDISTANCE                 0x3A
#define DISTANCEBETWEENTAPS         0x3B
#define TAPHOLDTIMEL                0x3C
#define TAPHOLDTIMEH                0x3D
#define GESTURECLICKTIME          0x3E
#define SWIPEHOLDTHRESH           0x3F
#define MINSWIPEVELOCITY          0x40
#define HORIZONTALGESTUREANGLE    0x41
#define VERTICALGESTUREANGLE      0x42

#define MTCH6102_SENSORVALUE_RX0   0x80
#define MTCH6102_SENSORVALUE_RX1   0x81
#define MTCH6102_SENSORVALUE_RX2   0x82
#define MTCH6102_SENSORVALUE_RX3   0x83
#define MTCH6102_SENSORVALUE_RX4   0x84
#define MTCH6102_SENSORVALUE_RX5   0x85
#define MTCH6102_SENSORVALUE_RX6   0x86
#define MTCH6102_SENSORVALUE_RX7   0x87
#define MTCH6102_SENSORVALUE_RX8   0x88
#define MTCH6102_SENSORVALUE_RX9   0x89
#define MTCH6102_SENSORVALUE_RX10  0x8A
#define MTCH6102_SENSORVALUE_RX11  0x8B
#define MTCH6102_SENSORVALUE_RX12  0x8C
#define MTCH6102_SENSORVALUE_RX13  0x8D
#define MTCH6102_SENSORVALUE_RX14  0x8E

class MTCH6102 {
  public:
  
    MTCH6102(void);
    
    boolean begin(uint8_t i2caddr = MTCH6102_I2CADDR_DEFAULT, uint8_t reset_pin = 6);
    
    uint8_t readReg(uint8_t reg);
    void writeReg(uint8_t reg, uint8_t value);

    void reset();
    void setMode(uint8_t mode);
    void showCoreRamMemoryMap();
    
  private:
    uint8_t _i2caddr;
    uint8_t _reset_pin;
};

#endif // MTCH6102_H
