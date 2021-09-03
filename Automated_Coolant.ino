  // put your setup code here, to run once:

#define MAX_DISTANCE 200

// STEPPER MOTOR SETUP
#include <Stepper.h>
#include <Servo.h>
Servo myservo;

int pos =0;

const int stepsPerPercent = 90;
// change this to fit the number of steps per revolution
// for your motor
int current_step = 0;
int steps;
// initialize the stepper library on pins 8 through 11:
Stepper myStepper(stepsPerPercent, 8, 9, 10, 11);
  
long NTRX_level;
long MAT_level;
long LB_level;
long LT_level;


int trigger_pin;
int echo_pin;
int concentration;
int valve_pin;
int machine[4];


// write some sort of code that takes information from a terminal (desired concentration to be added to each machine)
// and then writes that information into respective variables (i.e., NTRX(3))

// INITIALIZES MACHINES TO CORRESPONDING PINS
// machine[trigger_pin, echo_pin, concentration, valve_pin]
int NTRX[] = {2,3,9,7};
int MAT[] = {4,5,2,8};
//int LB[] = {a,b,c,d};
//int LT[] = {a,b,c,d};
  
void setup() {

  myservo.attach(10);
  // set the speed at 60 rpm:
  myStepper.setSpeed(5);
  // initialize the serial port:
  Serial.begin(9600);

  pinMode(NTRX[0],OUTPUT);
  pinMode(NTRX[1],INPUT);
  pinMode(NTRX[3],OUTPUT);
  pinMode(MAT[0],OUTPUT);
  pinMode(MAT[1],INPUT);
  pinMode(MAT[3],OUTPUT);

  pinMode(7,OUTPUT);
  
}

void loop() {
  // put your main code here, to run repeatedly:
delay(1000);
  
  NTRX_level = check_level(NTRX);
  // fills NTRX coolant until sensor reads high_coolant
  if(NTRX_level >= 100){
    adjust_concentration(NTRX[2]);
  }
  while(NTRX_level >= 100){
    add_coolant(NTRX);
    NTRX_level = check_level(NTRX);
  }
  digitalWrite(NTRX[3], LOW); //close NTRX valve


  MAT_level = check_level(MAT);
  //fills MAT coolant until sensor reads high_coolant
  if(MAT_level >= 100){
    adjust_concentration(MAT[2]);
  }
  while(MAT_level >= 100){
    add_coolant(MAT);
    MAT_level = check_level(MAT);
  }
  digitalWrite(MAT[3], LOW); //close MAT valve
/*
  LB_level = check_level(LB);
  //fills LB coolant until sensor reads high_coolant
  while(LB_level == "low_coolant"){
    add_coolant(LB);
    LB_level = check_level(LB);
  }
  digitalWrite(LB[4], LOW); //close LB valve

  LT_level = check_level(LT);
  //fills LT coolant until sensor reads high_coolant
  while(LT_level == "low_coolant"){ 
    add_coolant(LT);
    LT_level = check_level(LT);
  }
  digitalWrite(LT[4], LOW); //close LT valve
  */  
  
}


long check_level(int machine[]){

  trigger_pin = machine[0];
  echo_pin = machine[1];
  
  long coolant_level;
  long duration, inches, mm;
  
  digitalWrite(trigger_pin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigger_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger_pin, LOW);

  duration = pulseIn(echo_pin, HIGH);
  inches = microsecondsToInches(duration);
  mm = microsecondsToMillimeters(duration);

  delay(100);

  coolant_level = mm;
    if (mm < 100){ 
          Serial.println (" Full Coolant ");
          Serial.print (" Distance= ");              
          Serial.print (mm); 
          Serial.println (" mm");     
    } else {
          Serial.println (" Low Coolant ");
          Serial.print (" Distance= "); 
          Serial.print (mm);              
          Serial.println (" mm");       
    } 
    
  return coolant_level;
}

long microsecondsToInches(long microseconds) {
   return microseconds / 74 / 2;
}

long microsecondsToMillimeters(long microseconds) {
   return microseconds * 10 / 29 / 2;
}

void add_coolant(int machine[]){

  // changes concentration
  /*
  steps = machine[3]*stepsPerPercent - current_step; // calculates steps to reach concentration
  current_step = stepsPerPercent*machine[3]; // logs stepper position
  myStepper.step(steps*machine[3]); // steps to desired concentration
  delay(500);
  */
  digitalWrite(machine[3], HIGH);
  delay(1000);    
}
void adjust_concentration(int concentration){
    myservo.write(18*concentration);
}
