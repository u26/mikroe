#include <Wire.h>

#include "capslider2.h"

//-------------
// Library
//-------------
// ArduinoSTL


//#define __DEBUG__ 1

int pin_clr = 6;
int pin_rdy = 7;
uint8_t slave_addr = CAPSLDR2_DEVICE_ADDR;

uint32_t wheel_avrg1;
uint32_t wheel_avrg2;
uint16_t out_val;
uint8_t out_mode;
uint8_t cnt;

#define LIMIT_PRESS   100
#define EVENT_RELEASE 0x00
#define EVENT_PRESS   0x01
int event = 0;

#define ST_RELEASE 0x00
#define ST_PRESS   0x01
int st_gesture = ST_RELEASE;


#define LIMIT_PROX    300
#define LIMIT_TOP      60
#define LIMIT_RIGHT    60
#define LIMIT_BOTTOM   60
#define LIMIT_LEFT     60
#define LIMIT_CENTER  100
#define LIMIT_FRIC 5



#define EVT_PRESS (uint8_t)0x01
uint8_t touch_event = 0;

#define T_TOP     (uint8_t)0x01
#define T_RIGHT   (uint8_t)0x02
#define T_BOTTOM  (uint8_t)0x04
#define T_LEFT    (uint8_t)0x08
#define T_CENTER  (uint8_t)0x10
//#define T_TAP     (uint8_t)0x20
uint8_t tap_code = 0;

#define ST_TAP_IDLE     (uint8_t)0x01
#define ST_TAP_PRESS    (uint8_t)0x02
#define ST_TAP_LONG     (uint8_t)0x04
#define ST_TAP_RELEASE  (uint8_t)0x08

uint8_t st_tap = ST_TAP_IDLE;
long click_st_time = 0;


#define ST_FRIC_IDLE    0
#define ST_FRIC_PRESS   1
#define ST_FRIC_DETECT  2
int st_fric = ST_FRIC_IDLE;

int fric_cnt[4];
long st_time = 0;

int16_t rc_val[8] = {0};
int16_t sens_val[8] = {0};
int16_t val_old[8] = {0};
char sbuf[128]={0};


uint8_t capsldr2_write_reg (uint8_t reg, uint8_t *data_buf, uint8_t len, boolean _stop=true )
{
    while(digitalRead(pin_rdy));   
    
    Wire.beginTransmission(slave_addr);
    Wire.write( reg );
    for(int i=0; i< len; i++){
      Wire.write(data_buf[i]);
    }
    Wire.endTransmission(_stop); 

//    capslider2_generic_write( register_address, data_in, n_bytes );
    delay(30);
    return CAPSLDR2_OK;
}


uint8_t capsldr2_read_reg( uint8_t reg, uint8_t *rx_buf, uint8_t len, boolean _stop=false )
{
    while(digitalRead(pin_rdy));

    uint8_t reg_addr;

    Wire.beginTransmission(slave_addr);
    Wire.write( reg );
    Wire.endTransmission(false); 

    do{
      Wire.requestFrom(slave_addr, len, _stop);
    }while(Wire.available() == 0);
    //Load bytes into array
    int i=0;
    while(Wire.available()){
      rx_buf[i] = Wire.read();
      i++;
    }

//    delay(30);
    return CAPSLDR2_OK;
}



void setup() {

  Serial.begin(115200);
  delay(500);

  //
  // setup IQS333
  //
  capslider2_init();
  delay(500);

  capsldr2_reset();
  delay(500);

//  GetDeviceInfo();

    uint8_t data[ 10 ] = { 0 };
    
    data[0] = 0x20; // Projection mode
    capsldr2_write_reg( CAPSLDR2_SYSTEM_FLAGS_REG, 
                        data, 
                        1 );

    data[0] = CAPSLDR2_CH0_PROX_EN | 
              CAPSLDR2_CH1_EN |   // left
              CAPSLDR2_CH2_EN |   // center
              CAPSLDR2_CH5_EN |   // center
              CAPSLDR2_CH3_EN |   // right
              CAPSLDR2_CH4_EN |   // bottom
              CAPSLDR2_CH6_EN;    // top
    data[1] = 0x00;
    capsldr2_write_reg( CAPSLDR2_ACTIVE_CHANN_REG, 
                        data, 
                        2 );

    data[0] = 0x04;
    data[1] = 0x10;
    data[2] = 0x10;
    data[3] = 0x10;
    data[4] = 0x10;
    data[5] = 0x10;
    data[6] = 0x10;
    data[7] = 0x10;
    data[8] = 0x10;
    data[9] = 0x10;
    capsldr2_write_reg( CAPSLDR2_THRESH_REG, 
                        data, 
                        10 );
    
    data[0] = 0x80;
    data[1] = 0x40;
    capsldr2_write_reg( CAPSLDR2_TARGETS_REG, 
                        data, 
                        2 );

    data[0] = 0x80;
    data[1] = 0xc0;
    data[2] = 0x80;
    data[3] = 0xC0;
    data[4] = 0xC0;
    data[5] = 0x80;
    data[6] = 0xC0;
    data[7] = 0x0;
    data[8] = 0x0;
    data[9] = 0x0;
    capsldr2_write_reg( CAPSLDR2_MULTIPLIERS_REG, 
                        data, 
                        10 );                        

    data[0] = 0x16;
    data[1] = 0x20;
    data[2] = 0x48;
    data[3] = 0x80;
    data[4] = 0x07;
    data[5] = 0x7F;
    capsldr2_write_reg( CAPSLDR2_PROXSETT_REG, 
                        data, 
                        6 );                        
    data[0] = 0x0;
    data[1] = 0x0;
    data[2] = 0x0;
    data[3] = 0x0;
    data[4] = 0x0;
    data[5] = 0x0;
    data[6] = 0x0;
    data[7] = 0x0;
    data[8] = 0x0;
    data[9] = 0x0;
    capsldr2_write_reg( CAPSLDR2_COMPENS_REG, 
                        data, 
                        10 );    

    data[0] = 0x50;
    data[1] = 0x00;
    data[2] = 0x10;
    data[3] = 0x02;
    data[4] = 0x02;
    capsldr2_write_reg( CAPSLDR2_TIMINGS_REG, 
                        data, 
                        5 );    

//    do
//    {
//        delay(10);  // wait for IQS333 to finish conversions, then check ATI bit
//        capsldr2_read_reg( CAPSLDR2_SYSTEM_FLAGS_REG, data, (uint8_t)1, true);
//    }
//    while ((data[0] & 0b00000100) == 0b00000100);

delay(4000);
}

void GetDeviceInfo(){

  while(digitalRead(pin_rdy)); 
  
  uint8_t rbuf[16] = {0};

  // DEVICE_INFO
  //
  Serial.println("DEVICE_INFO");
  capsldr2_read_reg( 
    CAPSLDR2_DEVICE_INFO_REG, 
    rbuf,
    CAPSLDR2_DEV_INFO_NBYTES, 
    true);
  delay(1); 
  Serial.print("Product Number : ");Serial.println(rbuf[0]);
  Serial.print("Version Number : ");Serial.println(rbuf[1]);

  // CAPSLDR2_SYSTEM_FLAGS
  //
  Serial.println("_SYSTEM_FLAGS");  
  capsldr2_read_reg( 
    CAPSLDR2_SYSTEM_FLAGS_REG, 
    rbuf,
    CAPSLDR2_SYST_FLAGS_NBYTES, 
    true);

  for(int i=0; i<CAPSLDR2_SYST_FLAGS_NBYTES; i++){
    Serial.print(rbuf[i], HEX);
    Serial.print(" ");
  }
  Serial.println("");

  // CAPSLDR2_PROXSETT_REG
  //
  Serial.println("PROXSETT");  
  while(digitalRead(pin_rdy));
  memset(rbuf, 0x00, 16); 
  capsldr2_read_reg( 
    CAPSLDR2_PROXSETT_REG, rbuf,
    CAPSLDR2_PROXSETT_NBYTES, 
    true);

  for(int i=0; i<CAPSLDR2_PROXSETT_NBYTES; i++){
    Serial.print(rbuf[i], HEX);
    Serial.print(" ");
  }
  Serial.println("");


  Serial.println("THRESH_REG");  
//  while(digitalRead(pin_rdy));
  memset(rbuf, 0x00, 16); 
  capsldr2_read_reg( 
    CAPSLDR2_THRESH_REG, rbuf,
    CAPSLDR2_THRESH_NBYTES, 
    true);

  for(int i=0; i<CAPSLDR2_THRESH_NBYTES; i++){
    Serial.print(rbuf[i], HEX);
    Serial.print(" ");
  }
  Serial.println("");

  // CAPSLDR2_COMPENS_REG
  //
  Serial.println("CAPSLDR2_COMPENS_REG");  
//  while(digitalRead(pin_rdy)); 
  capsldr2_read_reg( CAPSLDR2_COMPENS_REG, rbuf,(uint8_t)10, true);
  for(int i=0; i<10; i++){
    Serial.print(rbuf[i], HEX);
    Serial.print(" ");
  }
  Serial.println("");


  // CAPSLDR2_MULTIPLIERS_REG
  //
  Serial.println("CAPSLDR2_MULTIPLIERS_REG");  
//  while(digitalRead(pin_rdy)); 
  capsldr2_read_reg( CAPSLDR2_MULTIPLIERS_REG, rbuf,(uint8_t)10, true);
  for(int i=0; i<10; i++){
    Serial.print(rbuf[i], HEX);
    Serial.print(" ");
  }
  Serial.println("");
 
}

void capslider2_init(){

  Wire.begin();
  Wire.setClock(400000);

  //pinMode( pin_rdy, INPUT_PULLUP);
  pinMode( pin_rdy, INPUT);  
  pinMode( pin_clr, OUTPUT);
  digitalWrite(pin_clr, HIGH);

}

void capsldr2_reset(){

  // CLR
  delay(200);
  digitalWrite(pin_clr, LOW);
  delay(200);
  digitalWrite(pin_clr, HIGH);
  delay(1000);

}

uint8_t capsldr2_enable_chann(uint16_t sel_chann){

    uint8_t chann_data[ 2 ] = { 0 };
    chann_data[0] = sel_chann & 0xff;
    chann_data[1] = 0;
    
    capsldr2_write_reg( CAPSLDR2_ACTIVE_CHANN_REG, 
                        chann_data, 
                        CAPSLDR2_ACT_CHANN_NBYTES );
    return CAPSLDR2_OK;
}

void capsldr2_config()
{
  uint8_t PROXSETT_data[CAPSLDR2_PROXSETT_NBYTES] = { 0x00,0x00,0x00, 0x0F,0x07,0x7f};
  capsldr2_write_reg( CAPSLDR2_PROXSETT_REG, 
                        PROXSETT_data, 
                        CAPSLDR2_PROXSETT_NBYTES,
                        true);

//    uint8_t compensation[ 10 ] = { 69, 106, 96, 102, 102, 113, 101, 40, 44, 41 };
//    uint8_t mult[ 10 ] = { 0xF3, 0x95, 0x93, 0x97, 0x95, 0x92, 0x95, 0x01, 0x01, 0x01 };
//    uint8_t mult[ 10 ]   = { 0x9f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x01, 0x01, 0x01, 0x01 };
    uint8_t mult[ 10 ]   = { 0x9f, 0x9f, 0x9f, 0x9f, 0x7f, 0x7f, 0x0, 0x0, 0x00, 0x00 };

//    uint8_t target[ 10 ] = { 128,  128,  128,  128,  128, 128, 0x0 , 0x0 , 0x0 , 0x0   };
  uint8_t target[ 10 ] = { 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x0 , 0x0 , 0x0 , 0x0   };
    uint8_t compensation[ 10 ] = {0,0,0,0,0,0, 0, 0, 0, 0 };

    uint8_t thresh_data[10] = {0x04,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10};
    capsldr2_write_reg( CAPSLDR2_THRESH_REG, 
                        thresh_data, 
                        10 );

    capsldr2_write_reg( CAPSLDR2_TARGETS_REG, 
                        target, 
                        2,
                        true);

    capsldr2_write_reg( CAPSLDR2_COMPENS_REG, 
                        compensation, 
                        10, 
                        true );

    capsldr2_write_reg( CAPSLDR2_MULTIPLIERS_REG, 
                        mult, 
                        CAPSLDR2_MULTIPL_NBYTES,
                        true);
}

void capsldr2_set_threshold(uint8_t thresh_value)
{
    uint8_t thresh_data[ 10 ];
    uint8_t cnt;

    for (cnt = 0; cnt < 10; cnt++){
        thresh_data[ cnt ] = thresh_value;
    }

    capsldr2_write_reg( CAPSLDR2_THRESH_REG, 
                        thresh_data, 
                        CAPSLDR2_THRESH_NBYTES );
}

void touch_bytes(){

  uint8_t buf[2] = {0};

  capsldr2_read_reg( CAPSLDR2_TOUCH_BYTES_REG, 
                     buf, 
                     2,
                     true);
                     
  Serial.print( buf[0], HEX);
  Serial.print( " ");
  Serial.println( buf[1], HEX);
}

void capsldr2_get_data( uint16_t *wheel_data1, uint16_t *wheel_data2 )
{
    uint8_t touch_buff[ 4 ];
    uint16_t ret_val;
    
    capsldr2_read_reg( CAPSLDR2_WHEEL_COORDNT_REG, 
                       touch_buff, 
                       CAPSLDR2_WHEEL_NBYTES );

    ret_val = touch_buff[ 1 ];
    ret_val <<= 8;
    ret_val |= touch_buff[ 0 ];

    *wheel_data1 = ret_val;
    
    ret_val = touch_buff[ 3 ];
    ret_val <<= 8;
    ret_val |= touch_buff[ 2 ];

    *wheel_data2 = ret_val;
}

void capsldr2_set_output ( uint8_t input_data, uint8_t output_mode )
{
    uint8_t output_data[ 8 ];
    uint8_t count;
    static uint8_t intensity = CAPSLDR2_PWM_DUTY_100_PERCENTS;
    static uint8_t n_leds = 0;

    if (output_mode == CAPSLDR2_LED_INTENSITY){

        intensity = input_data * 3.875;
        if (intensity > CAPSLDR2_PWM_DUTY_100_PERCENTS){
            intensity = CAPSLDR2_PWM_DUTY_100_PERCENTS;
        }
    }else{
        n_leds = input_data;
    }
    
    for (count = 0; count < 8; count++){
        if (count < n_leds){
            output_data[count] = intensity;
        }else{
            output_data[count] = CAPSLDR2_PWM_DUTY_0_PERCENTS;
        }
        output_data[count] |= CAPSLDR2_PWM_CONST;
    }

    capsldr2_write_reg( CAPSLDR2_PWM_DUTY_REG, 
                        output_data, 
                        CAPSLDR2_PWM_DUTY_NBYTES );
}

typedef enum {
  FRIC_UP = 1,
  FRIC_DOWN,
  FRIC_RIGHT,
  FRIC_LEFT
} FRIC_DIR;

uint8_t tkeys[2] = {0};
int tidx = 0;
typedef struct{
  
  FRIC_DIR fric;
  uint8_t code[5];
  
}FRIC_PATTERN;

//#define PATTERN_NUM 8

FRIC_PATTERN pattern[] = {

  // in to out
  {FRIC_UP,    T_BOTTOM, T_CENTER},
  {FRIC_LEFT,  T_RIGHT,  T_CENTER},
  {FRIC_RIGHT, T_LEFT,   T_CENTER},
  {FRIC_DOWN,  T_TOP,    T_CENTER},
  // out to in
  {FRIC_UP,    T_CENTER, T_TOP},
  {FRIC_LEFT,  T_CENTER, T_LEFT},
  {FRIC_RIGHT, T_CENTER, T_RIGHT},
  {FRIC_DOWN,  T_CENTER, T_BOTTOM},
  //out to out
  {FRIC_DOWN,  T_TOP, T_BOTTOM},
  {FRIC_DOWN,  T_RIGHT, T_BOTTOM},
  {FRIC_DOWN,  T_RIGHT, T_BOTTOM},

  {FRIC_UP,    T_BOTTOM, T_TOP},
  {FRIC_UP,    T_RIGHT, T_TOP},
  {FRIC_UP,    T_LEFT, T_TOP},
  
  {FRIC_RIGHT, T_BOTTOM, T_RIGHT},
  {FRIC_RIGHT, T_TOP, T_RIGHT},
  {FRIC_RIGHT, T_LEFT, T_RIGHT},
  
  {FRIC_LEFT,  T_BOTTOM, T_LEFT},
  {FRIC_LEFT,  T_TOP, T_LEFT},
  {FRIC_LEFT,  T_RIGHT, T_LEFT},

};

uint8_t getKey( int16_t* val ){
  
  int sz = sizeof(pattern) / sizeof(FRIC_PATTERN);
  uint8_t code = 0;
  memset(sbuf,0,128);
  sprintf(sbuf, 
          "{prox:%d, ch1:%d, ch2:%d, ch3:%d, ch4:%d, ch5:%d, ch6:%d}",
          val[0],
          val[1],
          val[2],
          val[3],
          val[4],
          val[5],
          val[6]);
//  Serial.println(sbuf);
 
//  if( val[0] >= LIMIT_PROX ){
//    touch_event |= EVT_PRESS;
//  }else{
//    touch_event = 0;
//  }

  if( val[1] >= LIMIT_TOP ){
      code |= T_TOP;
//      Serial.print(sbuf);
//      Serial.println("TOP");
  }       
  else if( val[6] >= LIMIT_RIGHT ){
      code |= T_RIGHT;
//      Serial.print(sbuf);
//      Serial.println("RIGHT");
  }       
  else if( val[3] >= LIMIT_BOTTOM ){
      code |= T_BOTTOM;
//      Serial.print(sbuf);
//      Serial.println("BOTTOM");
  }       
  else if( val[4] >= LIMIT_LEFT ){
      code |= T_LEFT;
//      Serial.print(sbuf);
//      Serial.println("LEFT");
  }
  else if( val[2] >= LIMIT_CENTER &&
           val[5] >= LIMIT_CENTER){  
      code |= T_CENTER;
//      Serial.print(sbuf);
//      Serial.println("CENTER");
  }



  // FRIC 
  if( touch_event == 0 ){
    
    if(code > 0 ){
      if( val[0] >= LIMIT_PROX ){
        
        touch_event |= EVT_PRESS;
        tkeys[0] = code;
        tkeys[1] = code;
        
//        Serial.print("[PRESS]");
//        Serial.println(tkeys[0], HEX);
      }
    }

  }else{
    
    if( val[0] >= LIMIT_PROX ){
      
      if(code > 0){
        tkeys[1] = code;
//        Serial.println(tkeys[1],HEX);
      }
    }
    else{

      touch_event = 0;
//      sprintf(sbuf, 
//              "[RELEASE] %x %x}",tkeys[0], tkeys[1]);
//      Serial.println(sbuf);
      int detect = 0;
      for(int i=0; i<sz; i++){
  
        if( pattern[i].code[0] == tkeys[0] &&
            pattern[i].code[1] == tkeys[1]){
            
          switch( pattern[i].fric){
            case FRIC_UP:
              Serial.println("[FRIC] UP");
              break;
            case FRIC_DOWN:
              Serial.println("[FRIC] DOWN");
              break;
            case FRIC_LEFT:
              Serial.println("[FRIC] LEFT");
              break;
            case FRIC_RIGHT:
              Serial.println("[FRIC] RIGHT");
              break;
            default:
              break;
          }
          detect = 1;
          break;
        }
      }
      
      if(detect == 0){
        if( tkeys[1] == tkeys[0]){
          switch( tkeys[0] ){
            case T_TOP:
              Serial.println("[TAP] UP");
              break;
            case T_BOTTOM:
              Serial.println("[TAP] BOTTOM");
              break;
            case T_LEFT:
              Serial.println("[TAP] LEFT");
              break;
            case T_RIGHT:
              Serial.println("[TAP] RIGHT");
              break;
            case T_CENTER:
              Serial.println("[TAP] CENTER");
              break;
            default:
              break;
          }
        }
      }
      
      tkeys[0] = 0;
      tkeys[1] = 0;
    }
  }

  
  return code;
}



void Counts(){

  //Read Counts
  while(digitalRead(pin_rdy)); 
  uint16_t Counts[8] = {0};
  uint16_t LTA[8] = {0};
  uint8_t rbuf[16] = {0};
  float A = 0.8;
  
  // Counts
  capsldr2_read_reg( CAPSLDR2_COUNTS_REG,  rbuf, 16, false);
  Counts[0] = (rbuf[1] << 8) + rbuf[0];
  Counts[1] = (rbuf[3] << 8) + rbuf[2];
  Counts[2]  = (rbuf[5] << 8) + rbuf[4];
  Counts[3]  = (rbuf[7] << 8) + rbuf[6];
  Counts[4] = (rbuf[9] << 8) + rbuf[8];
  Counts[5] = (rbuf[11] << 8) + rbuf[10];
  Counts[6] = (rbuf[13] << 8) + rbuf[12];
  Counts[7] = (rbuf[15] << 8) + rbuf[14];  

  // LTAs  
  capsldr2_read_reg( CAPSLDR2_LTA_REG,  rbuf, 16, true);
  delay(1);
  LTA[0] = (rbuf[1] << 8) + rbuf[0];
  LTA[1] = (rbuf[3] << 8) + rbuf[2];
  LTA[2] = (rbuf[5] << 8) + rbuf[4];
  LTA[3] = (rbuf[7] << 8) + rbuf[6];
  LTA[4] = (rbuf[9] << 8) + rbuf[8];
  LTA[5] = (rbuf[11] << 8) + rbuf[10];
  LTA[6] = (rbuf[13] << 8) + rbuf[12];
  LTA[7] = (rbuf[15] << 8) + rbuf[14];  

  //Calculate Deltas
  for(int i=0; i<8; i++){
    sens_val[i] = LTA[i] - Counts[i];
    sens_val[i]*=-1;
//    rc_val[i] = LTA[i] - Counts[i];
//    rc_val[i] *=-1;
//    sens_val[i] = A*sens_val[i] + (1-A)*(float)rc_val[i];
  }

//  Serial.println(sens_val[0]);
  if( sens_val[0] >= LIMIT_PRESS ){
    event = EVENT_PRESS;
  }else{
    event = EVENT_RELEASE;
  }

#ifdef __DEBUG__    
  if( sens_val[0] >= LIMIT_PRESS ){

    memset(sbuf,0,128);
    sprintf(sbuf, 
            "{prox:%d, ch1:%d, ch2:%d, ch3:%d, ch4:%d, ch5:%d, ch6:%d}",
            sens_val[0],
            sens_val[1],
            sens_val[2],
            sens_val[3],
            sens_val[4],
            sens_val[5],
            sens_val[6]);
    Serial.println(sbuf);

  }
#endif

}

#define PRESS_OFF 0
#define PRESS_ON  1
#define PRESS_WAIT  2

int st_press_sens = PRESS_OFF;
long timer_press=0;

void getPress(){
  
  int ret=0;
  int press_val = analogRead(A0);

  switch(st_press_sens){
    
    case PRESS_OFF:

      if(press_val > 70){

        memset(sbuf,0,128);
        sprintf(sbuf, 
                  "[Press] %d",
                  press_val);
        Serial.println(sbuf);
        
        st_press_sens = PRESS_ON;
  
        tkeys[0] = 0;
        tkeys[1] = 0;
      }
      break;
      
    case PRESS_ON:

      if(press_val < 5){
         Serial.println("[RELEASE]");
        st_press_sens = PRESS_OFF;
      }

//      if(press_val < 5){
//        st_press_sens = PRESS_WAIT;
//        timer_press = millis();
//      }
      break;
      
    case PRESS_WAIT:
      if( millis()-timer_press > 300){
        st_press_sens = PRESS_OFF;
      }
      break;      
    default:
      break;
  }
}



void loop() {

  getPress();
  Counts();
  getKey(sens_val);

#ifndef __DEBUG__ 
//  if( event == EVENT_PRESS ){
//  uint8_t code = getKey(sens_val);
//  }
#endif

//  delay(10);
}
