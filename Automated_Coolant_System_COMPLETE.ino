#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
#include <Stepper.h>
#include <Servo.h>

#include "Universum_TFTColours.h"

MCUFRIEND_kbv tft;
#include <TouchScreen.h>
#define MINPRESSURE 200
#define MAXPRESSURE 1000


#define indicatorRect(a) fillRect(140, 0, 40, 40, a)
#define clearScr() fillScreen(WHITE);

Servo myservo;
int pos = 0;

// ALL Touch panels and wiring is DIFFERENT
// copy-paste results from TouchScreen_Calibr_native.ino
const int XP=6, XM=A2, YP=A1, YM=7; //320x480 ID=0x1581
const int TS_LEFT=923, TS_RT=177, TS_TOP=964, TS_BOT=178;

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

Adafruit_GFX_Button NTRX_btn, MAT_btn, LB_btn, LT_btn, usage_btn, back_btn, week_btn, month_btn, year_btn, up_btn, down_btn;




int pixel_x, pixel_y;     //Touch_getXY() updates global vars
bool Touch_getXY(void)
{
    TSPoint p = ts.getPoint();
    pinMode(YP, OUTPUT);      //restore shared pins
    pinMode(XM, OUTPUT);
    digitalWrite(YP, HIGH);   //because TFT control pins
    digitalWrite(XM, HIGH);
    bool pressed = (p.z > MINPRESSURE && p.z < MAXPRESSURE);
    if (pressed) {
        pixel_x = map(p.x, TS_LEFT, TS_RT, 0, tft.width()); //.kbv makes sense to me
        pixel_y = map(p.y, TS_TOP, TS_BOT, 0, tft.height());
    }
    return pressed;
}

//Shared Variables
const int trig = 0;
const int echo = 1;
const int con = 2;
const int valve = 3;
int currentConcentration = 0;

//Sensor Variables
bool fillingCoolant = false;

long NTRX_level;
long MAT_level;
long LB_level;
long LT_level;

int trigger_pin;
int echo_pin;
int concentration;
int valve_pin;
int machine[4];

// machine[trigger_pin, echo_pin, concentration, valve_pin]
int NTRX[] = {2,3,9,3};
int MAT[] = {4,5,2,3};
int LT[] = {0,0,0,0};
int LB[] = {0,0,0,0};

//Touchscreen Global Variables
String NTRX_name = "NTRX";
String MAT_name = "MAT";
String LB_name = "LB";
String LT_name = "LT";
String currentMachine;

int currentPage = 0;
int alternate = 0;
int week[] = {7,7};
int month[] = {10,30};
int year[] = {12,365};


void setup(void)
{
    Serial.begin(9600);
    
    uint16_t ID = tft.readID();
    Serial.print("TFT ID = 0x");
    Serial.println(ID, HEX);
    Serial.println("Calibrate for your Touch Panel");
    if (ID == 0xD3D3) ID = 0x9486; // print-only shield
    tft.begin(ID);
    
    tft.setRotation(0);            //PORTRAIT
    tft.clearScr();

    drawHomeScreen();
}

void loop(void)
{
    if (fillingCoolant){
        NTRX_level = check_level(NTRX);
          if(NTRX_level < 100){
             digitalWrite(NTRX[valve], LOW); //closes NTRX valve;
          }
        MAT_level = check_level(MAT);
          if(MAT_level < 100){
            digitalWrite(MAT[valve], LOW); //closes MAT valve
          }
        LT_level = check_level(LT);
          if(NTRX_level < 100){
             digitalWrite(LT[valve], LOW); //closes NTRX valve;
          }
        LB_level = check_level(LB);
          if(MAT_level < 100){
            digitalWrite(LB[valve], LOW); //closes MAT valve
          }
          if(NTRX_level < 100 && MAT_level < 100 && LT_level < 100 && LB_level < 100){
            fillingCoolant = false;
          }
    }else{
        NTRX_level = check_level(NTRX);
          if(NTRX_level >= 100){
             addCoolant(NTRX);
             goto skip;
          }
        MAT_level = check_level(MAT);
          if(MAT_level >= 100){
            addCoolant(MAT);
            goto skip;
          }
        LT_level = check_level(LT);
          if(LT_level >= 100){
             addCoolant(LT);
             goto skip;
          }
        LB_level = check_level(LB);
          if(LB_level >= 100){
            addCoolant(LB);
            goto skip;
          }
    }
    //Only one valve should be open at a time, so code skips checking other levels if one is low on coolant
    skip:
    
    if (currentPage == 0) {
        bool down = Touch_getXY();
        NTRX_btn.press(down && NTRX_btn.contains(pixel_x, pixel_y));
        MAT_btn.press(down && MAT_btn.contains(pixel_x, pixel_y));
        LB_btn.press(down && LB_btn.contains(pixel_x, pixel_y));
        LT_btn.press(down && LT_btn.contains(pixel_x, pixel_y));
                
        if (NTRX_btn.justPressed()) {
            NTRX_btn.drawButton(true);
            currentPage = 1; //sets page to NTRX
            tft.clearScr();
            drawMachinePage(NTRX_name);
        }
        if (MAT_btn.justPressed()) {
            MAT_btn.drawButton(true);
            currentPage = 1; //sets page to MAT
            tft.clearScr();
            drawMachinePage(MAT_name);
        }
        if (LB_btn.justPressed()) {
            LB_btn.drawButton(true);
            currentPage = 1; //sets page to LB
            tft.clearScr();
            drawMachinePage(LB_name);
        }
        if (LT_btn.justPressed()) {
            LT_btn.drawButton(true);
            currentPage = 1; //sets page to LT
            tft.clearScr();
            drawMachinePage(LT_name);
            
        }
    }
    if (currentPage == 1) {
        bool down = Touch_getXY();
        week_btn.press(down && week_btn.contains(pixel_x, pixel_y));
        month_btn.press(down && month_btn.contains(pixel_x, pixel_y));
        year_btn.press(down && year_btn.contains(pixel_x, pixel_y));
        back_btn.press(down && back_btn.contains(pixel_x, pixel_y));
        up_btn.press(down && up_btn.contains(pixel_x, pixel_y));
        down_btn.press(down && down_btn.contains(pixel_x, pixel_y));
        
        if (week_btn.justPressed()) {
            week_btn.drawButton(true);
            currentPage = 2; //sets page to coolant usage graph
            tft.fillScreen(WHITE);
            tft.setRotation(1);
            drawGraph(week);
        }
        if (month_btn.justPressed()) {
            month_btn.drawButton(true);
            currentPage = 2; //sets page to coolant usage graph
            tft.fillScreen(WHITE);
            tft.setRotation(1);
            drawGraph(month);
        }
        if (year_btn.justPressed()) {
            year_btn.drawButton(true);
            currentPage = 2; //sets page to coolant usage graph
            tft.fillScreen(WHITE);
            tft.setRotation(1);
            drawGraph(year);
        }
        if (up_btn.justPressed() && currentConcentration != 10) {
            upButton();
        }
        if (down_btn.justPressed() && currentConcentration != 0) {
            downButton();
        }
        if (back_btn.justPressed()) {
            back_btn.drawButton(true);
            currentPage = 0; 
            tft.clearScr();
            drawHomeScreen();
        }
    }
    if (currentPage == 2) {
        bool down = Touch_getXY();
        back_btn.press(down && back_btn.contains(pixel_x, pixel_y));

        if (back_btn.justPressed()) {
            back_btn.drawButton(true);
            currentPage = 1; //sets page to NTRX
            tft.clearScr();
            tft.setRotation(0);
            drawMachinePage(currentMachine);
        }
    }
}

//Sensor Functions

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

void addCoolant(int machine[]){
  adjustConcentration(machine[con]);
  digitalWrite(machine[valve], HIGH);
  fillingCoolant = true;
}

void adjustConcentration(int concentration){
  // changes concentration
    myservo.write(18*concentration);
}

//LCD Screen Functions

void drawHomeScreen() {
  // Main Screen
    tft.setCursor(10, 10);
    tft.setTextColor(BLACK);  tft.setTextSize(1);
    tft.println("14/10/2021,10:32 PM");

    tft.setCursor(18, 25);
    tft.setTextColor(BLACK);  tft.setTextSize(2);
    tft.println("Automated Coolant System");

    tft.drawLine(10, 50, 310, 50, BLACK);
    tft.drawLine(10, 155, 310, 155, BLACK);
    tft.drawLine(10, 260, 310, 260, BLACK);
    tft.drawLine(10, 365, 310, 365, BLACK);
    tft.drawLine(10, 470, 310, 470, BLACK);
  
    NTRX_btn.initButton(&tft,  160, 103, 320, 100, WHITE, WHITE, BLACK, "NTRX", 4);
    MAT_btn.initButton(&tft, 160, 208, 320, 100, WHITE, WHITE, BLACK, "MAT", 4);
    LB_btn.initButton(&tft,  160, 313, 320, 100, WHITE, WHITE, BLACK, "LB", 4);
    LT_btn.initButton(&tft, 160, 418, 320, 100, WHITE, WHITE, BLACK, "LT", 4);
    NTRX_btn.drawButton(false);
    MAT_btn.drawButton(false);
    LB_btn.drawButton(false);
    LT_btn.drawButton(false);
}

void drawMachinePage(String machine) {
  // Machine Directories
    currentMachine = machine;
    setConcentration();
    tft.setCursor(115+50/machine.length(), 10);
    tft.setTextColor(BLACK);  tft.setTextSize(3);
    tft.println(machine);

    tft.drawLine(10, 60, 310, 60, BLUE);
    //tft.drawLine(10, 120, 310, 120, BLUE);

    tft.setCursor(80, 95);
    tft.setTextColor(BLACK);  tft.setTextSize(2);
    tft.println("Coolant Usage");

    tft.drawLine(107, 125, 107, 195, BLUE);
    tft.drawLine(213, 125, 213, 195, BLUE);


    week_btn.initButton(&tft,  53, 160, 103, 65, WHITE, WHITE, BLACK, "Week", 2);
    week_btn.drawButton(false);
    month_btn.initButton(&tft,  160, 160, 103, 65, WHITE, WHITE, BLACK, "Month", 2);
    month_btn.drawButton(false);
    year_btn.initButton(&tft,  267, 160, 103, 65, WHITE, WHITE, BLACK, "Year", 2);
    year_btn.drawButton(false);

    up_btn.initButton(&tft,  160, 300, 80, 80, WHITE, WHITE, BLACK, "", 3);
    up_btn.drawButton(false);
    down_btn.initButton(&tft,  160, 425, 80, 80, WHITE, WHITE, BLACK, "", 3);
    down_btn.drawButton(false);

    //up button
    tft.fillTriangle(130,325,160,275,190,325,BLACK);
    tft.drawTriangle(130,325,160,275,190,325,BLACK);
    //down button
    tft.fillTriangle(130,400,160,450,190,400,BLACK);
    tft.drawTriangle(130,400,160,450,190,400,BLACK);

    tft.setCursor(150, 350);
    tft.setTextColor(ORANGE);  
    tft.setTextSize(3);
    tft.print(currentConcentration);

    backButton();    
}

void fillingCoolantPrompt() {  
  if(alternate == 1){
    tft.setCursor(5, 470);
    tft.setTextColor(BLACK);  tft.setTextSize(1);
    tft.println("Filling Coolant");
    alternate =0;
  } else {
    tft.setCursor(5, 470);
    tft.setTextColor(WHITE);  tft.setTextSize(1);
    tft.println("Filling Coolant");
    alternate = 1;
  }
}

void backButton () {
  back_btn.initButton(&tft, 40, 20, 80, 40, WHITE, RED, WHITE, "<-BACK", 1);
  back_btn.drawButton(false);
}

void setConcentration() {
  if(currentMachine == "NTRX") currentConcentration = NTRX[con];
  if(currentMachine == "MAT") currentConcentration = MAT[con];
  if(currentMachine == "LB") currentConcentration = LB[con];
  if(currentMachine == "LT") currentConcentration = LT[con];
}

void setMachineConcentration() {
  if(currentMachine == "NTRX") NTRX[con] = currentConcentration;
  if(currentMachine == "MAT") MAT[con] = currentConcentration;
  if(currentMachine == "LB") LB[con] = currentConcentration;
  if(currentMachine == "LT") LT[con] = currentConcentration;
}

void upButton() {
  //up_btn.drawButton(true);
  //up_btn.drawButton(false);
  tft.fillTriangle(135,320,160,280,185,320,WHITE);
  tft.setCursor(150, 350);
  tft.setTextColor(WHITE);  
  tft.setTextSize(3);
  tft.print(currentConcentration);
  currentConcentration ++;
  tft.setCursor(150, 350);
  tft.setTextColor(ORANGE);  
  tft.setTextSize(3);
  tft.print(currentConcentration);
  setMachineConcentration();
  tft.fillTriangle(135,320,160,280,185,320,BLACK);
}

void downButton() {
  //down_btn.drawButton(true);
  //down_btn.drawButton(false);
  tft.fillTriangle(135,405,160,445,185,405,WHITE);
  tft.setCursor(150, 350);
  tft.setTextColor(WHITE);  
  tft.setTextSize(3);
  tft.print(currentConcentration);
  currentConcentration --;
  tft.setCursor(150, 350);
  tft.setTextColor(ORANGE);  
  tft.setTextSize(3);
  tft.print(currentConcentration);
  setMachineConcentration();
  tft.fillTriangle(135,405,160,445,185,405,BLACK);
}
