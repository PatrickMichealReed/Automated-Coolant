#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>

#include "Universum_TFTColours.h"

MCUFRIEND_kbv tft;
#include <TouchScreen.h>
#define MINPRESSURE 200
#define MAXPRESSURE 1000


#define indicatorRect(a) fillRect(140, 0, 40, 40, a)
#define clearScr() fillScreen(WHITE);

// ALL Touch panels and wiring is DIFFERENT
// copy-paste results from TouchScreen_Calibr_native.ino
const int XP=6, XM=A2, YP=A1, YM=7; //320x480 ID=0x1581
const int TS_LEFT=923, TS_RT=177, TS_TOP=964, TS_BOT=178;

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

Adafruit_GFX_Button NTRX_btn, MAT_btn, LB_btn, LT_btn, usage_btn, back_btn;

int currentPage = 0;

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


String NTRX = "NTRX";
String MAT = "MAT";
String LB = "LB";
String LT = "LT";
String currentMachine;

int alternate = 0;
int week[] = {7,7};
int month[] = {30,30};
int year[] = {12,365};

void setup(void)
{
    Serial.begin(9600);
    
    uint16_t ID = tft.readID();
    Serial.print("TFT ID = 0x");
    Serial.println(ID, HEX);
    Serial.println("Calibrate for your Touch Panel");
    if (ID == 0xD3D3) ID = 0x9486; // write-only shield
    tft.begin(ID);
    
    tft.setRotation(0);            //PORTRAIT
    tft.clearScr();

    drawHomeScreen();
}

void loop(void)
{
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
            drawMachinePage(NTRX);
        }
        if (MAT_btn.justPressed()) {
            MAT_btn.drawButton(true);
            currentPage = 1; //sets page to MAT
            tft.clearScr();
            drawMachinePage(MAT);
        }
        if (LB_btn.justPressed()) {
            LB_btn.drawButton(true);
            currentPage = 1; //sets page to LB
            tft.clearScr();
            drawMachinePage(LB);
        }
        if (LT_btn.justPressed()) {
            LT_btn.drawButton(true);
            currentPage = 1; //sets page to LT
            tft.clearScr();
            drawMachinePage(LT);
            
        }
    }
    if (currentPage == 1) {
        bool down = Touch_getXY();
        usage_btn.press(down && usage_btn.contains(pixel_x, pixel_y));
        back_btn.press(down && back_btn.contains(pixel_x, pixel_y));
        
        if (usage_btn.justPressed()) {
            usage_btn.drawButton(true);
            currentPage = 2; //sets page to coolant usage graph
            tft.fillScreen(WHITE);
            tft.setRotation(1);
            drawGraph(year);
        }
        if (back_btn.justPressed()) {
            back_btn.drawButton(true);
            currentPage = 0; //sets page to NTRX
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

void drawHomeScreen() {
  // Main Screen
    tft.setCursor(10, 10);
    tft.setTextColor(BLACK);  tft.setTextSize(1);
    tft.println("14/10/2021,10:32 PM");

    tft.drawLine(10, 60, 310, 60, BLACK);
    tft.drawLine(10, 165, 310, 165, BLACK);
    tft.drawLine(10, 270, 310, 270, BLACK);
    tft.drawLine(10, 375, 310, 375, BLACK);
  
    NTRX_btn.initButton(&tft,  160, 113, 320, 100, WHITE, WHITE, BLUE, "NTRX", 2);
    MAT_btn.initButton(&tft, 160, 218, 320, 100, WHITE, WHITE, BLUE, "MAT", 2);
    LB_btn.initButton(&tft,  160, 323, 320, 100, WHITE, WHITE, BLUE, "LB", 2);
    LT_btn.initButton(&tft, 160, 428, 320, 100, WHITE, WHITE, BLUE, "LT", 2);
    NTRX_btn.drawButton(false);
    MAT_btn.drawButton(false);
    LB_btn.drawButton(false);
    LT_btn.drawButton(false);
}

void drawMachinePage(String machine) {
  // Machine Directories
    currentMachine = machine;
    tft.setCursor(115+50/machine.length(), 10);
    tft.setTextColor(WHITE);  tft.setTextSize(3);
    tft.println(machine);

    tft.drawLine(10, 60, 310, 60, RED);
    
    usage_btn.initButton(&tft,  160, 200, 200, 80, WHITE, CYAN, BLACK, "Coolant Usage", 2);
    usage_btn.drawButton(false);
    backButton();    
}

void fillingCoolant() {  
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
