import processing.serial.*;
import de.looksgood.ani.*;
import controlP5.*;
Serial serial;      

ControlP5 cp5;
Slider slider[] = new Slider[7];
Slider slider_limit_prox;
Slider slider_limit_center;
Slider slider_limit_top;
Slider slider_limit_right;
Slider slider_limit_bottom;
Slider slider_limit_left;
Slider slider_limit_fric;

Toggle top;
Toggle left;
Toggle right;
Toggle bottom;
Toggle press;
Toggle center;

color blue = color(0,113,220);
color green = color(0,50,220);
color yellow = color(190,190,0);
color red = color(190,0,50);

int limit_LR = 80;
int limit_CENTER = 150;

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
int touch_pressed = 0;

int touchs[] = new int[5];
int frics[] = new int[4];


int val_old[] = new int[6];

final int ST_TAP_IDLE = 0;
final int ST_TAP_PRESS = 1;
final int ST_TAP_RELEASE = 2;
final int ST_TAP_LONG = 3;


int st_tap = ST_TAP_IDLE;
int touch_cnt = 0;

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


  slider[1] = cp5.addSlider("TOP")
   .setPosition(20,50)
   .setSize(200,20)
   .setRange(0,255)
   .setValue(0)
   .setSliderMode(Slider.FLEXIBLE);

  slider[2] = cp5.addSlider("RIGHT")
   .setPosition(20,80)
   .setSize(200,20)
   .setRange(0,255)
   .setValue(0)
   .setSliderMode(Slider.FLEXIBLE);

  slider[3] = cp5.addSlider("BOTTOM")
   .setPosition(20,110)
   .setSize(200,20)
   .setRange(0,255)
   .setValue(0)
   .setSliderMode(Slider.FLEXIBLE);

  slider[4] = cp5.addSlider("LEFT")
   .setPosition(20,140)
   .setSize(200,20)
   .setRange(0,255)
   .setValue(0)
   .setSliderMode(Slider.FLEXIBLE);

  slider[5] = cp5.addSlider("ch2")
   .setCaptionLabel("CENTER")
   .setPosition(20,170)
   .setSize(200,20)
   .setRange(0,512)
   .setValue(0)
   .setColorForeground(yellow)
   .setSliderMode(Slider.FLEXIBLE);

  slider[6] = cp5.addSlider("ch5")
   .setCaptionLabel("CENTER")
   .setPosition(20,200)
   .setSize(200,20)
   .setRange(0,512)
   .setValue(0)
   .setColorForeground(yellow)
   .setSliderMode(Slider.FLEXIBLE);



  // limit
  slider_limit_prox = cp5.addSlider("limit_prox")
   .setPosition(20,240)
   .setSize(200,20)
   .setRange(0,1024)
   .setColorForeground(red)   
   .setValue(500); 

  slider_limit_center = cp5.addSlider("limit_center")
   .setPosition(20,270)
   .setSize(200,20)
   .setRange(0,512)
   .setColorForeground(yellow)   
   .setValue(limit_CENTER); 

  slider_limit_top = cp5.addSlider("limit_top")
   .setPosition(20,300)
   .setSize(200,20)
   .setRange(0,255)
   .setValue(limit_LR);

  slider_limit_right = cp5.addSlider("limit_right")
   .setPosition(20,330)
   .setSize(200,20)
   .setRange(0,255)
   .setValue(limit_LR);

  slider_limit_bottom = cp5.addSlider("limit_bottom")
   .setPosition(20,360)
   .setSize(200,20)
   .setRange(0,255)
   .setValue(limit_LR);

  slider_limit_left = cp5.addSlider("limit_left")
   .setPosition(20,390)
   .setSize(200,20)
   .setRange(0,255)
   .setValue(limit_LR);

  slider_limit_fric = cp5.addSlider("slider_limit_fric")
   .setPosition(20,420)
   .setSize(200,20)
   .setRange(0,255)
   .setValue(40);
    
    
    

  int v_offset = 500;
  int h_offset = 100;
  
  center = cp5.addToggle("center")
              .setPosition(h_offset, v_offset)
              .setValue(false)
              .setSize(40, 20);
              
  top = cp5.addToggle("top")
              .setPosition(h_offset, v_offset-40)
              .setValue(false)
              .setSize(40, 20);

  right = cp5.addToggle("right")
              .setPosition(h_offset+60, v_offset)
              .setValue(false)
              .setSize(40, 20);
              
  bottom = cp5.addToggle("bottom")
              .setPosition(h_offset, v_offset+40)
              .setValue(false)
              .setSize(40, 20);
              
  left = cp5.addToggle("left")
              .setPosition(h_offset-60, v_offset)
              .setValue(false)
              .setSize(40, 20);
              
         
  press = cp5.addToggle("pressed")
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



long st_time = 0;

//////////////////////////////////////////////////////

void draw(){


  background(50,50,50,200);
  
  //  color c = color(255, 204, 0);

  // axis  
  int hh = height/2;
  int wh = width/2;
  strokeWeight(1);
  stroke(128);
  //line(0,hh, width, hh);
  line(wh,0,wh, height);

  String s = null;
  int val[] = new int[7];
  boolean exist = false;
  int keycode = 0;
  
  //float limit_prox= slider_limit_prox.getValue();
  //float limit_center= slider_limit_center.getValue();
  //float limit_top = slider_limit_top.getValue();
  //float limit_right = slider_limit_right.getValue();
  //float limit_bottom = slider_limit_bottom.getValue();
  //float limit_left = slider_limit_left.getValue();


  while (serial.available() > 0) {

    s = serial.readStringUntil(10);
    
    if (s != null) {

      try{
        JSONObject json = parseJSONObject(s);
        val[0] = json.getInt("prox");
        val[1] = json.getInt("ch1");  // top
        val[2] = json.getInt("ch6");  // r
        val[3] = json.getInt("ch3");  // b
        val[4] = json.getInt("ch4");  // l
        val[5] = json.getInt("ch2");  // center
        val[6] = json.getInt("ch5");  // center

        slider[0].setValue(val[0]);
        slider[1].setValue(val[1]);    // top
        slider[2].setValue(val[2]);    // center
        slider[3].setValue(val[3]);    // bottom
        slider[4].setValue(val[4]);    // left
        slider[5].setValue(val[5]);    // right
        slider[6].setValue(val[6]);    // right

        exist = true;
        
      }catch(Exception ex){
      } 
    }
    s = null;
  }
  
  top.setValue(false);      // t
  right.setValue(false);    // r
  bottom.setValue(false);   // b
  left.setValue(false);     // l
  press.setValue(false);    // press
  center.setValue(false);    // press

  
  if( exist == true ){

    int code = getKey(val);

    int detected = click(code);
    if( detected !=0 ){
      println("[detected]" +detected);
    }else{
      fric(val, val_old);
    }
    
    for(int i=0; i<6; i++){
      val_old[i] = val[i];
    }
  }
  
  textSize(24);
  int j=30;
  int sh = height/2 + 20;
  text("TAP " + touch_cnt, 20, sh+j);
  text("T   " + touchs[0], 20, sh+j*2);
  text("L   " + touchs[1], 20, sh+j*3);
  text("B   " + touchs[2], 20, sh+j*4);
  text("R   " + touchs[3], 20, sh+j*5);
  text("CENTER " + touchs[4], 20, sh+j*6);
  
  text("FRIC" , 20, sh + j*8);
  text("T   " + frics[0], 20, sh+j*9);
  text("L   " + frics[1], 20, sh+j*10);
  text("B   " + frics[2], 20, sh+j*11);
  text("R   " + frics[3], 20, sh+j*12);

}


//int DELTA_TH1 = 8;
//int DELTA_TH2 = 5;

final int ST_FRIC_IDLE = 0;
final int ST_FRIC_PRESS = 1;
final int ST_FRIC_DETECT = 2;
int st_fric = ST_FRIC_IDLE;

int[] fric_cnt = new int[4];
final int DELTA_TH = 10;


boolean prox = false; 

int getKey(int[] val ){

  int code = 0;
  float limit_prox= slider_limit_prox.getValue();
  float limit_top = slider_limit_top.getValue();
  float limit_right = slider_limit_right.getValue();
  float limit_bottom = slider_limit_bottom.getValue();
  float limit_left = slider_limit_left.getValue();
  float limit_center = slider_limit_center.getValue();

  if( val[0] >= limit_prox ){
    touch_event |= EVT_PRESS;
    press.setValue(true);
    //code |= T_PROX;
  }else{
    touch_event = 0;
  } 

  if( val[1] >= limit_top ){
      top.setValue(true);
      code |= T_TOP;
  }       
  else if( val[2] >= limit_right ){
      right.setValue(true);
      code |= T_RIGHT;
  }       
  else if( val[3] >= limit_bottom ){
      bottom.setValue(true);
      code |= T_BOTTOM;
  }       
  else if( val[4] >= limit_left ){
      left.setValue(true);
      code |= T_LEFT;
  }
  else if( val[5] >= limit_center ){  
  //else if( val[5] >= limit_center &&
  //         val[6] >= limit_center){
      center.setValue(true);
      code |= T_CENTER;
  }  
  return code;
}

long click_st_time = 0;

int click( int code ){

  int detected = 0;

  switch( st_tap ){

    case ST_TAP_IDLE:
    
     if( code > 0){
        touch_state = code;        
        click_st_time = millis();
        st_tap = ST_TAP_PRESS;
        println("[CLICK START] " + touch_state);
     }
     break;
      
    case ST_TAP_PRESS:
    
      if( code == 0){

        println("[CLICK RELEASE] " + touch_state);
        detected = touch_state;
        st_tap = ST_TAP_RELEASE;
      
      }else{

        if( millis() - click_st_time > 1000 ){
          
            detected = touch_state;
            st_tap = ST_TAP_LONG;
            println("[TAP LONG] " + touch_state);
        }
      }
      break;
      
    case ST_TAP_LONG:
      
      if( code == 0){
          println("[LONG CLICK RELEASE]");
          st_tap = ST_TAP_RELEASE;
      }
      break;
      
    case ST_TAP_RELEASE:
      if( code == 0){
        st_tap = ST_TAP_IDLE;
      }
      break;
  }
  
  return detected;
}


void fric( int[] new_val, int[] val ){

  float limit_prox= slider_limit_prox.getValue();
  //float limit_center= slider_limit_center.getValue();
  float limit_fric = slider_limit_fric.getValue();
  
  int df_t = new_val[1] - val[1];
  int df_r = new_val[2] - val[2];
  int df_b = new_val[3] - val[3];
  int df_l = new_val[4] - val[4];
  
  // Prox
  // if( val[0] >= limit_prox ){
  //    touch_event |= EVT_PRESS;
  //    press.setValue(true);
  //}else{
  //  touch_event=0;
  //}
  
  
  switch( st_fric ){
  
    case ST_FRIC_IDLE:
    
      if( (touch_event & EVT_PRESS) > 0){
        
        println("[PRESS]");
        st_time = millis();
        st_fric = ST_FRIC_PRESS;
        for(int i=0; i<4; i++){
          fric_cnt[i] = 0;
        }
      }
      break;
      
    case ST_FRIC_PRESS:

      fric_cnt[0] += df_t;
      fric_cnt[1] += df_r;
      fric_cnt[2] += df_b;
      fric_cnt[3] += df_l;
      
      if( touch_event == 0 ){

        long delta_time = millis() - st_time;
        st_fric = ST_FRIC_DETECT;
        println("[RELEASE]:  " + fric_cnt[0] + "  " + fric_cnt[1] + "  "+  fric_cnt[2]+ "  "+fric_cnt[3]);

        if( fric_cnt[0] <= 0 && 
            fric_cnt[1] <= 0 &&
            fric_cnt[2] <= 0 &&
            fric_cnt[3] <= 0 ){
              
          println( "[NOISY]");
          
        }else{

          int max_val = max(fric_cnt);
          int idx = 0;
          for(int i=0; i<4; i++){
            if(fric_cnt[i] == max_val){
              idx=i;
              break;
            }
          }
          
          //if( fric_cnt[idx] > slider_limit_fric.getValue() ){
      
          print( "[FRIC] :" + delta_time + "ms  ");
          frics[idx]++;
          switch(idx){
            case 0: println( "T");  break;
            case 1: println( "R");  break;
            case 2: println( "B");  break;
            case 3: println( "L");  break;
            default:
              break;
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
}
