// Universum | Universum Projects > TFTHistoryGraph

// Andrei Florian 14/JUN/2018
#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
#include <DHT.h>

MCUFRIEND_kbv tft;
#include <TouchScreen.h>

#define MINPRESSURE 200
#define MAXPRESSURE 1000

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define PURPLE  0x8010
#define GREY    0xC618
#define ORANGE  0xFC00


#define indicatorRect(a) fillRect(140, 0, 40, 40, a)
#define clearScr() fillScreen(BLACK);

// ALL Touch panels and wiring is DIFFERENT
// copy-paste results from TouchScreen_Calibr_native.ino
const int XP=6, XM=A2, YP=A1, YM=7; //320x480 ID=0x1581
const int TS_LEFT=923, TS_RT=177, TS_TOP=964, TS_BOT=178;

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

// Global variables
  int valueBlock[500];
  int timeBlock[500];
  int locationBlock[500];
  int valuePos = 0;
  int blockPos = 0;
  float temp;
  int chk;
  DHT dht(2,DHT11);
  
// Editable Variables
  bool proDebug = 0;

  uint16_t graphColor = BLUE;
  uint16_t pointColor = BLACK;
  uint16_t lineColor = BLACK;

  String graphName = "Coolant Level [%]";

  int graphRange = 100;
  int markSize = 3;
  const int numberOfMarksY = 10;
  const int numberOfMarksX = 12;
  
// Calculate Values
  const int originX = 45;
  const int originY = 280;
  const int sizeX = 400;
  const int sizeY = 230;
  const int deviation = 30;
  
  int boxSize = (sizeX / numberOfMarksX);
  int mark[numberOfMarksX];
  
  const int minorSizeY = (originY + 10);
  const int minorSizeX = (originX - 10);

  int numberSize = (sizeY / numberOfMarksY);
  int number[numberOfMarksY];
  int val[numberOfMarksY];

void drawGraph()
{
  // draw title
  tft.setCursor(90, 15); // set the cursor
  tft.setTextColor(BLUE); // set the colour of the text
  tft.setTextSize(3); // set the size of the text
  tft.println(graphName);
  
  // draw outline
  tft.drawLine(originX, originY, (originX + sizeX), originY, graphColor);
  tft.drawLine(originX, originY, originX, (originY - sizeY), graphColor);

  // draw lables
  for(int i = 1; i <= numberOfMarksX; i++)
  {
    mark[i-1] = (boxSize*i + deviation);
    tft.drawLine(mark[i-1], originY, mark[i-1], minorSizeY, graphColor);
  }

  // draw numbers
  for(int i = 1; i <= numberOfMarksY; i++)
  {
    number[i-1] = numberSize*i;
    tft.drawLine(originX, (originY - number[i-1]), minorSizeX, (originY - number[i-1]), graphColor);
  }
  // draw number values
  for(int i = 1; i <= numberOfMarksY; i++)
  {
    val[i-1] = graphRange*(numberOfMarksY-i+1)/numberOfMarksY;
    tft.setCursor((minorSizeX - 25), (originY - sizeY*(numberOfMarksY-i+1)/numberOfMarksY));
    tft.setTextColor(graphColor);
    tft.setTextSize(1);
    tft.println(val[i-1]);
  }
  
}

void graph()
{
  //chk = dht.read(22);
  //temp = dht.readTemperature();
  temp = random(5,graphRange);
  timeBlock[valuePos] = (millis() / 1000);

  valueBlock[valuePos] = temp;
  
  if(proDebug)
  {
    Serial.println(timeBlock[valuePos]);
  }

  if(blockPos < numberOfMarksX)
  {
    // print the time
    tft.setCursor((mark[valuePos] - 5), (originY + 16));
    tft.setTextColor(graphColor, WHITE);
    tft.setTextSize(1);
    tft.println(timeBlock[valuePos]);
    
    
    // map the value
    locationBlock[valuePos] = map(temp, 0, graphRange, originY, (originY - sizeY));

    // draw point
    tft.fillRect((mark[valuePos] - 1), (locationBlock[valuePos] - 1), markSize, markSize, pointColor);

    // try connecting to previous point
    if(valuePos != 0)
    {
      tft.drawLine(mark[valuePos], locationBlock[valuePos], mark[(valuePos - 1)], locationBlock[(valuePos - 1)], lineColor);
    }
    
    blockPos++;
  }
  else
  {
    // clear the graph's canvas
    tft.fillRect((originX + 2), (originY - sizeY), sizeX, sizeY, WHITE);

    // map the value - current point
    locationBlock[valuePos] = map(temp, 0, graphRange, originY, (originY - sizeY));

    // draw point - current point
    tft.fillRect((mark[numberOfMarksX-1]), (locationBlock[valuePos] - 1), markSize, markSize, pointColor);

    // draw all points
    for(int i = 0; i < numberOfMarksX; i++)
    {
      tft.fillRect((mark[(blockPos - (i + 1))] - 1), (locationBlock[(valuePos - i)] - 1), markSize, markSize, pointColor);
    }

    // draw all the lines
    for(int i = 0; i < numberOfMarksX-1; i++)
    {
      tft.drawLine(mark[blockPos - (i + 1)], locationBlock[valuePos - i], mark[blockPos - (i + 2)], locationBlock[valuePos - (i + 1)], lineColor);
    }
    
    // change time lables
    for(int i = 0; i < numberOfMarksX; i++)
    {
      tft.setCursor((mark[(numberOfMarksX - 1 - i)] - 5), (originY + 16));
      tft.setTextColor(graphColor, WHITE);
      tft.setTextSize(1);
      tft.println(timeBlock[valuePos - i]);
    }
  }

  valuePos++;
}

void setup()
{
  dht.begin();
  if(proDebug)
  {
    Serial.begin(9600);
    while(!Serial) {};
  }
  
  tft.reset();
  delay(500);
  uint16_t identifier = tft.readID();
  identifier=0x1581;

  tft.begin(identifier);
  tft.setRotation(1);

  //drawHome();
  tft.fillScreen(WHITE);
  drawGraph();
}

void loop()
{
  graph();
  delay(3000);
}
