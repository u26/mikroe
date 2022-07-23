import processing.serial.*;
import de.looksgood.ani.*;
import controlP5.*;

// 
// select device
//-----------------------------------------------------
static final int DEV_MOUSE =0;
static final int DEV_TOUCHPAD = 1;
static final int DEV_TOUCHPAD5 = 2;

int device = DEV_TOUCHPAD5;    // << select device
String txt_device = "MOUSE";
int pos_dispx;
//-----------------------------------------------------

//ControlP5 bt_mouse;
//ControlP5 bt_touchpad;
//ControlP5 bt_touchpad5;

Slider slider_click;
Slider slider_click_time;
Slider slider_fric_limit;
Slider slider_fric_time;
float scale=1;
//int FLICK_THRESOULD_NOISE;


Serial serial;      

int FLICK_RELEASE_JUDGE_CNT = 10;  //n * 10ms(sensor tx interval)
int FLICK_WAIT_MS = 100;


//int sx;
//int sy;
Point pt_sum = new Point();
Point pt_release = new Point();

String dir = new String();
String tx_dir = new String();

Point pt_move = new Point();


long st_time;
float deg = 0;
float deg_last = 0;
float vec = 0;

int cnt[] = new int[5];

class Point{
  
  Point(){
    this.x = 0;
    this.y = 0;
    strength = 0;
    long t;
  }
  Point(int x, int y, int strength){
    this.x = x;
    this.y = y;
    this.strength = strength;
    this.t = millis();
  }
  
  int x;
  int y;
  int strength;
  int t;
  
}

ArrayList<Point> pts = new ArrayList<Point>();

enum ST_FLICK {
  IDLE,
  PRESS,
  RELEASE,
  LONG_PRESS,
}

ST_FLICK st_flick = ST_FLICK.IDLE;
boolean flick_detected = false; 
int flick_release_cnt = 0; 
int lf = 10;


//////////////////////////////////////////////////////

void setup(){
  
  size(1000, 800);
 
  printArray(Serial.list());
  serial = new Serial(this, Serial.list()[0], 115200);
  serial.clear();
  
  serial.readStringUntil(lf);
   
  Ani.init(this);

  pos_dispx = width-180;
  //bt_mouse = new ControlP5(this);
  //bt_mouse.addButton("bt_mouse_clikced")
  //        .setLabel("MOUSE SENSOR")
  //        .setPosition(pos_dispx, 60)
  //        .setSize(120, 30);
 
  //bt_touchpad = new ControlP5(this);
  //bt_touchpad.addButton("bt_touchpad_clikced")
  //        .setLabel("TouchPad")
  //        .setPosition(pos_dispx, 100)
  //        .setSize(120, 30);
 
  //bt_touchpad5 = new ControlP5(this);
  //bt_touchpad5.addButton("bt_touchpad5_clikced")
  //        .setLabel("IQS550")
  //        .setPosition(pos_dispx, 140)
  //        .setSize(120, 30);
          
  //bt_mouse_clikced();


  // limit
  int row = 30;
  int y_pos = 100;
  ControlP5 cp5 = new ControlP5(this);
  slider_click = cp5.addSlider("CLICK  LIMIT")
   .setPosition(pos_dispx, y_pos)
   .setSize(100,20)
   .setRange(0,1024)
   .setValue(100); 
   
  slider_click_time = cp5.addSlider("CLICK  TIME")
   .setPosition(pos_dispx, y_pos+row*1)
   .setSize(100,20)
   .setRange(0,2000)
   .setValue(500);    
  
  slider_fric_limit = cp5.addSlider("FRIC  LIMIT")
   .setPosition(pos_dispx,y_pos+row*2)
   .setSize(100,20)
   .setRange(0,1024)
   .setValue(100); 

  slider_fric_time = cp5.addSlider("FRIC  TIME")
   .setPosition(pos_dispx,y_pos+row*3)
   .setSize(100,20)
   .setRange(0,1024)
   .setValue(150); 


}

//////////////////////////////////////////////////////

void draw(){

  background(50);
  
  //  color c = color(255, 204, 0);

  // axis  
  int hh = height/2;
  int wh = width/2-100;
  strokeWeight(1);
  stroke(128);
  line(0,hh, width-200, hh);
  line(wh,0,wh, height);

  String s = null;

  pushMatrix();
 
  translate(wh, hh);

  noFill();

  //rectMode(CENTER);
  //rect(0,0, FLICK_THRESOULD_NOISE*2, FLICK_THRESOULD_NOISE*2);
  circle(0,0,(slider_fric_limit.getValue()*scale)*2);
  
  //float th = slider_click_area.getValue();
  while (serial.available() > 0) {

    s = serial.readStringUntil(lf);
    
    if (s != null) {

      try{
        JSONObject json = parseJSONObject(s);
        int dx = json.getInt("x");   
        int dy = json.getInt("y");
        int status = json.getInt("status");
        int strength = json.getInt("strength");

        if( status > 0){
            print(s);
        }

        flick(dx, dy, status, strength);
      
      }catch(Exception ex){
      
      } 
    }
    s = null;
  }
  
  // Draw Points
  push();

  fill(255);
  noStroke();
  for(int i=0; i<pts.size(); i++){
    
    translate( pts.get(i).x*scale, 
               pts.get(i).y*scale );
    
    if( st_flick == ST_FLICK.IDLE ){
      if( i >= release_idx){
        fill(0, 255,200);
      }
    }
    circle(0,0,10);
  }
  pop();

  stroke(color(0, 255,200));
  strokeWeight(2);
  line(0, 0, pt_release.x*scale, pt_release.y*scale);

  stroke(color(255));
  strokeWeight(2);
  line(0, 0, pt_sum.x*scale, pt_sum.y*scale);

  popMatrix();
  

  fill(255);

  textSize(18);
  text( "IQS550", pos_dispx, 40);

  int lh = 30;
  int sh = 40;
  int i=0;
  
  textSize(24);
  text( tx_dir, 40, sh+lh*(++i));

  textSize(20);
  text( "dx sum: " + pt_sum.x, 40, sh+lh*(++i)); 
  text( "dy sum: " + pt_sum.y, 40, sh+lh*(++i)); 
  text( "degree: " + deg, 40,sh+lh*(++i));
  text( "vec: " + vec, 40,sh+lh*(++i));
 
  if(st_flick == ST_FLICK.RELEASE){
    text( "RELEASE", 40, sh+lh*(++i));
  }else if(st_flick == ST_FLICK.LONG_PRESS){
    text( "LONG_PRESS", 40, sh+lh*(++i));
  }else if(st_flick == ST_FLICK.PRESS){
    text( "PRESS", 40, sh+lh*(++i));
  }else{
    text( "IDLE", 40, sh+lh*(++i));
  }
  text( "Left:  " + cnt[0], 40,sh+lh*(++i));
  text( "Right: " + cnt[1], 40,sh+lh*(++i));
  text( "UP:    " + cnt[2], 40,sh+lh*(++i));
  text( "DOWN:  " + cnt[3], 40,sh+lh*(++i));
  text( "Click: " + cnt[4], 40,sh+lh*(++i));
 
 } 

int release_idx = 0;
long release_dt = 0; 
long click_dt = 0; 


void cal_sum(){

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
    
    int s = pts.get(i).strength;
    if( max_strength < s ){
      max_strength = s;
    }else{
      release_idx = i+1;
      println("MAX:"+ max_strength + " max_idx:"+release_idx);
      break;
    }
  }
  
  for(int i=release_idx; i<=sz; i++){
    pt_release.x += pts.get(i).x;
    pt_release.y += pts.get(i).y;

    println("release x:"+  pts.get(i).x + 
            " y:" +  pts.get(i).y + 
            " strength:" + pts.get(i).strength + 
            " t:" + pts.get(i).t );
  }
  release_dt =  pts.get(sz).t - pts.get(release_idx).t;
  println("RELEASE x:"+ pt_release.x + " y:" + pt_release.y + " dt:" + release_dt );
  
  int st_pos = release_idx - 5;
  if( st_pos < 0 ){
    st_pos=0;
  }
  for(int i=st_pos; i < release_idx; i++){
      pt_move.x += pts.get(i).x;
      pt_move.y += pts.get(i).y;
  }
  println("CLICK x:"+ pt_move.x + " y:" + pt_move.y );
  
  for(int i=0; i<=sz; i++){
    pt_sum.x += pts.get(i).x;
    pt_sum.y += pts.get(i).y;
  }
  println("SUM x:"+ pt_sum.x + " y:" + pt_sum.y);

}


//////////////////////////////////////////////////////

void flick(int dx, int dy, int status, int strength){

  float fric_limit = slider_fric_limit.getValue();
  float FRIC_TIME = slider_fric_time.getValue();
  float CLICK_LIMIT = slider_click.getValue();
  float CLICK_TIME = slider_click_time.getValue();

  
  if( st_flick == ST_FLICK.IDLE ){
    
    if( status > 0){
      println("PRESS");
      st_flick = ST_FLICK.PRESS;
      dir = "";
      tx_dir = "";
      
      pts.clear();
      st_time = millis();
    }
    
  }else if(st_flick == ST_FLICK.PRESS){

    click_dt = millis() - st_time;    

    if( status == 0 ){

      println("[RELEASE] delta:"+click_dt);
      st_flick = ST_FLICK.IDLE;

      cal_sum();

      vec = sqrt(pow(pt_sum.x,2) + pow(pt_sum.y,2));
      println("vector: " + vec);
      
     
      boolean b_fric = (abs(pt_release.x) > fric_limit || abs(pt_release.y) > fric_limit)?true:false;

      if( abs(pt_sum.x) < CLICK_LIMIT && 
          abs(pt_sum.y) < CLICK_LIMIT &&
          click_dt < CLICK_TIME &&
          b_fric == false){

        println("[CLICK]");
        cnt[4]++;

      }else if( b_fric == true &&
                release_dt < FRIC_TIME ){
      
        print("[FRIC]");
        deg = atan2(pt_sum.y, pt_sum.x);
        deg = degrees(deg) * -1;
        if( (deg > 135 && deg <= 180)||(deg < -135 && deg >= -180) ){
          println("LEFT");
          cnt[0]++;     
        }
        else if((deg >= -45 && deg < 45)){
          println("RIGHT");
          dir = "RIGHT";
          cnt[1]++;          
        }
        else if(deg >= 45 && deg < 135){
          println("UP");
          dir = "UP";
          cnt[2]++;       
        }
        else if(deg < -45 && deg > -135){
          println("DOWN");
          dir = "DOWN";
          cnt[3]++;             
        }
        else{
        }
      
      }else{
        println("NOISY DATA");
      }

    }else{

      pts.add( new Point(dx, dy, strength) );

//      if( click_dt < CLICK_TIME ){
//        pts.add( new Point(dx, dy, strength) );
//      }else{
//          println("[LONG PRESSED]");
//          st_flick = ST_FLICK.LONG_PRESS;
//          pts.clear();
//      }
    }
  
  }else if(st_flick == ST_FLICK.LONG_PRESS){
    
    if( status == 0 ){
      st_flick = ST_FLICK.IDLE;
    }
  }
/*    
    if( status == 0 ){
      
      println("RELEASE");
      st_flick = ST_FLICK.IDLE;
    
      cal_sum();

      vec = sqrt(pow(pt_sum.x,2) + pow(pt_sum.y,2));
      println("vector: " + vec);
      
      float fric_limit = slider_fric_limit.getValue();
      if( abs(pt_sum.x) > fric_limit || 
          abs(pt_sum.y) > fric_limit ){
            
          println("FRIC");
          deg = atan2(pt_sum.y, pt_sum.x);
          deg = degrees(deg) * -1;
          if( (deg > 135 && deg <= 180)||(deg < -135 && deg >= -180) ){
            println("LEFT");
            cnt[0]++;     
          }
          else if((deg >= -45 && deg < 45)){
            println("RIGHT");
            dir = "RIGHT";
            cnt[1]++;          
          }
          else if(deg >= 45 && deg < 135){
            println("UP");
            dir = "UP";
            cnt[2]++;       
          }
          else if(deg < -45 && deg > -135){
            println("DOWN");
            dir = "DOWN";
            cnt[3]++;             
          }
          else{
          }
      }else{
        if( abs(pt_move.x) < 50 && 
            abs(pt_move.y) < 50 ){
  
          println("CLICK");
          cnt[4]++;  
        }          
      }
      
    }else{
      pts.add( new Point(dx,dy, strength) );
    }
  }
*/   
}


void keyPressed() {
  if (key == ESC) {
    pts.clear();
    //sx = 0;
    //sy = 0;
    dir = "";
    tx_dir = "";
    deg = 0;
    cnt[0] = 0;
    cnt[1] = 0;
    cnt[2] = 0;
    cnt[3] = 0;
    cnt[4] = 0;
  }
}
