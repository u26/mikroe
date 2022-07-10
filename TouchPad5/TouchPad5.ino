
#include <Wire.h>

#include "iqs550.h"


//// Snap buffer size
//#define SNAP_BUFFER_SIZE                     ( 8 )
//
//// Number of read channels for snap status
//#define SNAP_NUMBER_OF_CHANNELS              ( 8 )
//
//// Snap detection condition
//#define SNAP_MAX_NO_CONSECUTIVE_SAMPLES      ( 3 )

// Touchpad strength threshold for XY touch detection



#define ST_INIT 0
#define ST_RUN 1
int st_app = ST_INIT;  

IQS550* iqs;
IQS550::position_t delta;

void setup() {

  iqs = new IQS550(7, 6);
  
  Wire.begin();
  Wire.setClock(400000);
  
  Serial.begin(115200);
  delay(1000);
  
  iqs->setup();
}

void loop() {

  IQS550::touchpad5_touch_t t;
  iqs->getTouch( &t );
  iqs->calcTouchDelta( &t, &delta );
  delay(20);
}
