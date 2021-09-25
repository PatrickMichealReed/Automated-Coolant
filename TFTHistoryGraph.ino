// Universum | Universum Projects > TFTHistoryGraph > Automated Coolant System

// Andrei Florian 14/JUN/2018

// Altered by Patrick Reed 17/SEPT/2021

// Global variables
  int valueBlock[500];
  int timeBlock[500];
  int locationBlock[500];
  int valuePos = 0;
  int blockPos = 0;
  float temp;
  int chk;
  float coolantUse [365]; 
  
// Editable Variables
  bool proDebug = 0;

  uint16_t graphColor = BLUE;
  uint16_t pointColor = BLACK;
  uint16_t lineColor = BLACK;

  String graphName = "Coolant Usage [L/day]";

  int graphRange = 15;
  int markSize = 2;
  const int numberOfMarksY = 7;

  
// Calculate Values
  const int originX = 45;
  const int originY = 280;
  const int sizeX = 400;
  const int sizeY = 230;
  const int deviation = 40;
  
  const int minorSizeY = (originY + 10);
  const int minorSizeX = (originX - 10);

  int numberSize = (sizeY / numberOfMarksY);
  int number[numberOfMarksY];
  int val[numberOfMarksY];

void drawGraph(int timeFrame[])
{
  const int numberOfMarksX = timeFrame[0];
  int boxSize = (sizeX / numberOfMarksX);
  int mark[numberOfMarksX];
    
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
    tft.drawLine(originX, (originY - number[i-1]), originX+sizeX, (originY - number[i-1]), LIGHT_GREY);
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

  
  //X-axis

  int pointSeperation = (sizeX / timeFrame[1]);
  
  for(int i = 0; i < 365; i++){
    coolantUse[i] = random(5.2,8.7);
    locationBlock[i] = map(coolantUse[i], 0, graphRange, originY, (originY - sizeY));
  }

    // draw all points
    for(int i = 0; i < timeFrame[1]; i++)
    {
      tft.fillRect(pointSeperation*i+mark[0], locationBlock[i], markSize, markSize, pointColor);
    }

    // draw all the lines
    for(int i = 1; i < timeFrame[1]; i++)
    {
      tft.drawLine(pointSeperation*(i-1)+mark[0], locationBlock[i-1], pointSeperation*(i)+mark[0], locationBlock[i], lineColor);
    }
    
    // change time lables
    for(int i = 0; i < numberOfMarksX; i++)
    {
      tft.setCursor((mark[i] - 5), (originY + 16));
      tft.setTextColor(graphColor, WHITE);
      tft.setTextSize(1);
      tft.println(i+1);
    }
  backButton();
}
