#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>

MCUFRIEND_kbv tft;
#include <TouchScreen.h>
#define MINPRESSURE 200
#define MAXPRESSURE 1000


#define indicatorRect(a) fillRect(140, 0, 40, 40, a)
#define clearScr() fillScreen(BLACK);

// ALL Touch panels and wiring is DIFFERENT
// copy-paste results from TouchScreen_Calibr_native.ino
const int XP=6, XM=A2, YP=A1, YM=7; //320x480 ID=0x1581
const int TS_LEFT=923, TS_RT=177, TS_TOP=964, TS_BOT=178;

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

Adafruit_GFX_Button NTRX_btn, MAT_btn, LB_btn, LT_btn, conc_btn, back_btn;

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

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

int currentPage = 0;
String NTRX = "NTRX";
String MAT = "MAT";
String LB = "LB";
String LT = "LT";

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
    tft.fillScreen(BLACK);

    drawHomeScreen();
    tft.setCursor(5, 470);
    tft.setTextColor(WHITE);  tft.setTextSize(1);
    tft.println("Filling Coolant");
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
        conc_btn.press(down && conc_btn.contains(pixel_x, pixel_y));
        back_btn.press(down && back_btn.contains(pixel_x, pixel_y));

        
        if (conc_btn.justReleased())
            conc_btn.drawButton();
        if (back_btn.justReleased())
            back_btn.drawButton();
        
        if (conc_btn.justPressed()) {
            conc_btn.drawButton(true);
        }
        if (back_btn.justPressed()) {
            back_btn.drawButton(true);
            currentPage = 0; //sets page to NTRX
            tft.clearScr();
            drawHomeScreen();
        }
    }
}

void drawHomeScreen() {
  // Main Screen
    NTRX_btn.initButton(&tft,  160, 100, 200, 80, WHITE, CYAN, BLACK, "NTRX", 2);
    MAT_btn.initButton(&tft, 160, 200, 200, 80, WHITE, CYAN, BLACK, "MAT", 2);
    LB_btn.initButton(&tft,  160, 300, 200, 80, WHITE, CYAN, BLACK, "LB", 2);
    LT_btn.initButton(&tft, 160, 400, 200, 80, WHITE, CYAN, BLACK, "LT", 2);
    NTRX_btn.drawButton(false);
    MAT_btn.drawButton(false);
    LB_btn.drawButton(false);
    LT_btn.drawButton(false);
    tft.indicatorRect(BLACK);
}

void drawMachinePage(String machine) {
  // Machine Directories
    Serial.print(machine.length());
    tft.setCursor(115+50/machine.length(), 10);
    tft.setTextColor(WHITE);  tft.setTextSize(3);
    tft.println(machine);

    tft.drawLine(10, 60, 310, 60, RED);
    
    conc_btn.initButton(&tft,  160, 200, 200, 80, WHITE, CYAN, BLACK, "Conc.", 2);
    back_btn.initButton(&tft, 40, 20, 80, 40, WHITE, RED, BLACK, "<- BACK", 1);
    
    conc_btn.drawButton(false);
    back_btn.drawButton(false);
    
}
