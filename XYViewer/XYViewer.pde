import processing.serial.*;
import de.looksgood.ani.*;

// 
// select device
//-----------------------------------------------------
static final int DEV_MOUSE =0;
static final int DEV_TOUCHPAD = 1;
static final int DEV_TOUCHPAD5 = 2;

int device = DEV_MOUSE;    // << select device
//-----------------------------------------------------

float scale=0.5;
//int scale;
int FLICK_THRESOULD_NOISE;


Serial serial;      

int FLICK_RELEASE_JUDGE_CNT = 10;  //n * 10ms(sensor tx interval)
int FLICK_WAIT_MS = 200;

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

// axis  
int hh;
int wh;
int pos_x;
int pos_y;
//////////////////////////////////////////////////////

int w,h=1080;

void setup(){
  
  size(2160, 2160);
 
  printArray(Serial.list());
  serial = new Serial(this, Serial.list()[0], 115200);
  serial.clear();
  
  serial.readStringUntil(lf);
   
  Ani.init(this);
  
  background(50,50,50,200);
  hh = 1080;
  wh = 1080;
  pos_x= 0;
  pos_y = 0;
}



//////////////////////////////////////////////////////

void draw(){

  background(50,50,50,1);
  
  //  color c = color(255, 204, 0);


  strokeWeight(1);
  stroke(128);
  line(0,hh, width, hh);
  line(wh,0,wh, height);

  String s = null;

  while (serial.available() > 0) {

    s = serial.readStringUntil(lf);
    
    if (s != null) {

      try{
        JSONObject json = parseJSONObject(s);
        int dx = json.getInt("x");   
        int dy = json.getInt("y");
        //int strength = json.getInt("strength");

        if( !(dx==0 && dy==0)){
            print(s);
        }
        push();
        fill(255);
        noStroke();
        translate(wh,hh);
        //pos_x += dx;
        //pos_y += dy;
        translate( dx * scale, 
                   dy * scale );
      
        circle(0,0,10);
       // circle(dx,dy,10);
        pop();
        
      }catch(Exception ex){
      
      } 
    }
    s = null;
  }

} 
