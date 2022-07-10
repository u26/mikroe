#include <Wire.h>
#include "capslider2.h"


int pin_clr = 6;
int pin_rdy = 7;
uint8_t slave_addr = CAPSLDR2_DEVICE_ADDR;

uint32_t wheel_avrg1;
uint32_t wheel_avrg2;
uint16_t out_val;
uint8_t out_mode;
uint8_t cnt;


#define EVENT_RELEASE 0x00
#define EVENT_PRESS   0x01
int event = 0;

#define ST_RELEASE 0x00
#define ST_PRESS   0x01

int st_gesture = ST_RELEASE;

#define UP    0x01
#define DOWN  0x02
#define LEFT  0x03
#define RIGHT 0x04
#define CENTER  0x10
uint8_t touch = 0;

#define LIMIT 50
int sens_val[8] = {0};
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
  delay(1000);

  //
  // setup IQS333
  //
  capslider2_init();
  delay(500);

  capsldr2_reset();
  delay(500);
  capsldr2_enable_chann(  CAPSLDR2_CH0_PROX_EN | 
                          CAPSLDR2_CH1_EN | 
                          CAPSLDR2_CH2_EN | 
                          CAPSLDR2_CH3_EN |
                          CAPSLDR2_CH4_EN |
                          CAPSLDR2_CH5_EN);

  capsldr2_config();
  delay(1000);

  capsldr2_set_threshold( 0x04 );
  delay(1000);

  GetDeviceInfo();

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



void Counts(){

  //Read Counts
  while(digitalRead(pin_rdy)); 
  uint16_t Counts[8] = {0};
  uint16_t LTA[8] = {0};
  uint8_t rbuf[16] = {0};
  
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
  for(int i=0; i<6; i++){
    sens_val[i] = LTA[i] - Counts[i];
  }


  if( sens_val[0] >= 100 ){
    event = EVENT_PRESS;
  }else{
    event = EVENT_RELEASE;
  }

  if(st_gesture == ST_RELEASE){
    
    if( event == EVENT_PRESS ){
//        Serial.println("[PRESS]");
        st_gesture = ST_PRESS;
    }
    
  }else if(st_gesture == ST_PRESS){

    memset(sbuf,0,128);
    sprintf(sbuf, 
            "{prox:%d, ch1:%d, ch2:%d, ch3:%d, ch4:%d, ch5:%d}",
            sens_val[0],
            sens_val[1],
            sens_val[2],
            sens_val[3],
            sens_val[4],
            sens_val[5]);
    Serial.println(sbuf);
     
    if(event == EVENT_RELEASE){
      st_gesture = ST_RELEASE;
      touch = 0;
    }

#if 0    
    for(int i=0; i<6; i++){
      
      sens_val[i] = LTA[i] - Counts[i];
      Serial.print("[");
      Serial.print(i);
      Serial.print("]");
      Serial.print(sens_val[i]);
      Serial.print(" ");
    }
 
//    if( sens_val[2] >= 20 ){
//      Serial.print("CENTER");
//      touch = 0;
//    }
//    else 
    if( sens_val[4] >= LIMIT && 
        sens_val[5] >= LIMIT ){

      touch = 0;
    }
    else if( sens_val[3] >= LIMIT && 
        sens_val[5] >= LIMIT ){
      touch = UP; 
      Serial.print("UP");
    }
    else if( sens_val[1] >= LIMIT && 
             sens_val[5] >= LIMIT ){
      touch = DOWN; 
      Serial.print("DOWN");
    }
    else if( sens_val[3] >= LIMIT && 
             sens_val[4] >= LIMIT ){
      touch = RIGHT;  
      Serial.print("RIGHT");
    }
    else if( sens_val[1] >= LIMIT && 
             sens_val[4] >= LIMIT ){
      touch = LEFT;  
      Serial.print("LEFT");
    }

    Serial.println("");
    
    if(event == EVENT_RELEASE){
      
      Serial.println("[RELEASE]");
      if(touch == UP){
        Serial.print("UP");
      }else if(touch == DOWN){
        Serial.print("DOWN");
      }else if(touch == RIGHT){
        Serial.print("RIGHT");
      }else if(touch == LEFT){
        Serial.print("LEFT");
      }
      Serial.println("");
      st_gesture = ST_RELEASE;
      touch = 0;
    }
#endif
  }  
}

void loop() {

    uint16_t data_wheel1;
    uint16_t data_wheel2;
    uint8_t ready_check;

  //touch_bytes();

  Counts();

  delay(10);
}


void capslider2_init(){

  Wire.begin();
  Wire.setClock(400000);

  pinMode( pin_rdy, INPUT_PULLUP);
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

void capsldr2_config ( )
{

//    uint8_t cnfg[ 10 ] = { 69, 106, 96, 102, 102, 113, 101, 40, 44, 41 };
//    uint8_t mult[ 10 ] = { 0xF3, 0x95, 0x93, 0x97, 0x95, 0x92, 0x95, 0x01, 0x01, 0x01 };
    uint8_t compensation[ 10 ] = { 128, 128, 128, 128, 128, 128, 0, 0, 0, 0 };
    uint8_t mult[ 10 ]   = { 0x9f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x01 , 0x01 , 0x01 , 0x01   };
    uint8_t target[ 10 ] = { 0xff,  0xff,  0xff,  0xff,  0xff, 0xff, 0x01 , 0x01 , 0x01 , 0x01   };

    capsldr2_write_reg( CAPSLDR2_TARGETS_REG, 
                        mult, 
                        CAPSLDR2_TARGETS_NBYTES,
                        true);

    capsldr2_write_reg( CAPSLDR2_COMPENS_REG, 
                        compensation, 
                        CAPSLDR2_COMPENS_NBYTES, 
                        true );

    capsldr2_write_reg( CAPSLDR2_MULTIPLIERS_REG, 
                        mult, 
                        CAPSLDR2_MULTIPL_NBYTES,
                        true);
}

void capsldr2_set_threshold ( uint8_t thresh_value )
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
