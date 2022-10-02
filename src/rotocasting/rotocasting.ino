#include <TimerOne.h>
#include <Wire.h>
#include <MultiFuncShield.h>

//encoder stuff

#define encpinbtn A5
#define encpinA 9
#define encpinB 6

int aState; // for knob
int bState;
int aLastState;
int counter = 0;
int knobsens = 4;
boolean encbstate=false; //for button
boolean encbprevstate=true;

//h bridge stuff

#define pinpwm 5
#define maxpwm 255 //absolute maximum speed
#define minpwm 40
int pwmpow = 0;
int toppwm = 55; //default max spinning speed

//screen stuff

char disp[4];
int mode = 0; // screen modes 0=time set, 1=speed set, 2=spinning

//spinning stuff

int acceltime=2000; //time between speed increments when accelerating
int minutes=15; //default spinning time
int deltav=1; //acceleration in pwm units
unsigned long currentime=0; //timer
unsigned long spintimer=0; //time when spinning starts
unsigned long ttime=0; //total time to sipin
unsigned long countdowntime=0; //countdown clock to show on display

int fase = -1;

void setup() {
  
  Timer1.initialize();
  MFS.initialize(&Timer1);
  MFS.beep();
  MFS.write("on");
  delay(1000);
  MFS.beep(5,5,4,1,50);
  pinMode(pinpwm,OUTPUT);
  pinMode(encpinbtn,INPUT);
  pinMode(encpinA,INPUT);
  pinMode(encpinB,INPUT);
  //Serial.begin(9600);

}

void loop() {

updatescreen();
inputbuttons();
encoderbutton();
knob();
rotocasting();

}

void updatescreen(){

switch (mode){
  
  case 0:{ //timeset
  disp[0]='t';
  disp[1]='m';
  disp[2]=char((minutes/10)+48);
  disp[3]=char((minutes%10)+48); 
    break;}
  case 1:{ //speedset
  disp[0]='p';
  disp[1]=char((toppwm/100)+48);
  disp[2]=char(((toppwm%100)/10)+48);
  disp[3]=char(((toppwm%100)%10)+48);
    break;}
    
  case 2:{ //spinning
    
    if (fase==0){
      disp[0]='s';
      disp[1]='p';
      disp[2]='u';
      disp[3]='p';
    }
    if (fase==1){

    }
    if (fase==2){
      disp[0]='s';
      disp[1]='p';
      disp[2]='d';
      disp[3]='n';
    }

    if (fase==3){
      disp[0]='d';
      disp[1]='o';
      disp[2]='n';
      disp[3]='e';
    }

    break;}

}
   
  MFS.write(disp);
  
}

void inputbuttons(){


  byte btn = MFS.getButton(); 
  
  if(btn){
    
    byte buttonNumber = btn & B00111111;
    byte buttonAction = btn & B11000000;
    
    if (buttonAction == BUTTON_LONG_PRESSED_IND){

      if(buttonNumber==1){ //manual slow rotation
          pwmpow=minpwm;
                
      }

      if(buttonNumber==2){ 
               //to be defined
      }

      if(buttonNumber==3){ 
           //to be defined
      }

    }
        else{
         pwmpow=0;
    }

  }
 
}

void encoderbutton(){

  encbstate=digitalRead(encpinbtn);
    if (encbstate==false && encbstate!=encbprevstate && mode!=2){ //mode change
      mode=mode+1;
      mode=mode%3;
      MFS.beep();
    if(mode==2){
      fase=0;
      MFS.beep(5,5,4,1,50);
      pwmpow=minpwm;
      currentime=millis();
      ttime=minutes*60000;
      encbstate=true;
      delay(1000);
    }
  }  
  encbprevstate=encbstate;
  estop();
  
}

void estop(){
     
     if (encbstate==false && (fase<3) && mode==2){ //emergency stop
      MFS.write("stop");
      MFS.beep(15,5,4,1,50);
      delay(2000);
      fase=2;  
     }
  
}

void knob(){

   aState = digitalRead(encpinA); 
   bState = digitalRead(encpinB);
 
   if (aState != aLastState){   

     counter=counter+1;
     
     if(counter==knobsens){
      
     if (bState != aState) { 

      if(mode==0){
        minutes --;
      }
      if(mode==1){
        toppwm --;
      }
     } else {
      if(mode==0){
        minutes ++;
      }
      if(mode==1){
        toppwm ++;
      }
     }

     counter=0;
     }
   } 
   
   aLastState = aState; // Updates the previous state of the outputA with the current state

  parameterslimit();

}

void parameterslimit(){
  
   if(minutes>60){
    minutes=60;
   }
   if(minutes<1){
    minutes=1;
   }
   if(toppwm>maxpwm){
    toppwm=60;
   }
   if(toppwm<minpwm){
    toppwm=40;
   }

}

void rotocasting(){

switch (fase){
  case 0:{
    accel(toppwm,deltav);
    break;}
  case 1:{
    spinning();
    break;}
  case 2:{
    accel(minpwm,-deltav);
    break;}
  case 3:{
    done();
    break;}

}

analogWrite(pinpwm,pwmpow);
  
}

void accel(int target, int var){

if(millis()-currentime>=acceltime){
  
  pwmpow=pwmpow+var;
  currentime=millis();
  constrain(pwmpow,minpwm,toppwm);
  if(pwmpow==target){
    fase=fase+1;
    spintimer=currentime;
    MFS.beep();
  }
}
  
}

void spinning(){
  countdown();
  if(countdowntime>ttime){
    fase=fase+1;
    MFS.beep();
  }
}

void countdown(){
  
  disp[0]='t';
  if(millis()-currentime>=1000){
  if(disp[1]=='m'){  
  disp[1]=' ';
  }
  else{
    disp[1]='m';
  }
  currentime=millis();
  disp[2]=char((((countdowntime+60000)/60000)/10)+48);
  disp[3]=char((((countdowntime+60000)/60000)%10)+48);
  }
  countdowntime=ttime-(millis()-spintimer);  
}

void done(){
  mode=0;
  fase=-1;
  pwmpow=0;
  MFS.beep(20,5,8,1,50);
  delay(2000);
}
