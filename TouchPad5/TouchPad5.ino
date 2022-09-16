
#include <Wire.h>
#include <Vector.h>

#include "iqs550.h"

//-------------
// DEBUG FLAG
//-------------
//#define DEBUG_FLICK 1


//-------------
// variables
//-------------

#define ST_INIT 0
#define ST_RUN 1
int st_app = ST_INIT;  
char sbuf[128]={0};

IQS550* iqs;
IQS550::position_t delta;
IQS550::position_t old_pos;

int st_flick = ST_RELEASED;

// lowpass filter(RC Filter)
//a = 1/(2π･fc･dt + 1)      fc [Hz]  dt[s] sampling cycle
#define a 0.8     // 0 < a < 1.0(strong)
float rc_x=0;          
float rc_y=0;          

#define PT_BUF_SIZE 64
IQS550::position_t pt_buf[PT_BUF_SIZE];
Vector<IQS550::position_t> pts(pt_buf);

#define FRIC_LIMIT 100  // delta xy
#define CLICK_LIMIT 100 // delta xy
#define CLICK_TIME 500 // ms
#define FRIC_TIME 200  // ms

IQS550::position_t pt_sum;
IQS550::position_t pt_move;
IQS550::position_t pt_release;
int release_idx = 0;
long release_dt = 0;
long st_time;
long click_dt;



#define PRESS_OFF 0
#define PRESS_ON  1
#define PRESS_WAIT  2
int st_press_sens = PRESS_OFF;
long timer_press=0;

#define PRESS_LIMIT   100 //70
#define RELEASE_LIMIT 50 //5

//-------------
// functions
//-------------
void getPress();
void flick_main( IQS550::touchpad5_touch_t* t );
void flic_calc();

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
  // iqs->getTouch( &t );
  // iqs->calcTouchDelta( &t, &delta );

  getPress();
  
  if(st_press_sens == PRESS_OFF){

    iqs->getTouch( &t );
    flick_main( &t );
  }
  delay(10);
}


void flick_clear(){

  pts.clear();
  st_flick = ST_RELEASED;
}

void flick_main( IQS550::touchpad5_touch_t* t ){
  
  int16_t dx=0;
  int16_t dy=0;

  switch( st_flick ){

    case ST_RELEASED:

      if( t->x_pos != 0 && 
          t->y_pos != 0){
        
        if( t->touch_strength > TOUCH_THRESHOLD ){
  
          st_flick = ST_PRESSED;
          old_pos.x = t->x_pos;
          old_pos.y = t->y_pos;
          old_pos.strength = t->touch_strength;

          pts.clear();
          st_time = millis();
        }
      }
      break;

    case ST_PRESSED:

      if( t->touch_strength < TOUCH_THRESHOLD){

        st_flick = ST_RELEASED;
        old_pos.x = 0;
        old_pos.y = 0;
        old_pos.strength = 0;
        click_dt = millis() - st_time;
        
        flic_calc();

        pts.clear();

      }else{

        if(t->xy_info & TOUCHPAD5_NOISE_STATUS){
          // pass through 
        }else{

          // === ATTENTION ===
          // change x <-> y for board direction.
          //
          dy = t->x_pos - old_pos.x;
          dx = t->y_pos - old_pos.y;
          // === ATTENTION ===
          //  dx = t->x_pos - old_pos.x;
          //  dy = t->y_pos - old_pos.y;

          // RC Filter
          rc_x = a *rc_x + (1-a) * (float)dx;
          rc_y = a * rc_y + (1-a) * (float)dy;

          dx = rc_x; 
          dy = rc_y; 

          old_pos.x = t->x_pos;
          old_pos.y = t->y_pos;

          if( pts.size() >= PT_BUF_SIZE ){
            pts.remove(0);
          }

          delta.x = dx;
          delta.y = dy;
          delta.strength = t->touch_strength;
          delta.t = millis();
          pts.push_back(delta);
        }
      }
      break;

    default:
      break;    
  }

  // debug
//  sprintf(
//    sbuf, 
//    "{\"x\":%d,\"y\":%d,\"strength\":%d,\"status\":%d }", 
//    dx, 
//    dy, 
//    t->touch_strength, 
//    st_flick);
//    
//  Serial.println(sbuf);     

}

void flic_calc(){

  pt_sum.x = 0;
  pt_sum.y = 0;
  pt_move.x = 0;
  pt_move.y = 0;
  pt_release.x=0;
  pt_release.y=0;
  
  int sz = pts.size()-1;
  int max_strength = 0;
  release_idx = sz;

  for(int i=sz; i>=0; i--){
    
    int s = pts[i].strength;
    if( max_strength < s ){
      max_strength = s;
    }else{
      release_idx = i+1;
#if DEBUG_FLICK
      sprintf(
        sbuf, 
        "MAX: %d  max_id: %d", 
        max_strength,
        release_idx) ;
      Serial.println(sbuf);
#endif      
      break;
    }
  }
  
  for(int i=release_idx; i<=sz; i++){
    
    pt_release.x += pts[i].x;
    pt_release.y += pts[i].y;
    
#if DEBUG_FLICK
    sprintf(
      sbuf, 
      "release x: %d  y: %d strength: %d", 
       pts[i].x,
       pts[i].y,
       pts[i].strength
       );
    Serial.println(sbuf);
#endif      
  }

  release_dt = pts[sz].t - pts[release_idx].t;

#if DEBUG_FLICK
  sprintf(
    sbuf, 
    "RELEASE x: %d  y: %d  delta: %d", 
     pt_release.x,
     pt_release.y,
     release_dt
     );
  Serial.println(sbuf);
#endif
  
  int st_pos = release_idx - 5;
  if( st_pos < 0 ){
    st_pos=0;
  }
  for(int i=st_pos; i < release_idx; i++){
      pt_move.x += pts[i].x;
      pt_move.y += pts[i].y;
  }

#if DEBUG_FLICK
  sprintf(
    sbuf, 
    "MOVE x: %d  y: %d", 
     pt_move.x,
     pt_move.y );
  Serial.println(sbuf);
#endif
  
  for(int i=0; i<=sz; i++){
    pt_sum.x += pts[i].x;
    pt_sum.y += pts[i].y;
  }

#if DEBUG_FLICK
  sprintf(
    sbuf, 
    "DELTA x: %d  y: %d", 
     pt_sum.x,
     pt_sum.y );
  Serial.println(sbuf);
#endif

  float vec = sqrt(pow(pt_sum.x,2) + pow(pt_sum.y,2));

#if DEBUG_FLICK
  sprintf(
    sbuf, 
    "vector %f", 
     vec);
  Serial.println(sbuf);
#endif

  if( abs(pt_release.x) > FRIC_LIMIT || 
      abs(pt_release.y) > FRIC_LIMIT ){

    if( release_dt < FRIC_TIME ){
      
      int deg = atan2(pt_sum.y, pt_sum.x);
      deg = degrees(deg) * -1;
      
      if( (deg > 135 && deg <= 180)||(deg < -135 && deg >= -180) ){
        Serial.println("[FRIC] LEFT");
      }
      else if((deg >= -45 && deg < 45)){
        Serial.println("[FRIC] RIGHT");
      }
      else if(deg >= 45 && deg < 135){
        Serial.println("[FRIC] UP");
      }
      else if(deg < -45 && deg > -135){
        Serial.println("[FRIC] DOWN");
      }
      else{
        Serial.println("[FRIC] ERROR");
      }
    }else{
        Serial.println("[FRIC] unknown");
    }

  }else{
    
//    if( abs(pt_sum.x) < CLICK_LIMIT && 
//        abs(pt_sum.y) < CLICK_LIMIT &&
//        click_dt < CLICK_TIME){
//
//        Serial.println("[TAP]");
//    }else{
//        Serial.println("[UNKNOWN] ");
//    }
  }

}



void getPress(){
  
  int ret=0;
  int press_val = analogRead(A0);
  
//    memset(sbuf,0,128);
//    sprintf(sbuf, 
//              "%d ",
//              press_val);
//    Serial.println(sbuf);

  switch(st_press_sens){
    
    case PRESS_OFF:

      if(press_val > PRESS_LIMIT){

        Serial.println("[PRESS]");
        st_press_sens = PRESS_ON;
      }
      break;
      
    case PRESS_ON:

//      if(press_val < RELEASE_LIMIT){
//         Serial.println("[RELEASE]");
//        st_press_sens = PRESS_OFF;
//        flick_clear();
//      }

      if(press_val < RELEASE_LIMIT){
        
        Serial.println("[RELEASE]");
        st_press_sens = PRESS_WAIT;
        timer_press = millis();
        flick_clear();
      }
      break;
      
    case PRESS_WAIT:
    
      if( millis() - timer_press > 300){
        st_press_sens = PRESS_OFF;
      }
      break;      
    default:
      break;
  }
}
