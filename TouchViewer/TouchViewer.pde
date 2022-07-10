import processing.serial.*;
import de.looksgood.ani.*;
import controlP5.*;

// 
// select device
//-----------------------------------------------------
static final int DEV_MOUSE =0;
static final int DEV_TOUCHPAD = 1;
static final int DEV_TOUCHPAD5 = 2;

int device = DEV_TOUCHPAD;    // << select device
String txt_device = "MOUSE";
int pos_dispx;
//-----------------------------------------------------

ControlP5 bt_mouse;
ControlP5 bt_touchpad;
ControlP5 bt_touchpad5;


float scale;
int FLICK_THRESOULD_NOISE;


Serial serial;      

int FLICK_RELEASE_JUDGE_CNT = 10;  //n * 10ms(sensor tx interval)
int FLICK_WAIT_MS = 100;

//int sx;
//int sy;
Point pt_sum = new Point();
String dir = new String();
String tx_dir = new String();

long st_time;
float deg = 0;
float deg_last = 0;
float vec = 0;

int cnt[] = new int[5];

class Point{
  
  Point(){
    this.x = 0;
    this.y = 0;
  }
  Point(int x, int y){
    this.x = x;
    this.y = y;
  }
  
  int x;
  int y;
}

ArrayList<Point> pts = new ArrayList<Point>();

enum ST_FLICK {
  IDLE,
  PRESS,
  RELEASE,
  WAITING,
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
  bt_mouse = new ControlP5(this);
  bt_mouse.addButton("bt_mouse_clikced")
          .setLabel("MOUSE SENSOR")
          .setPosition(pos_dispx, 60)
          .setSize(120, 30);
 
  bt_touchpad = new ControlP5(this);
  bt_touchpad.addButton("bt_touchpad_clikced")
          .setLabel("TouchPad")
          .setPosition(pos_dispx, 100)
          .setSize(120, 30);
 
  bt_touchpad5 = new ControlP5(this);
  bt_touchpad5.addButton("bt_touchpad5_clikced")
          .setLabel("IQS550")
          .setPosition(pos_dispx, 140)
          .setSize(120, 30);
          
  bt_mouse_clikced();


}

void bt_touchpad5_clikced(){
  
  device = DEV_TOUCHPAD5;
  txt_device = "TouchPad5";
  scale = 0.3;  
  FLICK_THRESOULD_NOISE = 300;
}

void bt_touchpad_clikced(){

  device = DEV_TOUCHPAD;
  txt_device = "TouchPad";
  scale = 10;
  FLICK_THRESOULD_NOISE = 5;

}

void bt_mouse_clikced(){

  device = DEV_MOUSE;
  txt_device = "MOUSE";
  scale = 0.5;  
  FLICK_THRESOULD_NOISE = 80;
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
  circle(0,0,(FLICK_THRESOULD_NOISE*scale)*2);
  
  while (serial.available() > 0) {


    s = serial.readStringUntil(lf);
    
    if (s != null) {

      try{
        JSONObject json = parseJSONObject(s);
        int dx = json.getInt("x");   
        int dy = json.getInt("y");

        if( !(dx==0 && dy==0)){
            print(s);
        }
        flick(dx,dy);
       
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
      
    circle(0,0,10);
  }
  pop();

  // delta position
  stroke(color(255, 200,0));
  strokeWeight(4);
  //Point pt = deltaPoint();
  line(0, 0, pt_sum.x*scale, pt_sum.y*scale);

  popMatrix();
  

  fill(255);

  textSize(18);
  text( txt_device, pos_dispx, 40);

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
  }else if(st_flick == ST_FLICK.WAITING){
    text( "WAIT", 40, sh+lh*(++i));
  }else if(st_flick == ST_FLICK.PRESS){
    text( "PRESS", 40, sh+lh*(++i));
  }else{
    text( "IDLE", 40, sh+lh*(++i));
  }
  text( "Left:  " + cnt[0], 40,sh+lh*(++i));
  text( "Right: " + cnt[1], 40,sh+lh*(++i));
  text( "UP:    " + cnt[2], 40,sh+lh*(++i));
  text( "DOWN:  " + cnt[3], 40,sh+lh*(++i));
  text( "noise: " + cnt[4], 40,sh+lh*(++i));
 
} 


void deltaPoint(){

  pt_sum.x = 0;
  pt_sum.y = 0;

  for(int i=0; i<pts.size(); i++){
    pt_sum.x += pts.get(i).x;
    pt_sum.y += pts.get(i).y;
  }
}

//////////////////////////////////////////////////////

void flick(int dx, int dy){
  
  
  if( (dx != 0 || dy != 0) &&
      st_flick == ST_FLICK.IDLE ){

    println("PRESS");
    st_flick = ST_FLICK.PRESS;
    dir = "";
    tx_dir = "";
    
    pts.clear();
    pts.add( new Point(dx,dy) );
    
  }else if(st_flick == ST_FLICK.PRESS){
    
    if( dx == 0 && dy == 0 ){

      st_flick = ST_FLICK.RELEASE;
      flick_release_cnt = 0;
   
    }else{
      pts.add( new Point(dx,dy) );
    }

  }else if(st_flick == ST_FLICK.RELEASE){

    // RELEASE CHECK
    if( dx == 0 && dy == 0 ){

      flick_release_cnt++;
      if( flick_release_cnt > FLICK_RELEASE_JUDGE_CNT ){

        println("RELEASE");
        st_flick = ST_FLICK.WAITING;
        st_time = millis();
  
        pts.add( new Point(dx,dy) );
        
        deltaPoint();
        deg = atan2(pt_sum.y, pt_sum.x);
        deg = degrees(deg) * -1;
  
        if( (deg > 135 && deg <= 180)||(deg < -135 && deg >= -180) ){
          dir = "LEFT";
        }
        else if((deg >= -45 && deg < 45)){
          dir = "RIGHT";
        }
        else if(deg >= 45 && deg < 135){
          dir = "UP";
        }
        else if(deg < -45 && deg > -135){
          dir = "DOWN";
        }
        else{
        }
  
        // CHECK NOISE
        vec = sqrt(pow(pt_sum.x,2) + pow(pt_sum.y,2));
        
        if( vec < FLICK_THRESOULD_NOISE){
          dir = "NOISE";
        }
  
        tx_dir = dir;
  
        println("dx:"+pt_sum.x + " dy:"+pt_sum.y +" deg:"+deg+" dir:" + dir + " vec:"+vec);
  
  
  
        // DEBUG COUNT
        if( tx_dir.equals( "LEFT") ){
          cnt[0]++;
        }else if( tx_dir.equals( "RIGHT") ){
          cnt[1]++;
        }else if( tx_dir.equals( "UP") ){
          cnt[2]++;
        }else if( tx_dir.equals( "DOWN") ){
          cnt[3]++;
        }else{
          cnt[4]++;
        }
      }else{
        //do nothing
      }
      
    }else{
      flick_release_cnt = 0;
      st_flick = ST_FLICK.PRESS;
    }
    
  }else if(st_flick == ST_FLICK.WAITING){
    
    if( abs( millis()-st_time) > FLICK_WAIT_MS ){

      println("IDLE");
      st_flick = ST_FLICK.IDLE;
    }
    
  }else{
    // do nothing
  }
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
