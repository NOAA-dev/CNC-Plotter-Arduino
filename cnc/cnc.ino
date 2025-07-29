#include <Stepper.h>
struct point{
float x;
float y;
float z;
};
struct point actuatorPos;
bool debug = true;

//servo
// int servopwm = 6; 
//Stepper
#define STEPDELAY 400  // 2 milliseconds

float round2(float val) {
  //return round(val * 100.0) / 100.0;
  return val;
}

//SX
int StepsPerMillimeterX=52;
Stepper SX(StepsPerMillimeterX,8,10,9,11);

//SY
int StepsPerMillimeterY=50 ;
Stepper SY(StepsPerMillimeterY,2,4,3,5);
//SZ
int stepz = 512;
Stepper SZ(stepz,6,12,7,13);


int penupangle = 0;     //for servo 90
int pendownangle = 1;   //180

//Drawing area milimeter
int Xmin = 0;
int Ymin=0;
int Zmin=0;
int Xmax= 100;
int Ymax=100;
int Zmax = 1;
float Xpos{0} ,Ypos{0};
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  //Servo
 // pinMode(12,OUTPUT);
//  pinMode(servopwm,OUTPUT);
  //stepper

  SZ.setSpeed(50);
  //SX
  SX.setSpeed(50);
  //SY
  SY.setSpeed(50);
  //setup
  penupdown(penupangle);
  Serial.println("CNC machine ready");
}

void loop() {
  delay(100);
  char line[512];
  char c;
  int lineIndex;
  bool lineIsComment, lineSemiColon;

  lineIndex = 0;
  lineSemiColon = false;
  lineIsComment = false;

  while (1) {

    // Serial reception 
    while ( Serial.available() > 0 ) {
      c = Serial.read();
      if (( c == '\n') || (c == '\r') ) {             // End of line reached
        if ( lineIndex > 0 ) {                        // Line is complete. Then execute!
          line[ lineIndex ] = '\0';                   // Terminate string
          if (debug) {
            Serial.print( "Received : ");
            Serial.println( line );
          }
          processIncomingLine( line, lineIndex );
          lineIndex = 0;
        }
        else {
          ///////
        }
        lineIsComment = false;
        lineSemiColon = false;
        Serial.println("ok");
      }
      else {
        if ( (lineIsComment) || (lineSemiColon) ) {   
          if ( c == ')' )  lineIsComment = false;     
        }
        else {
          if ( c <= ' ' ) {                           
          }
          else if ( c == '/' ) {                    
          }
          else if ( c == '(' ) {                    
            lineIsComment = true;
          }
          else if ( c == ';' ) {
            lineSemiColon = true;
          }
          else if ( lineIndex >= 512 - 1 ) {
            Serial.println( "ERROR - lineBuffer overflow" );
            lineIsComment = false;
            lineSemiColon = false;
          }
          else if ( c >= 'a' && c <= 'z' ) {        // Upcase lowercase
            line[ lineIndex++ ] = c - 'a' + 'A';
          }
          else {
            line[ lineIndex++ ] = c;
          }
        }
      }
    }
  }

}

void processIncomingLine( char* line, int charNB ) {
  int currentIndex = 0;
  char buffer[ 64 ];                                
  struct point newPos;

  newPos.x = 0.0;
  newPos.y = 0.0;

  while ( currentIndex < charNB ) {
    switch ( line[ currentIndex++ ] ) {             
      case 'U':
        penupdown(penupangle);
        break;
      case 'D':
        penupdown(pendownangle);
        break;
      case 'G':
        buffer[0] = line[ currentIndex++ ];         
        //      buffer[1] = line[ currentIndex++ ];
        //      buffer[2] = '\0';
        buffer[1] = '\0';

        switch ( atoi( buffer ) ) {                 
          case 0:                                   
          case 1:
            // /!\ Suppose that X is before Y
            char* indexX = strchr( line + currentIndex, 'X' ); // Get X/Y position in the string 
            char* indexY = strchr( line + currentIndex, 'Y' );
            if ( indexY <= 0 ) {
              newPos.x = round2(atof( indexX + 1));
              newPos.y = actuatorPos.y;
            }
            else if ( indexX <= 0 ) {
              newPos.y = round2(atof( indexY + 1));
              newPos.x = actuatorPos.x;
            }
            else {
              newPos.y = round2(atof( indexY + 1));
              newPos.x = round2(atof( indexX + 1));
            }
            draw(newPos.x, newPos.y );
            //        Serial.println("ok");
            actuatorPos.x = newPos.x;
            actuatorPos.y = newPos.y;
            break;
        }
        break;
      case 'M':
        buffer[0] = line[ currentIndex++ ];        
        buffer[1] = line[ currentIndex++ ];
        buffer[2] = line[ currentIndex++ ];
        buffer[3] = '\0';
        switch ( atoi( buffer ) ) {
          case 300:
            {
              char* indexS = strchr( line + currentIndex, 'S' );
              float Spos = atof( indexS + 1);
              //         Serial.println("ok");
              if (Spos == 30) {
                penupdown(pendownangle);
              }
              if (Spos == 50) {
                penupdown(penupangle);
              }
              break;
            }
          case 114:                                // M114 - Repport position
            Serial.print( "Absolute position : X = " );
            Serial.print( actuatorPos.x );
            Serial.print( "  -  Y = " );
            Serial.println( actuatorPos.y );
            break;
          default:
            Serial.print( "Command not recognized : M");
            Serial.println( buffer );
        }
    }
  }
}


void penupdown(int angle){
//for servo
// int on = map(angle,0,180,0,2000);
// int off = 20000 - on;
// for(int i =0;i<100;i++){
//   digitalWrite(servopwm,1);
//   delayMicroseconds(on);
//   digitalWrite(servopwm,0);
//   delayMicroseconds(off);
// }
// delayMicroseconds(500);
if(angle==1){
  SZ.step(stepz*(1));
}
else if(angle==0){
  SZ.step(stepz*(-1));
}
delay(200);
}
// void step(int dp,int d,int s,int no) {       /////for A4988 motor driver (Nema 17 motors)///
//   digitalWrite(dp,d);
//   for(int i=0;i<no;i++){
//     digitalWrite(s,1);
//     delayMicroseconds(2000);
//     digitalWrite(s,0);
//     delayMicroseconds(2000);
//   }
// }

void draw(float x, float y) {
  // bound
  if (x > Xmax) x = Xmax;
  if (y > Ymax) y = Ymax;
  if (x < Xmin) x = Xmin;
  if (y < Ymin) y = Ymin;

  if (debug) {
    Serial.print("From (mm): ");
    Serial.print(actuatorPos.x); Serial.print(", ");
    Serial.print(actuatorPos.y); Serial.print("  ->  To (mm): ");
    Serial.print(x); Serial.print(", ");
    Serial.println(y);
  }

  // Convert mm to steps
  long x0 = actuatorPos.x * StepsPerMillimeterX;
  long y0 = actuatorPos.y * StepsPerMillimeterY;
  long x1 = x * StepsPerMillimeterX;
  long y1 = y * StepsPerMillimeterY;

  long delx = abs(x1 - x0);
  long dely = abs(y1 - y0);
  if (delx == 0 && dely == 0) return;

  int dx = (x1 >= x0) ? 1 : -1;
  int dy = (y1 >= y0) ? 1 : -1;

  long over = 0;

  if (delx > dely) {
    for (long i = 0; i <= delx; i++) {
      SX.step(dx*1);
      delayMicroseconds(STEPDELAY);
      over += dely;
      if (over >= delx) {
        over -= delx;
        SY.step(dy*1);
        delayMicroseconds(STEPDELAY);
      }
      
    }
  } else {
    for (long i = 0; i <= dely; i++) {
      SY.step(dy*1);
      delayMicroseconds(STEPDELAY);
      over += delx;
      if (over >= dely) {
        over -= dely;
        SX.step(dx*1);
        delayMicroseconds(STEPDELAY);
      }
      
    }
  }

  // Update actual position in mm
  actuatorPos.x = x;
  actuatorPos.y = y;
}
