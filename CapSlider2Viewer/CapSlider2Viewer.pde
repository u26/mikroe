import processing.serial.*;
import de.looksgood.ani.*;
import controlP5.*;
Serial serial;      

ControlP5 cp5;
Slider slider[] = new Slider[6];
Slider slider_limit_prox;
Slider slider_limit_center;
Slider slider_limit_ch4;
Slider slider_limit_ch5;

Toggle toggle[] =new Toggle[6];

color blue = color(0,113,220);
color green = color(0,50,220);
color yellow = color(190,190,0);
color red = color(190,0,50);

int limit_LR = 50;
int limit_CENTER = 200;

class Gesture {
  
  Gesture(){
  }
  
  int val1 = 0;
  int val2 = 0;
  int direction = 0;
  int max = 0;
  
  boolean pressed = false;
  
}

ArrayList<Gesture> gesture = new ArrayList<Gesture>();


final int T_TOP = 0x01;
final int T_BOTTOM = 0x02;
final int T_LEFT = 0x04;
final int T_RIGHT = 0x08;
final int T_CENTER = 0x10;

final int EVT_PRESS = 0x1;

int touch_event = 0;
int touch_state = 0;

int cntup[] = new int[4];
int touchs[] = new int[4];
int frics[] = new int[4];


int val_old[] = new int[6];

//////////////////////////////////////////////////////

int w,h=1080;

void setup(){
  
  size(1080,1080);
 
  printArray(Serial.list());
  serial = new Serial(this, Serial.list()[0], 115200);
  serial.clear();
  
  serial.readStringUntil(10);
   
  Ani.init(this);
  
  cp5 = new ControlP5(this);
  slider[0] = cp5.addSlider("ch0")
   .setCaptionLabel("PROXITY")  
   .setPosition(20,20)
   .setSize(200,20)
   .setRange(0,512)
   .setValue(0)
   .setColorForeground(red)   
   .setSliderMode(Slider.FLEXIBLE);

  slider[2] = cp5.addSlider("ch2")
   .setCaptionLabel("CENTER")
   .setPosition(20,50)
   .setSize(200,20)
   .setRange(0,512)
   .setValue(0)
   .setColorForeground(yellow)
   .setSliderMode(Slider.FLEXIBLE);

  slider[1] = cp5.addSlider("ch1")
   .setPosition(20,80)
   .setSize(200,20)
   .setRange(0,255)
   .setValue(0)
   .setSliderMode(Slider.FLEXIBLE);

  slider[3] = cp5.addSlider("ch3")
   .setPosition(20,110)
   .setSize(200,20)
   .setRange(0,255)
   .setValue(0)
   .setSliderMode(Slider.FLEXIBLE);

  slider[4] = cp5.addSlider("ch4")
   .setPosition(20,140)
   .setSize(200,20)
   .setRange(0,255)
   .setValue(0)
   .setSliderMode(Slider.FLEXIBLE);

  slider[5] = cp5.addSlider("ch5")
   .setPosition(20,170)
   .setSize(200,20)
   .setRange(0,255)
   .setValue(0)
   .setSliderMode(Slider.FLEXIBLE);


  // limit
  slider_limit_prox = cp5.addSlider("limit_prox")
   .setPosition(20,210)
   .setSize(200,20)
   .setRange(0,512)
   .setColorForeground(red)   
   .setValue(150); 

  slider_limit_center = cp5.addSlider("limit_center")
   .setPosition(20,240)
   .setSize(200,20)
   .setRange(0,512)
   .setColorForeground(yellow)   
   .setValue(limit_CENTER); 

  slider_limit_ch4 = cp5.addSlider("limit_ch4")
   .setPosition(20,270)
   .setSize(200,20)
   .setRange(0,255)
   .setValue(limit_LR);

  slider_limit_ch5 = cp5.addSlider("limit_ch5")
   .setPosition(20,300)
   .setSize(200,20)
   .setRange(0,255)
   .setValue(limit_LR);
    
    

  int v_offset = 400;
  int h_offset = 100;
  toggle[0] = cp5.addToggle("center")
              .setPosition(h_offset, v_offset)
              .setValue(false)
              .setSize(40, 20);
              
  toggle[1] = cp5.addToggle("top")
              .setPosition(h_offset, v_offset-40)
              .setValue(false)
              .setSize(40, 20);
              
  toggle[2] = cp5.addToggle("bottom")
              .setPosition(h_offset, v_offset+40)
              .setValue(false)
              .setSize(40, 20);
              
  toggle[3] = cp5.addToggle("left")
              .setPosition(h_offset-60, v_offset)
              .setValue(false)
              .setSize(40, 20);
              
  toggle[4] = cp5.addToggle("right")
              .setPosition(h_offset+60, v_offset)
              .setValue(false)
              .setSize(40, 20);
              
  toggle[5] = cp5.addToggle("pressed")
            .setPosition(h_offset+120, v_offset)
            .setValue(false)
            .setColorActive(red) 
            .setSize(40, 20);
              
  for(int i=0; i<6; i++){
    gesture.add(new Gesture());
  }           
  
}

void ch0(int theVal) {
}
void ch1(int theVal) {
}
void ch2(int theVal) {
}
void ch3(int theVal) {
}
void ch4(int theVal) {
}
void ch5(int theVal) {
}


final int ST_TAP_IDLE = 0;
final int ST_TAP_PRESS = 1;
final int ST_TAP_RELEASE = 2;

int st_tap = ST_TAP_IDLE;

//////////////////////////////////////////////////////

void draw(){

  background(50,50,50,200);
  
  //  color c = color(255, 204, 0);

  // axis  
  int hh = height/2;
  int wh = width/2;
  strokeWeight(1);
  stroke(128);
  line(0,hh, width, hh);
  line(wh,0,wh, height);

  String s = null;
  int val[] = new int[6];
  boolean exist = false;
  boolean tap_detected = false;

  float limit_prox= slider_limit_prox.getValue();
  float limit_center= slider_limit_center.getValue();
  float limit_ch4 = slider_limit_ch4.getValue();
  float limit_ch5 = slider_limit_ch5.getValue();


  while (serial.available() > 0) {

    s = serial.readStringUntil(10);
    
    if (s != null) {

      try{
        JSONObject json = parseJSONObject(s);
        val[0] = json.getInt("prox");
        val[1] = json.getInt("ch1");   
        val[2] = json.getInt("ch2");
        val[3] = json.getInt("ch3");
        val[4] = json.getInt("ch4");
        val[5] = json.getInt("ch5");

        slider[0].setValue(val[0]);
        slider[1].setValue(val[1]);
        slider[2].setValue(val[2]);
        slider[3].setValue(val[3]);
        slider[4].setValue(val[4]);
        slider[5].setValue(val[5]);
        exist = true;
        
      }catch(Exception ex){
      } 
    }
    s = null;
  }
  toggle[0].setValue(false);      
  toggle[1].setValue(false);      
  toggle[2].setValue(false);      
  toggle[3].setValue(false);      
  toggle[4].setValue(false);

  // disp raw 
  if( val[2] >= limit_center){
      toggle[0].setValue(true);
  }
  if( val[3] >= limit_ch5 && 
      val[5] >= limit_ch5 ){
      toggle[1].setValue(true);
  }
  if( val[1] >= limit_ch5 && 
      val[5] >= limit_ch5 ){
      toggle[2].setValue(true);      
  }
  if( val[1] >= limit_ch4 && 
      val[4] >= limit_ch4 ){
      toggle[3].setValue(true);      
  }
  if( val[3] >= limit_ch4&& 
      val[4] >= limit_ch4 ){
      toggle[4].setValue(true);      
  }
  
  if( exist == true ){
  
    // Prox
    if( val[0] >= limit_prox ){
        touch_event |= EVT_PRESS;
        toggle[5].setValue(true);
    }else{
        touch_event = 0;
        toggle[5].setValue(false);
    }
     
    switch( st_tap ){
    
      case ST_TAP_IDLE:
        if( (touch_event & EVT_PRESS) > 0){
          st_tap = ST_TAP_PRESS;
          println("PRESS");
        }
        break;
        
      case ST_TAP_PRESS:
      
        if( (touch_event & EVT_PRESS) == 0){
      
          st_tap = ST_TAP_RELEASE;
          println("RELEASE");
        
        }else{
        
          int new_key = detectKey(val, limit_ch4, limit_ch4, limit_ch4, limit_ch5);
          
          if( new_key != 0 ){
             if( new_key != touch_state ){
                println(new_key);
                tap_detected = true;
              }
            touch_state = new_key;
          }
        }
        break;
        
      case ST_TAP_RELEASE:
      
        if((touch_state & T_TOP) > 0){
          touchs[0] += 1;
        }else if((touch_state & T_LEFT) > 0){
          touchs[1] += 1;
        }else if((touch_state & T_BOTTOM) > 0){
          touchs[2] += 1;
        }else if((touch_state & T_RIGHT) > 0){
          touchs[3] += 1;
        }
        
        st_tap = ST_TAP_IDLE;
        touch_state=0;
        break;
    }

    fric(val, val_old);
    for(int i=0; i<6; i++){    
      val_old[i] = val[i];
    }
      }
  
  textSize(24);
  text("T   " + touchs[0], 20, height/2+40);
  text("L   " + touchs[1], 20, height/2+70);
  text("B   " + touchs[2], 20, height/2+100);
  text("R   " + touchs[3], 20, height/2+130);
  
  text("FRIC" , 20, height/2 + 170);
  text("T   " + frics[0], 20, height/2+200);
  text("L   " + frics[1], 20, height/2+230);
  text("B   " + frics[2], 20, height/2+260);
  text("R   " + frics[3], 20, height/2+290);
  

}

final int CH1 = 0x01;
final int CH3 = 0x02;
final int CH4 = 0x04;
final int CH5 = 0x08;

int detectKey(int val[],  float limit1,  float limit3, float limit4, float limit5){

  int st = 0;
  int keycode = 0;
  
 
  if( val[1] > limit1 ){
    st |= CH1;
  }
  if( val[3] > limit3 ){
    st |= CH3;  
  }
  
  if( val[4] > limit4 ){
    st |= CH4;  
  }
  if( val[5] > limit5 ){
    st |= CH5;  
  }
  
  if( st == (CH1|CH4)){
    keycode = T_LEFT;
  }
  else if( st == (CH1|CH5)){
    keycode = T_BOTTOM;
  }
  else if( st == (CH3|CH4)){
    keycode = T_RIGHT;
  }
  else if( st == (CH3|CH5)){
    keycode = T_TOP;
  }else{
    keycode =0;
  }
  
  return keycode;
}


int DELTA_TH1 = 8;
int DELTA_TH2 = 5;

final int ST_FRIC_IDLE = 0;
final int ST_FRIC_PRESS = 1;
final int ST_FRIC_DETECT = 2;
int st_fric = ST_FRIC_IDLE;

int[] fric_cnt = new int[4];
final int DELTA_TH = 10;

int fric( int[] new_val, int[] val ){

  int code = 0;
  int df_t;
  int df_b;
  int df_l;
  int df_r;

  int df_ch1 = new_val[1] - val[1];
  int df_ch3 = new_val[3] - val[3];
  int df_ch4 = new_val[4] - val[4];
  int df_ch5 = new_val[5] - val[5];
  
  df_b = df_ch1 + df_ch5;
  df_t = df_ch3 + df_ch5;
  df_l = df_ch1 + df_ch4;
  df_r = df_ch3 + df_ch4;

  println(df_t+ "  " + df_l  + "  " + df_b + "  " + df_r);

  switch(st_fric){
  
    case ST_FRIC_IDLE:
    
      if( (touch_event & EVT_PRESS) > 0){

        st_fric = ST_FRIC_PRESS;
        for(int i=0; i<4; i++){
          fric_cnt[i] = 0;
        }
      }
      break;
      
    case ST_FRIC_PRESS:
    
      if( df_t > DELTA_TH){
        fric_cnt[0] += df_t;
      }
      if( df_l > DELTA_TH){
        fric_cnt[1] += df_l;
      }
      if( df_b > DELTA_TH){
        fric_cnt[2] += df_b;
      }
      if( df_r > DELTA_TH){
        fric_cnt[3] += df_r;
      }
      
      if( (touch_event & EVT_PRESS) == 0){
        
        st_fric = ST_FRIC_DETECT;
        println("FRIC:  " + fric_cnt[0] + "  " + fric_cnt[1] + "  "+  fric_cnt[2]+ "  "+fric_cnt[3]);

        if( fric_cnt[0] == 0 && 
            fric_cnt[1] == 0 &&
            fric_cnt[2] == 0 &&
            fric_cnt[3] == 0 ){
        }else{

          int max_val = max(fric_cnt);
          int idx = 0;
          for(int i=0; i<4; i++){
            if(fric_cnt[i] == max_val){
              idx=i;
              break;
            }
          }
          if( fric_cnt[idx] > 40 ){
            frics[idx]++;
          }
        }
      }      
      break;
      
    case ST_FRIC_DETECT:
    
      st_fric = ST_FRIC_IDLE;
      break;
      
    default:
      break;
  }
  
  return code;
}
