/*
 * This sketch is the second version  
 * of a dice roller for D&D
 * 
 * A user can select the type of dice
 * and the number of dice to roll
 * by using the touchscreen
 * 
 * This sketch is designed for use 
 * with an ESP8266 or ESP32
 * 
 * And the Adafruit 480 * 320
 * tft touchscreen
 * 
 * Created by Samuel Sheedy
 *
 * The libraries included in this file 
 * contain licencing information from Adafruit
 * about the use of their code, which includes the drawBMP()
 * function at the end of this code
 */
// tft and touchscreen libraries
#include <SPI.h>
#include <Adafruit_GFX.h>    
#include <Adafruit_HX8357.h>  
#include <Adafruit_STMPE610.h>
#include <SD.h>

//these are the pins that are used by the touchscreen, tft and sd card
//remember to modify for different boards, the options below are for the Adafruit ESP32
#define STMPE_CS 32
#define TFT_CS   15
#define TFT_DC   33
#define SD_CS    14
#define statusLED 16

#define TFT_RST -1

// This is calibration data for the raw touch data to the screen coordinates
#define TS_MINX 3800
#define TS_MAXX 100
#define TS_MINY 100
#define TS_MAXY 3750

// Size of the color selection boxes and the paintbrush size
#define BOXSIZE 40
#define PENRADIUS 3

#define BUFFPIXEL 20

Adafruit_HX8357 tft = Adafruit_HX8357(TFT_CS, TFT_DC, TFT_RST);
Adafruit_STMPE610 ts = Adafruit_STMPE610(STMPE_CS);

int pageFlag, menuFlag;
int result, numOfDie, typeOfDie;
long mils, prevMils;
int arrayOfResults[100] = {};
int arrayOfNumbers[2] = {};
int numFlag;
bool rolledPrev = false;

void setup(){
  //Define pins
  pinMode(statusLED,OUTPUT);
  
  Serial.begin(115200);

  //start the touchscreen and test if the driver is functioning
  if (!ts.begin()){
    Serial.println("Couldn't start touchscreen controller");
    while (1);
  }
  Serial.println("Touchscreen started");

  Serial.print("Initializing SD card...");
  if (!SD.begin(SD_CS)) {
    Serial.println("failed!");
  }
  Serial.println("OK!");

  //tft.begin(HX8357D);
  tft.begin();
  tft.fillScreen(HX8357_BLACK);

  bmpDraw("/logo.bmp", 0, 0);
  tft.setRotation(1);
  
}

void loop() 
{
  if(pageFlag == 9)
  {
    pageFlag = 0;
    blankScreen();
  }
  else if(ts.touched()){
    pageFlag=1;
  }
  else{
    pageFlag=0;    
  }
  if (pageFlag == 1){
      mainMenu();
  } 
}

void mainMenu(){
  tft.fillScreen(0xA01F);
  tft.fillRect(0, 0, 480, 80, 0x5B0C);
  tft.fillRect(0, 240, 480, 80, 0x5B0C);
  tft.drawLine(240, 80, 240, 260, 0x5B0C);
  tft.drawLine(240, 240, 0, 480, 0x0000);
  tft.drawLine(240, 240, 480, 480, 0x0000);
  tft.fillTriangle(240,240,0,480,240,480,0xFFFF);
  tft.fillTriangle(240,240,240,480,480,480,0x9CD3);
  
  tft.setCursor(40, 30);
  tft.setTextColor(0XFFFF);
  tft.setTextSize(4);
  tft.print("Select Menu Below");

  tft.setCursor(70, 150);
  tft.setTextColor(0XFFFF);
  tft.setTextSize(2);
  tft.print("Roll dice");
  
  tft.setCursor(295, 150);
  tft.setTextColor(0XFFFF);
  tft.setTextSize(2);
  tft.print("Last Result");

  delay(500);
  mils = millis();
  while(!ts.touched())
  {
    delay(100);
    if(millis() >= mils + 30000)
    {
      pageFlag = 9;
      break;
    }
  }

  while(pageFlag == 1)
  {
    if (ts.touched())
    {
      Serial.println("touching");
      TS_Point p; 
      while ( ! ts.bufferEmpty() )
      { 
        p = ts.getPoint();
      }
  
      p.x = map(p.x, TS_MINX, TS_MAXX, tft.height(), 0);
      p.y = map(p.y, TS_MINY, TS_MAXY, tft.width(), 0);
      Serial.print("X = "); Serial.print(p.x); Serial.print("\tY = "); 
      Serial.print(p.y);  Serial.print("\tPressure = "); Serial.println(p.z); 
    
      if(p.y >= 240 && p.y <= 480 && p.x >= 80 && p.x <= 240)
      {
        pageFlag = 6;
      }
      else if(p.y >= 0 && p.y <= 240 && p.x >= 80 && p.x <= 240)
      {
        pageFlag = 7;
      }
      else
      {
        pageFlag = 9;
        break;
      }
    }
    if(pageFlag == 6)
    {
      //roll dice
      diceMenu();
    }
    else if(pageFlag == 7)
    {
      showLastResult();
    }
  }
  
}

void diceMenu(){
  int line = 0x0000;
  tft.fillScreen(0xA01F);
 // tft.fillRect(0, 0, 480, 80, 0x5B0C);
  tft.fillRect(0, 240, 480, 80, 0x5B0C);
  tft.drawLine(0, 120, 480, 120, 0x0000);
  for(int i = 0; i <= 480; i = i + 120)
  {
    tft.drawLine(i, 0, i, 240, 0x0000);
    tft.drawLine(i, 240, 240, 480, 0x0000);
  }
  tft.fillTriangle(240,240,0,480,240,480,0xFFFF);
  tft.fillTriangle(240,240,240,480,480,480,0x9CD3);
  
  tft.setCursor(50, 50);
  tft.setTextColor(0XFFFF);
  tft.setTextSize(2);
  tft.print("D2");

  tft.setCursor(175, 50);
  tft.setTextColor(0XFFFF);
  tft.setTextSize(2);
  tft.print("D4");

  tft.setCursor(285, 50);
  tft.setTextColor(0XFFFF);
  tft.setTextSize(2);
  tft.print("D6");

  tft.setCursor(400, 50);
  tft.setTextColor(0XFFFF);
  tft.setTextSize(2);
  tft.print("D8");
  
  tft.setCursor(40, 175);
  tft.setTextColor(0XFFFF);
  tft.setTextSize(2);
  tft.print("D10");

  tft.setCursor(165, 175);
  tft.setTextColor(0XFFFF);
  tft.setTextSize(2);
  tft.print("D12");

  tft.setCursor(275, 175);
  tft.setTextColor(0XFFFF);
  tft.setTextSize(2);
  tft.print("D20");

  tft.setCursor(390, 175);
  tft.setTextColor(0XFFFF);
  tft.setTextSize(2);
  tft.print("D100");
  
  delay(500);
  mils = millis();
  while(!ts.touched())
  {
    delay(100);
    if(millis() >= mils + 30000)
    {
      pageFlag = 9;
      break;
    }
  }

  while(pageFlag == 6)
  {
    if (ts.touched())
    {
      Serial.println("touching");
      TS_Point p; 
      while ( ! ts.bufferEmpty() )
      { 
        p = ts.getPoint();
      }
  
      p.x = map(p.x, TS_MINX, TS_MAXX, tft.height(), 0);
      p.y = map(p.y, TS_MINY, TS_MAXY, tft.width(), 0);
      Serial.print("X = "); Serial.print(p.x); Serial.print("\tY = "); 
      Serial.print(p.y);  Serial.print("\tPressure = "); Serial.println(p.z); 

      if(p.y >= 360 && p.y <= 480 && p.x >= 200 && p.x <= 320)
      {        
        typeOfDie = 2;
        pageFlag = 2;
      }
      else if(p.y >= 240 && p.y <= 480 && p.x >= 200 && p.x <= 320)
      {       
        typeOfDie = 4;
        pageFlag = 2;  
      }
      else if(p.y >= 120 && p.y <= 480 && p.x >= 200 && p.x <= 320)
      {
        typeOfDie = 6;
        pageFlag = 2;
      }
      else if(p.y >= 0 && p.y <= 480 && p.x >= 200 && p.x <= 320)
      { 
        typeOfDie = 8;
        pageFlag = 2;
      }
      else if(p.y >= 360 && p.y <= 480 && p.x >= 80 && p.x <= 200)
      {   
        typeOfDie = 10;
        pageFlag = 2;
      }
      else if(p.y >= 240 && p.y <= 480 && p.x >= 80 && p.x <= 200)
      { 
        typeOfDie = 12;
        pageFlag = 2;  
      }
      else if(p.y >= 120 && p.y <= 480 && p.x >= 80 && p.x <= 200)
      {       
        typeOfDie = 20;
        pageFlag = 2;
      }
      else if(p.y >= 0 && p.y <= 480 && p.x >= 80 && p.x <= 200)
      {
        typeOfDie = 100;
        pageFlag = 2;
      }
      else
      {
        break;
      }
    }
    if(pageFlag == 2)
    {
      numOfDiceMenu();
    }
  }
}
void showLastResult(){
  mils = millis();
  if(rolledPrev != false){
    tft.fillScreen(0x840F);
    tft.setCursor(160, 40);
    tft.setTextSize(4);
    tft.setTextColor(0XFFFF);
    tft.printf("D%d x %d:",typeOfDie, numOfDie);
    tft.setTextSize(4);
    //need an update to move the cursor depending on how many dice have been rolled and their type
    int cursPos = 0;
    if(numOfDie <= 4)
    {
       cursPos = 200/numOfDie;
    }
    tft.setCursor(cursPos, 120);
    tft.setTextColor(0XFFFF);
    tft.setTextSize(5);
    for(int j = 0; j < numOfDie; j++)
    {
      tft.print(arrayOfResults[j]);
      if(j != numOfDie - 1){
        tft.print("+");
      }
    }
    tft.print("="); //Global variable
    tft.print(result);
  }
  else{
    tft.fillScreen(0x840F);
    tft.setCursor(50, 140);
    tft.setTextColor(0XFFFF);
    tft.setTextSize(3);
    tft.print("No results to display");
  }
  delay(1000);
  pageFlag=0;
  while(!ts.touched())
  {
    delay(10);
    if(millis() >= mils + 20000)
    {
      pageFlag = 9;
      break;
    }
  }
}
void numOfDiceMenu(){
  int line = 0x0000;
  tft.fillScreen(0xA01F);
 // tft.fillRect(0, 0, 480, 80, 0x5B0C);
  tft.fillRect(0, 240, 480, 80, 0x5B0C);
  tft.drawLine(0, 120, 480, 120, 0x0000);
  for(int i = 0; i <= 480; i = i + 96)
  {
    tft.drawLine(i, 0, i, 240, 0x0000);
    //tft.drawLine(i, 240, 240, 480, 0x0000);
  }
  //tft.fillTriangle(240,240,0,480,240,480,0xFFFF);
  //tft.fillTriangle(240,240,240,480,480,480,0x9CD3);
  
  tft.setCursor(40, 50);
  tft.setTextColor(0XFFFF);
  tft.setTextSize(2);
  tft.print("1");

  tft.setCursor(140, 50);
  tft.setTextColor(0XFFFF);
  tft.setTextSize(2);
  tft.print("2");

  tft.setCursor(240, 50);
  tft.setTextColor(0XFFFF);
  tft.setTextSize(2);
  tft.print("3");

  tft.setCursor(335, 50);
  tft.setTextColor(0XFFFF);
  tft.setTextSize(2);
  tft.print("4");
  
  tft.setCursor(430, 50);
  tft.setTextColor(0XFFFF);
  tft.setTextSize(2);
  tft.print("5");

  tft.setCursor(40, 175);
  tft.setTextColor(0XFFFF);
  tft.setTextSize(2);
  tft.print("6");

  tft.setCursor(140, 175);
  tft.setTextColor(0XFFFF);
  tft.setTextSize(2);
  tft.print("7");

  tft.setCursor(240, 175);
  tft.setTextColor(0XFFFF);
  tft.setTextSize(2);
  tft.print("8");

  tft.setCursor(335, 175);
  tft.setTextColor(0XFFFF);
  tft.setTextSize(2);
  tft.print("9");

  tft.setCursor(430, 175);
  tft.setTextColor(0XFFFF);
  tft.setTextSize(2);
  tft.print("0");

  tft.setCursor(20, 260);
  tft.setTextColor(0XFFFF);
  tft.setTextSize(2);
  tft.print("Press here to roll: ");

  tft.setCursor(400, 260);
  tft.setTextColor(0XFFFF);
  tft.setTextSize(2);
  tft.printf("x D%d",typeOfDie);

  numFlag = 0;
  delay(750);
  mils = millis();
  while(!ts.touched())
  {
    delay(100);
    if(millis() >= mils + 30000)
    {
      pageFlag = 9;
      break;
    }
  }

  while(pageFlag == 2)
  {
    delay(200);
    if (ts.touched())
    {
      Serial.println("touching");
      TS_Point p; 
      while ( ! ts.bufferEmpty() )
      { 
        p = ts.getPoint();
      }
  
      p.x = map(p.x, TS_MINX, TS_MAXX, tft.height(), 0);
      p.y = map(p.y, TS_MINY, TS_MAXY, tft.width(), 0);
      Serial.print("X = "); Serial.print(p.x); Serial.print("\tY = "); 
      Serial.print(p.y);  Serial.print("\tPressure = "); Serial.println(p.z); 
      
      if(p.y >= 384 && p.y <= 480 && p.x >= 200 && p.x <= 320)
      {
        if(numFlag < 2){
          arrayOfNumbers[numFlag] = 1;
          tft.setCursor(300+numFlag*10, 260);
          tft.setTextColor(0XFFFF);
          tft.setTextSize(2);
          tft.print(arrayOfNumbers[numFlag]);
          numFlag++;
        }

      }
      else if(p.y >= 288 && p.y <= 480 && p.x >= 200 && p.x <= 320)
      {
        if(numFlag < 2){
          arrayOfNumbers[numFlag] = 2;
          tft.setCursor(300+numFlag*10, 260);
          tft.setTextColor(0XFFFF);
          tft.setTextSize(2);
          tft.print(arrayOfNumbers[numFlag]);
          numFlag++;
        }
      }
      else if(p.y >= 192 && p.y <= 480 && p.x >= 200 && p.x <= 320)
      {
        if(numFlag < 2){
          arrayOfNumbers[numFlag] = 3;
          tft.setCursor(300+numFlag*10, 260);
          tft.setTextColor(0XFFFF);
          tft.setTextSize(2);
          tft.print(arrayOfNumbers[numFlag]);
          numFlag++;          
        }
      }
      else if(p.y >= 96 && p.y <= 480 && p.x >= 200 && p.x <= 320)
      {
        if(numFlag < 2){
          arrayOfNumbers[numFlag] = 4;
          tft.setCursor(300+numFlag*10, 260);
          tft.setTextColor(0XFFFF);
          tft.setTextSize(2);
          tft.print(arrayOfNumbers[numFlag]);
          numFlag++;
        }
      }
      else if(p.y >= 0 && p.y <= 480 && p.x >= 200 && p.x <= 320)
      {
        if(numFlag < 2){
          arrayOfNumbers[numFlag] = 5;
          tft.setCursor(300+numFlag*10, 260);
          tft.setTextColor(0XFFFF);
          tft.setTextSize(2);
          tft.print(arrayOfNumbers[numFlag]);
          numFlag++;
        }
      }
      else if(p.y >= 384 && p.y <= 480 && p.x >= 80 && p.x <= 200)
      {
        if(numFlag < 2){
          arrayOfNumbers[numFlag] = 6;
          tft.setCursor(300+numFlag*10, 260);
          tft.setTextColor(0XFFFF);
          tft.setTextSize(2);
          tft.print(arrayOfNumbers[numFlag]);
          numFlag++;
        }
      }
      else if(p.y >= 288 && p.y <= 480 && p.x >= 80 && p.x <= 200)
      {
        if(numFlag < 2){
          arrayOfNumbers[numFlag] = 7;
          tft.setCursor(300+numFlag*10, 260);
          tft.setTextColor(0XFFFF);
          tft.setTextSize(2);
          tft.print(arrayOfNumbers[numFlag]);
          numFlag++;
        }
      }
      else if(p.y >= 192 && p.y <= 480 && p.x >= 80 && p.x <= 200)
      {
        if(numFlag < 2){
          arrayOfNumbers[numFlag] = 8;
          tft.setCursor(300+numFlag*10, 260);
          tft.setTextColor(0XFFFF);
          tft.setTextSize(2);
          tft.print(arrayOfNumbers[numFlag]);
          numFlag++;
        }
      }
      else if(p.y >= 96 && p.y <= 480 && p.x >= 80 && p.x <= 200)
      {
        if(numFlag < 2){
          arrayOfNumbers[numFlag] = 9;
          
          tft.setCursor(300+numFlag*10, 260);
          tft.setTextColor(0XFFFF);
          tft.setTextSize(2);
          tft.print(arrayOfNumbers[numFlag]);
          numFlag++;
        }
      }
      else if(p.y >= 0 && p.y <= 480 && p.x >= 80 && p.x <= 200)
      {
        if(numFlag > 0 && numFlag <2){
          arrayOfNumbers[numFlag] = 0;
          tft.setCursor(300+numFlag*10, 260);
          tft.setTextColor(0XFFFF);
          tft.setTextSize(2);
          tft.print(arrayOfNumbers[numFlag]);
          numFlag++;
        }
      }
      else
      {
        if(numFlag == 0)
        {
          numOfDie = 1;
        }
        else if(numFlag == 1){
          numOfDie = arrayOfNumbers[0];
          arrayOfNumbers[0] = 0;
          arrayOfNumbers[1] = 0;

        }
        else{
          numOfDie = (arrayOfNumbers[0]*10) + arrayOfNumbers[1];
          arrayOfNumbers[0] = 0;
          arrayOfNumbers[1] = 0;
          if(numOfDie > 20)
          {
            numOfDie = 20;
          }
        }
        pageFlag = 3;
      }
    }
    if(pageFlag == 3)
    {
      resultScreen();
    }
  }
}

void resultScreen(){
  rolledPrev = true;
  result = 0;
  for(int i = 0; i < numOfDie; i++)
  {
    randomSeed(analogRead(0));
    arrayOfResults[i] = random(1,typeOfDie+1);
    result += arrayOfResults[i];
  }
  mils = millis();
  tft.fillScreen(0x840F);
  tft.setCursor(160, 40);
  tft.setTextSize(4);
  tft.setTextColor(0XFFFF);
  tft.printf("D%d x %d:",typeOfDie, numOfDie);
  tft.setTextSize(4);
  //need an update to move the cursor depending on how many dice have been rolled
  int cursPos = 0;
  if(numOfDie <= 4)
  {
     cursPos = 200/numOfDie;
  }
  tft.setCursor(cursPos, 120);
  tft.setTextColor(0XFFFF);
  tft.setTextSize(5);
  for(int j = 0; j < numOfDie; j++)
  {
    tft.print(arrayOfResults[j]);
    if(j != numOfDie - 1){
      tft.print("+");
    }
  }
  tft.print("="); //Global variable
  tft.print(result);
  delay(1000);
  pageFlag=0;
  while(!ts.touched())
  {
    delay(10);
    if(millis() >= mils + 20000)
    {
      pageFlag = 9;
      break;
    }
  }
}

void blankScreen()
{
   tft.fillScreen(0X00000);
}

void bmpDraw(char *filename, uint8_t x, uint16_t y) {

  File     bmpFile;
  int      bmpWidth, bmpHeight;   // W+H in pixels
  uint8_t  bmpDepth;              // Bit depth (currently must be 24)
  uint32_t bmpImageoffset;        // Start of image data in file
  uint32_t rowSize;               // Not always = bmpWidth; may have padding
  uint8_t  sdbuffer[3*BUFFPIXEL]; // pixel buffer (R+G+B per pixel)
  uint8_t  buffidx = sizeof(sdbuffer); // Current position in sdbuffer
  boolean  goodBmp = false;       // Set to true on valid header parse
  boolean  flip    = true;        // BMP is stored bottom-to-top
  int      w, h, row, col;
  uint8_t  r, g, b;
  uint32_t pos = 0, startTime = millis();

  if((x >= tft.width()) || (y >= tft.height())) return;

  Serial.println();
  Serial.print(F("Loading image '"));
  Serial.print(filename);
  Serial.println('\'');

  // Open requested file on SD card
  if ((bmpFile = SD.open(filename)) == NULL) {
    Serial.print(F("File not found"));
    return;
  }

  // Parse BMP header
  if(read16(bmpFile) == 0x4D42) { // BMP signature
    Serial.print(F("File size: ")); Serial.println(read32(bmpFile));
    (void)read32(bmpFile); // Read & ignore creator bytes
    bmpImageoffset = read32(bmpFile); // Start of image data
    Serial.print(F("Image Offset: ")); Serial.println(bmpImageoffset, DEC);
    // Read DIB header
    Serial.print(F("Header size: ")); Serial.println(read32(bmpFile));
    bmpWidth  = read32(bmpFile);
    bmpHeight = read32(bmpFile);
    if(read16(bmpFile) == 1) { // # planes -- must be '1'
      bmpDepth = read16(bmpFile); // bits per pixel
      Serial.print(F("Bit Depth: ")); Serial.println(bmpDepth);
      if((bmpDepth == 24) && (read32(bmpFile) == 0)) { // 0 = uncompressed

        goodBmp = true; // Supported BMP format -- proceed!
        Serial.print(F("Image size: "));
        Serial.print(bmpWidth);
        Serial.print('x');
        Serial.println(bmpHeight);

        // BMP rows are padded (if needed) to 4-byte boundary
        rowSize = (bmpWidth * 3 + 3) & ~3;

        // If bmpHeight is negative, image is in top-down order.
        // This is not canon but has been observed in the wild.
        if(bmpHeight < 0) {
          bmpHeight = -bmpHeight;
          flip      = false;
        }

        // Crop area to be loaded
        w = bmpWidth;
        h = bmpHeight;
        if((x+w-1) >= tft.width())  w = tft.width()  - x;
        if((y+h-1) >= tft.height()) h = tft.height() - y;

        // Set TFT address window to clipped image bounds
        tft.setAddrWindow(x, y, x+w-1, y+h-1);

        for (row=0; row<h; row++) { // For each scanline...

          // Seek to start of scan line.  It might seem labor-
          // intensive to be doing this on every line, but this
          // method covers a lot of gritty details like cropping
          // and scanline padding.  Also, the seek only takes
          // place if the file position actually needs to change
          // (avoids a lot of cluster math in SD library).
          if(flip) // Bitmap is stored bottom-to-top order (normal BMP)
            pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
          else     // Bitmap is stored top-to-bottom
            pos = bmpImageoffset + row * rowSize;
          if(bmpFile.position() != pos) { // Need seek?
            bmpFile.seek(pos);
            buffidx = sizeof(sdbuffer); // Force buffer reload
          }

          for (col=0; col<w; col++) { // For each pixel...
            // Time to read more pixel data?
            if (buffidx >= sizeof(sdbuffer)) { // Indeed
              bmpFile.read(sdbuffer, sizeof(sdbuffer));
              buffidx = 0; // Set index to beginning
            }

            // Convert pixel from BMP to TFT format, push to display
            b = sdbuffer[buffidx++];
            g = sdbuffer[buffidx++];
            r = sdbuffer[buffidx++];
            tft.pushColor(tft.color565(r,g,b));
          } // end pixel
        } // end scanline
        Serial.print(F("Loaded in "));
        Serial.print(millis() - startTime);
        Serial.println(" ms");
      } // end goodBmp
    }
  }

  bmpFile.close();
  if(!goodBmp) Serial.println(F("BMP format not recognized."));
}

// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.

uint16_t read16(File &f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

uint32_t read32(File &f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}
