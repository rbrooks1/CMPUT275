/*
Name: Ryan Brooks
ID: 1530605
Course: CMPUT 275 winter 2019
assignment: Weekly exercise 2: restaurants and pointers
*/

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <SD.h>
#include <TouchScreen.h>

#define TFT_DC 9
#define TFT_CS 10
#define SD_CS 6

// physical dimensions of the tft display (# of pixels)
#define DISPLAY_WIDTH  320
#define DISPLAY_HEIGHT 240

// touch screen pins, obtained from the documentaion
#define YP A2  // must be an analog pin, use "An" notation!
#define XM A3  // must be an analog pin, use "An" notation!
#define YM  5  // can be a digital pin
#define XP  4  // can be a digital pin

// dimensions of the part allocated to the map display
#define MAP_DISP_WIDTH (DISPLAY_WIDTH - 48)
#define MAP_DISP_HEIGHT DISPLAY_HEIGHT

#define REST_START_BLOCK 4000000
#define NUM_RESTAURANTS 1066

// calibration data for the touch screen, obtained from documentation
// the minimum/maximum possible readings from the touch point
#define TS_MINX 150
#define TS_MINY 120
#define TS_MAXX 920
#define TS_MAXY 940

// thresholds to determine if there was a touch
#define MINPRESSURE   10
#define MAXPRESSURE 1000

#define TFT_WIDTH  320
#define TFT_HEIGHT 240

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

// a multimeter reading says there are 300 ohms of resistance across the plate,
// so initialize with this to get more accurate readings
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

// different than SD
Sd2Card card;
struct restaurant {
  int32_t lat;
  int32_t lon;
  uint8_t rating;  // from 0 to 10
  char name[55];
} restNames;

// global variable setup
restaurant restBlock[8];
uint32_t fastBlockNum = 0;
int callTimes = 0;
int callTimesFast = 0;
int totalRunTime = 0;
int totalRunTimeFast = 0;


// setup function
void setup() {
  init();
  Serial.begin(9600);

  // tft display initialization
  tft.begin();
  tft.fillScreen(ILI9341_BLACK);
  tft.setRotation(3);
  tft.setTextSize(3);

  // initial setup for the display
  tft.println("RECENT SLOW RUN");
  tft.println("not yet run");
  tft.println("SLOW RUN AVG");
  tft.println("not yet run");
  tft.println("RECENT FAST RUN");
  tft.println("not yet run");
  tft.println("FAST RUN AVG");
  tft.println("not yet run");
  tft.drawRect(272, 0, 48, 120, ILI9341_WHITE);
  tft.drawRect(272, 120, 48, 120, ILI9341_WHITE);
  tft.drawChar(291, 28, 'F', ILI9341_WHITE, 1, 2);
  tft.drawChar(291, 44, 'A', ILI9341_WHITE, 1, 2);
  tft.drawChar(291, 60, 'S', ILI9341_WHITE, 1, 2);
  tft.drawChar(291, 76, 'T', ILI9341_WHITE, 1, 2);
  tft.drawChar(291, 148, 'S', ILI9341_WHITE, 1, 2);
  tft.drawChar(291, 164, 'L', ILI9341_WHITE, 1, 2);
  tft.drawChar(291, 180, 'O', ILI9341_WHITE, 1, 2);
  tft.drawChar(291, 196, 'W', ILI9341_WHITE, 1, 2);
  // SD card initialization for raw reads
  Serial.print("Initializing SPI communication for raw reads...");
  if (!card.init(SPI_HALF_SPEED, SD_CS)) {
    Serial.println("failed! Is the card inserted properly?");
    while (true) {}
  } else {
    Serial.println("OK!");
  }
}

// function that reads all the restaurants on the SD card
void getRestaurant(int restIndex, restaurant* restPtr) {
  uint32_t blockNum = REST_START_BLOCK + restIndex/8;

  while (!card.readBlock(blockNum, (uint8_t*) restBlock)) {
    Serial.println("Read block failed, trying again.");
  }

  *restPtr = restBlock[restIndex % 8];
}


// function that reads through all the restaurants on the
// SD card in a more efficient way then getRestaurant
void getRestaurantFast(int restIndex, restaurant* restPtr) {
  uint32_t newBlockNum = REST_START_BLOCK + restIndex/8;
  if (newBlockNum == fastBlockNum) {
    *restPtr = restBlock[restIndex % 8];
  } else if (newBlockNum != fastBlockNum) {
      while (!card.readBlock(newBlockNum, (uint8_t*) restBlock)) {
        Serial.println("Read block failed, trying again.");
      }
      *restPtr = restBlock[restIndex % 8];
      fastBlockNum = newBlockNum;
    }
}

// Function that processes the touch on the touch
// screen and runs either getRestaurantFast or
// getRestaurant based on where the touch occured
void touchProcess() {
    TSPoint touch = ts.getPoint();
      if (touch.z < MINPRESSURE || touch.z > MAXPRESSURE) {
        return;
      }
      int16_t screen_x = map(touch.y, TS_MINY, TS_MAXY, TFT_WIDTH-1, 0);
      int16_t screen_y = map(touch.x, TS_MINX, TS_MAXX, 0, TFT_HEIGHT-1);
      // run for the efficient read of the restaurants
      if ((screen_x > 270) && (screen_x < 320) && (screen_y > 0) && (screen_y < 120)) {
        callTimesFast += 1;
        uint32_t initTime = millis();
        for (int i = 0; i < 1066; ++i) {
            getRestaurantFast(i, &restNames);
        }
        // final variable and arithmetic work for timing to be
        // displayed on the lcd display
        uint32_t endTime = millis();
        uint32_t finalTime = endTime - initTime;
        totalRunTimeFast += finalTime;
        int callTimesFastAVG = totalRunTimeFast/callTimesFast;
        tft.setCursor(0, 120);
        tft.fillRect(0, 120, 270, 24, ILI9341_BLACK);
        tft.print(finalTime);
        tft.println(" ms");
        tft.setCursor(0, 169);
        tft.fillRect(0, 169, 270, 24, ILI9341_BLACK);
        tft.print(callTimesFastAVG);
        tft.println(" ms");
      }
      // run for the slow read of the restaurants on the SD card
      if ((screen_x > 270) && (screen_x < 320) && (screen_y > 120) && (screen_y < 240)) {
        callTimes += 1;
        uint32_t initTime = millis();
        for (int i = 0; i < 1066; ++i) {
            getRestaurant(i, &restNames);
        }
        // final variable and arithmetic for timing to be printed
        // to the lcd display
        uint32_t endTime = millis();
        uint32_t finalTime = endTime - initTime;
        totalRunTime += finalTime;
        int callTimesAVG = totalRunTime/callTimes;
        tft.setCursor(0, 25);
        tft.fillRect(0, 25, 270, 24, ILI9341_BLACK);
        tft.print(finalTime);
        tft.println(" ms");
        tft.setCursor(0, 72);
        tft.fillRect(0, 72, 270, 24, ILI9341_BLACK);
        tft.print(callTimesAVG);
        tft.println(" ms");
      }
}


// main function
int main() {
  setup();
  delay(300);
  while (true) {
      touchProcess();
  }
  Serial.end();

  return 0;
}
