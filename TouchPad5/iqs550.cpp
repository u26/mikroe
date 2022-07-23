#include "iqs550.h"


IQS550::IQS550(int rdy, int rst){

  // RESET
  pin_rdy = rdy;
  pin_rst = rst;
  
//  pinMode( pin_rdy, INPUT_PULLUP);
  pinMode( pin_rdy, INPUT);
  pinMode( pin_rst, OUTPUT);
  //touch5_reset();
}

IQS550::~IQS550(){
    
}


void IQS550::readReg(uint8_t reg, uint8_t* rx_buf, int len, boolean _stop)
{
    // wait RDY
    while(!digitalRead(pin_rdy));
    
    Wire.beginTransmission(_slave_address);
    Wire.write( reg );
    Wire.endTransmission(false); 

    Wire.requestFrom( _slave_address, len, false);
    while( Wire.available() ){
        *rx_buf = Wire.read();
        rx_buf++;
    }
    Wire.endTransmission(_stop); 
}


void IQS550::writeReg(uint8_t reg, uint8_t* tx_buf, int len, boolean _stop)
{
    uint16_t dt=0;

    // wait RDY
    while(!digitalRead(pin_rdy));

    Wire.beginTransmission(_slave_address);
    Wire.write( reg );
    for(int i=0; i< len; i++){
      Wire.write(tx_buf[i]);
    }
    Wire.endTransmission(true); 
//    delay(30);
}

void IQS550::touchpad5_channel_setup() 
{
    uint8_t tx_buf[ 7 ];
    
    tx_buf[ 0 ] = 10;
    tx_buf[ 1 ] = 13;
    tx_buf[ 2 ] = 10;
    tx_buf[ 3 ] = 13;

//    tx_buf[ 4 ] = TOUCHPAD5_CHARGE_TYPE_SELF_CHARG|
//                  TOUCHPAD5_RX_GROUP_RXB;
    tx_buf[ 4 ] = TOUCHPAD5_CHARGE_TYPE_SELF_CHARG;      
//    tx_buf[ 4 ] = 0;            
    tx_buf[ 5 ] = 0x1f;
    tx_buf[ 6 ] = 0xff;
    
   writeReg( TOUCHPAD5_CMD_CHANNEL_SETUP, tx_buf, 7 );
}



void IQS550::touchpad5_control_setup() 
{
    uint8_t tx_buf[ 2 ];
    tx_buf[ 0 ] = TOUCHPAD5_SINGLE_XY|
                TOUCHPAD5_AUTO_ATI;
//    tx_buf[ 0 ] = TOUCHPAD5_SINGLE_XY|
//                  TOUCHPAD5_AUTO_ATI|
//                  TOUCHPAD5_TRACKPAD_RESEED;
    tx_buf[ 1 ] = 0;
//    tx_buf[ 1 ] = TOUCHPAD5_SNAP_EN;

    writeReg( TOUCHPAD5_CMD_CONTROL_SETTINGS, tx_buf, 2 );
}

void IQS550::touchpad5_threshold_setup() 
{
    uint8_t tx_buf[ 8 ];
        
    tx_buf[ 0 ] = 10;
//    tx_buf[ 1 ] = 5;     // multifier
//    tx_buf[ 2 ] = 7;     // shifter
    tx_buf[ 1 ] = 5;     // multifier
    tx_buf[ 2 ] = 7;     // shifter
    tx_buf[ 3 ] = 10;
    tx_buf[ 4 ] = 0;
    tx_buf[ 5 ] = 0;
    
    writeReg( TOUCHPAD5_CMD_THRESHOLD_SETTINGS, tx_buf, 6 );
}

void IQS550::getSnapStatus() 
{
  
  uint8_t buf[26];
  readReg( TOUCHPAD5_CMD_SNAP_STATUS, buf, 26, true );

  uint16_t st[13];
  st[0] =  (((uint16_t)buf[0])<< 8)|buf[1];
  st[1] =  (((uint16_t)buf[2])<< 8)|buf[3];
  st[2] =  (((uint16_t)buf[4])<< 8)|buf[5];
  st[3] =  (((uint16_t)buf[6])<< 8)|buf[7];
  st[4] =  (((uint16_t)buf[8])<< 8)|buf[8];
  st[5] =  (((uint16_t)buf[10])<< 8)|buf[11];
  st[6] =  (((uint16_t)buf[12])<< 8)|buf[13];
  st[7] =  (((uint16_t)buf[14])<< 8)|buf[15];
  st[8] =  (((uint16_t)buf[16])<< 8)|buf[17];
  st[9] =  (((uint16_t)buf[18])<< 8)|buf[19];
  st[10] = (((uint16_t)buf[20])<< 8)|buf[21];
  st[11] = (((uint16_t)buf[22])<< 8)|buf[23];
  st[12] = (((uint16_t)buf[24])<< 8)|buf[25];

  for(int i=1; i<12; i++){
    Serial.println(st[i],HEX);
  }
}


void IQS550::getTouch( touchpad5_touch_t *t ) 
{
    uint8_t rx_buf[ 8 ];
    
    readReg( TOUCHPAD5_CMD_XY_DATA, rx_buf, 8, true );

    t->xy_info = rx_buf[ 0 ];
    t->no_of_fingers = rx_buf[ 0 ] & TOUCHPAD5_NO_OF_FINGERS; 
    t->id_tag = rx_buf[ 1 ];
    t->x_pos = (((uint16_t)rx_buf[2])<< 8) | rx_buf[3];
    t->y_pos = (((uint16_t)rx_buf[4])<< 8) | rx_buf[5];
    t->touch_strength = (((uint16_t)rx_buf[6])<<8)|rx_buf[7];

#if 0
  if( t->x_pos > 0 && 
      t->y_pos > 0 && 
      t->touch_strength > TOUCH_THRESHOLD
    ){

      if(t->xy_info & TOUCHPAD5_NOISE_STATUS){
      }else{
        char buf[64];
        sprintf( buf, 
                 "{\"x\":%d,\"y\":%d,\"strength\":%d}", 
                 t->x_pos, 
                 t->y_pos, 
                 t->touch_strength);
        Serial.println(buf);
      }   
//      Serial.print("x: "); 
//      Serial.print(t->x_pos); 
//      Serial.print("  y: "); 
//      Serial.print(t->y_pos); 
//      Serial.print("  strength: "); 
//      Serial.print(t->touch_strength); 
//      Serial.print("  fingers: "); 
//      Serial.print(t->no_of_fingers); 
//      Serial.print("  info: "); 
//      Serial.println(t->xy_info,HEX); 
//  
//      if(t->xy_info & TOUCHPAD5_NOISE_STATUS){
//        Serial.println("NOISE");
//      }
//      if( t->xy_info & TOUCHPAD5_SHOW_RESET ){
//        Serial.println("RESET"); 
//      }
  }
#endif  
}

char buf[64]={0};
int16_t dx_avg, dy_avg=0;

void IQS550::calcTouchDelta( touchpad5_touch_t *t, position_t* delta ) 
{
  int16_t dx,dy=0;

//  sprintf(buf, "{\"x\":%d,\"y\":%d,\"strength\":%d }",  t->x_pos, t->y_pos, t->touch_strength);
//  Serial.println(buf);

  switch(state){

    case ST_RELEASED:

      if(t->x_pos != 0 && t->y_pos != 0){
        if( t->touch_strength > TOUCH_THRESHOLD ){
            
          state = ST_PRESSED;
          old_pos.x = t->x_pos;
          old_pos.y = t->y_pos;
          dx_avg=0;  
          dy_avg=0;    
        }
      }
      sprintf(buf, "{\"x\":%d,\"y\":%d,\"strength\":%d,\"status\":0 }", dx, -dy, t->touch_strength);
      Serial.println(buf);     
      break;

    case ST_PRESSED:

      if( t->touch_strength < TOUCH_THRESHOLD){

        state = ST_RELEASED;
        old_pos.x = 0;
        old_pos.y = 0;

        sprintf(buf, "{\"x\":%d,\"y\":%d,\"strength\":%d,\"status\":0}", dx, -dy, t->touch_strength);
        Serial.println(buf);     

      }else{

       if(t->xy_info & TOUCHPAD5_NOISE_STATUS){
        // pass through 
       }else{
          dx = t->x_pos - old_pos.x;
          dy = t->y_pos - old_pos.y;

          // AVG
//          dx_avg += dx;
//          dy_avg += dy;
//          dx_avg/=2;
//          dy_avg/=2;
//
//          sprintf(buf, "{\"x\":%d,\"y\":%d,\"strength\":%d,\"status\":1}", dx_avg, -dy_avg, t->touch_strength);
//          Serial.println(buf);        
//
//          dx_avg = dx;
//          dy_avg = dy;

          // RC Filter
          rc_x = a *rc_x + (1-a) * (float)dx;
          rc_y = a * rc_y + (1-a) * (float)dy;
          sprintf(buf, "{\"x\":%d,\"y\":%d,\"strength\":%d,\"status\":1}", (int)rc_x, -(int)rc_y, t->touch_strength);
          Serial.println(buf);        


//  RAW DATA
//          sprintf(buf, "{\"x\":%d,\"y\":%d,\"strength\":%d,\"status\":1}", dx, -dy, t->touch_strength);
//          Serial.println(buf);        
          old_pos.x = t->x_pos;
          old_pos.y = t->y_pos;
       }
      }
      break;

    default:
      break;    
  }

}



void IQS550::touch5_reset()
{
  // RESET
  digitalWrite(pin_rst, LOW);
  delay(200);
  digitalWrite(pin_rst, HIGH);
  delay(200);
}


void IQS550::touchpad5_init()
{
 
  touchpad5_channel_setup();
  delay(500);
  
  // SETTINGS
  uint8_t tx_buf[1]= { TOUCHPAD5_ACK_RESET };
  writeReg(TOUCHPAD5_CMD_CONTROL_SETTINGS,tx_buf, 1, true);
  delay(500);

  touchpad5_control_setup();
  delay(500);

  touchpad5_threshold_setup();
  delay(500);


  uint8_t rx_buf[16] = {0};
  readReg( TOUCHPAD5_CMD_VERSION_INFO, rx_buf, 10, true );
  readReg( TOUCHPAD5_CMD_CONTROL_SETTINGS, rx_buf, 2, true );
  readReg( TOUCHPAD5_CMD_THRESHOLD_SETTINGS, rx_buf, 6, true );
}

void IQS550::setup()
{
  touch5_reset();
  touchpad5_init();
}
