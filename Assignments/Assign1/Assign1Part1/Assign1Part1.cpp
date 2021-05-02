/*
Name: Ryan Brooks, Charlie Chen
ID: 1530605, 1530488
Course: CKPUT 275 winter 2019
assignment: Assignment 1 part 1: Simple version of restaurant finder
*/

#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_ILI9341.h>
#include <Adafruit_GFX.h>
#include "lcd_image.h"
#include <TouchScreen.h>
#include <math.h>

#define TFT_DC 9
#define TFT_CS 10
#define SD_CS 6

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

// physical dimensions of the tft display (# of pixels)
#define DISPLAY_WIDTH  320
#define DISPLAY_HEIGHT 240

// dimensions of the part allocated to the map display
#define MAP_DISP_WIDTH (DISPLAY_WIDTH - 48)
#define MAP_DISP_HEIGHT DISPLAY_HEIGHT

// width and height (in pixels) of the LCD image
#define LCD_WIDTH 2048
#define LCD_HEIGHT 2048
lcd_image_t yegImage = { "yeg-big.lcd", LCD_WIDTH, LCD_HEIGHT };

#define JOY_VERT  A1  // should connect A1 to pin VRx
#define JOY_HORIZ A0  // should connect A0 to pin VRy
#define JOY_SEL   2

#define JOY_CENTER   512
#define JOY_DEADZONE 64

#define CURSOR_SIZE 9

// smaller numbers yield faster cursor movement
#define JOY_SPEED 64

int flag = 0;
// the cursor position on the display, stored as the middle pixel of the cursor
int cursorX, cursorY;

// upper-left coordinates in the image of the middle of the map of Edmonton
#define YEG_MIDDLE_X (LCD_WIDTH/2 - MAP_DISP_WIDTH/2)
#define YEG_MIDDLE_Y (LCD_HEIGHT/2 - MAP_DISP_HEIGHT/2)

int yegCurrentX = YEG_MIDDLE_X;
int yegCurrentY = YEG_MIDDLE_Y;

#define REST_START_BLOCK 4000000
#define NUM_RESTAURANTS 1066

// calibration data for the touch screen, obtained from documentation
// the minimum/maximum possible readings from the touch point
#define TS_MINX 150
#define TS_MINY 120
#define TS_MAXX 920
#define TS_MAXY 940

#define YP A2  // must be an analog pin, use "An" notation!
#define XM A3  // must be an analog pin, use "An" notation!
#define YM  5  // can be a digital pin
#define XP  4  // can be a digital pin

// thresholds to determine if there was a touch
#define MINPRESSURE   10
#define MAXPRESSURE 1000

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

//  These  constants  are  for  the  2048 by 2048  map.
#define  LAT_NORTH  5361858l
#define  LAT_SOUTH  5340953l
#define  LON_WEST  -11368652l
#define  LON_EAST  -11333496l

// different than SD
Sd2Card card;
struct restaurant {
  int32_t lat;
  int32_t lon;
  uint8_t rating;  // from 0 to 10
  char name[55];
} rest;

struct  RestDist {
  uint16_t  index;  // index  of  restaurant  from 0 to  NUM_RESTAURANTS -1
  uint16_t  dist;  //  Manhatten  distance  to  cursor  position
};

RestDist rest_dist[NUM_RESTAURANTS];
restaurant restBlock[8];
uint32_t fastBlockNum = 0;

// forward declaration for drawing the cursor
void redrawCursor(int newX, int newY, int oldX, int oldY);

void setup() {
  init();

  Serial.begin(9600);

  // not actually used in this exercise, but it's ok to leave it
	pinMode(JOY_SEL, INPUT_PULLUP);

	tft.begin();

  Serial.print("Initializing SPI communication for raw reads...");
    if (!card.init(SPI_HALF_SPEED, SD_CS)) {
      Serial.println("failed! Is the card inserted properly?");
      while (true) {}
    } else {
      Serial.println("OK!");
    }

	Serial.print("Initializing SD card...");
	if (!SD.begin(SD_CS)) {
		Serial.println("failed! Is it inserted properly?");
		while (true) {}
	}
	Serial.println("OK!");

	tft.setRotation(3);

  tft.fillScreen(ILI9341_BLACK);

  // draws the centre of the Edmonton map, leaving the rightmost 48 columns black
	lcd_image_draw(&yegImage, &tft, yegCurrentX, yegCurrentY,
                 0, 0, MAP_DISP_WIDTH, MAP_DISP_HEIGHT);

  // initial cursor position is the middle of the screen
  cursorX = (DISPLAY_WIDTH - 48 - CURSOR_SIZE)/2;
  cursorY = (DISPLAY_HEIGHT - CURSOR_SIZE)/2;

  // draw the initial cursor
  redrawCursor(cursorX, cursorY, cursorX, cursorY);
}

// redraws the patch of edmonton over the older cursor position
// given by "oldX, oldY" and draws the cursor at its new position
// given by "newX, newY"
void redrawCursor(int newX, int newY, int oldX, int oldY) {
  // draw the patch of edmonton over the old cursor
  lcd_image_draw(&yegImage, &tft,
                 yegCurrentX + oldX, yegCurrentY + oldY,
                 oldX, oldY, CURSOR_SIZE, CURSOR_SIZE);

  // and now draw the cursor at the new position
  tft.fillRect(newX, newY, CURSOR_SIZE, CURSOR_SIZE, ILI9341_RED);
}


/*
Redraws a new section of the map 240 pixels up or down, based on
which boundary the cursor reaches.
*/
void moveY() {
  // if the cursor reaches the top boundary.
  if (cursorY <= 0) {
    if ((yegCurrentY - MAP_DISP_HEIGHT) > 0) {
      yegCurrentY = yegCurrentY - MAP_DISP_HEIGHT;
      lcd_image_draw(&yegImage, &tft, yegCurrentX, yegCurrentY,
        0, 0, MAP_DISP_WIDTH, MAP_DISP_HEIGHT);
      cursorX = MAP_DISP_WIDTH/2;
      cursorY = MAP_DISP_HEIGHT/2;
      redrawCursor(cursorX, cursorY, cursorX, cursorY);
    } else if ((yegCurrentY - MAP_DISP_HEIGHT) < 0) {
      // ensures screen stays in the boundaries of the map.
        if (yegCurrentY != 0) {
          yegCurrentY = 0;
          lcd_image_draw(&yegImage, &tft, yegCurrentX, yegCurrentY,
            0, 0, MAP_DISP_WIDTH, MAP_DISP_HEIGHT);
          cursorX = MAP_DISP_WIDTH/2;
          cursorY = MAP_DISP_HEIGHT/2;
          redrawCursor(cursorX, cursorY, cursorX, cursorY);
        }
      }
  }
  // if the cursor reaches the bottom boundary.
  if (cursorY >= (MAP_DISP_HEIGHT - CURSOR_SIZE)) {
    if ((yegCurrentY + MAP_DISP_HEIGHT) < (LCD_HEIGHT - MAP_DISP_HEIGHT)) {
      yegCurrentY = yegCurrentY + MAP_DISP_HEIGHT;
      lcd_image_draw(&yegImage, &tft, yegCurrentX, yegCurrentY,
        0, 0, MAP_DISP_WIDTH, MAP_DISP_HEIGHT);
      cursorX = MAP_DISP_WIDTH/2;
      cursorY = MAP_DISP_HEIGHT/2;
      redrawCursor(cursorX, cursorY, cursorX, cursorY);
    } else if ((yegCurrentY + MAP_DISP_HEIGHT) > (LCD_HEIGHT - MAP_DISP_HEIGHT)) {
      // ensures screen stays in the boundaries of the map.
        if (yegCurrentY != LCD_HEIGHT - MAP_DISP_HEIGHT) {
          yegCurrentY = LCD_HEIGHT - MAP_DISP_HEIGHT;
          lcd_image_draw(&yegImage, &tft, yegCurrentX, yegCurrentY,
            0, 0, MAP_DISP_WIDTH, MAP_DISP_HEIGHT);
          cursorX = MAP_DISP_WIDTH/2;
          cursorY = MAP_DISP_HEIGHT/2;
          redrawCursor(cursorX, cursorY, cursorX, cursorY);
        }
      }
  }
}


/*
Redraws a new section of the map 272 pixels left or right, based on
which boundary the cursor reaches.
*/
void moveX() {
  // if the cursor reaches the leftmost boundary.
  if (cursorX <= 0) {
    if ((yegCurrentX - MAP_DISP_WIDTH) > 0) {
      // sets the global currentX value
      yegCurrentX = yegCurrentX - MAP_DISP_WIDTH;
      lcd_image_draw(&yegImage, &tft, yegCurrentX, yegCurrentY,
        0, 0, MAP_DISP_WIDTH, MAP_DISP_HEIGHT);
      cursorX = MAP_DISP_WIDTH/2;
      cursorY = MAP_DISP_HEIGHT/2;
      redrawCursor(cursorX, cursorY, cursorX, cursorY);
    } else if ((yegCurrentX - MAP_DISP_WIDTH) < 0) {
      // ensures screen stays in the boundaries of the map.
        if (yegCurrentX != 0) {
          yegCurrentX = 0;
          lcd_image_draw(&yegImage, &tft, yegCurrentX, yegCurrentY,
            0, 0, MAP_DISP_WIDTH, MAP_DISP_HEIGHT);
          yegCurrentX = 0;
          cursorX = MAP_DISP_WIDTH/2;
          cursorY = MAP_DISP_HEIGHT/2;
          redrawCursor(cursorX, cursorY, cursorX, cursorY);
        }
      }
  }
  // if the cursor reaches the rightmost boundary.
  if (cursorX >= MAP_DISP_WIDTH - CURSOR_SIZE) {
    if ((yegCurrentX + MAP_DISP_WIDTH) < LCD_WIDTH - (MAP_DISP_WIDTH)) {
      yegCurrentX = yegCurrentX + MAP_DISP_WIDTH;
      lcd_image_draw(&yegImage, &tft, yegCurrentX, yegCurrentY,
        0, 0, MAP_DISP_WIDTH, MAP_DISP_HEIGHT);
      cursorX = MAP_DISP_WIDTH/2;
      cursorY = MAP_DISP_HEIGHT/2;
      redrawCursor(cursorX, cursorY, cursorX, cursorY);
    } else if ((yegCurrentX + MAP_DISP_WIDTH) > LCD_WIDTH - MAP_DISP_HEIGHT) {
      // ensures screen stays in the boundaries of the map.
        if (yegCurrentX != LCD_WIDTH - MAP_DISP_WIDTH) {
          yegCurrentX = LCD_WIDTH - MAP_DISP_WIDTH;
          lcd_image_draw(&yegImage, &tft, yegCurrentX, yegCurrentY,
            0, 0, MAP_DISP_WIDTH, MAP_DISP_HEIGHT);
          cursorX = MAP_DISP_WIDTH/2;
          cursorY = MAP_DISP_HEIGHT/2;
          redrawCursor(cursorX, cursorY, cursorX, cursorY);
        }
      }
  }
}

//  These  functions  convert  between x/y map  position  and  lat/lon
// (and  vice  versa.)
int32_t  x_to_lon(int16_t x) {
  return  map(x, 0, LCD_WIDTH , LON_WEST , LON_EAST);
}
int32_t  y_to_lat(int16_t y) {
  return  map(y, 0, LCD_HEIGHT , LAT_NORTH , LAT_SOUTH);
}
int16_t  lon_to_x(int32_t  lon) {
  return  map(lon , LON_WEST , LON_EAST , 0, LCD_WIDTH);
}
int16_t  lat_to_y(int32_t  lat) {
  return  map(lat , LAT_NORTH , LAT_SOUTH , 0, LCD_HEIGHT);
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

// function that reads all the restaurants on the SD card
void getRestaurant(int restIndex, restaurant* restPtr) {
  uint32_t blockNum = REST_START_BLOCK + restIndex/8;

  while (!card.readBlock(blockNum, (uint8_t*) restBlock)) {
    Serial.println("Read block failed, trying again.");
  }

  *restPtr = restBlock[restIndex % 8];
}


/*
Runs the processes for the touchscreen. If the screen is pressed the location
of all the restaurants viewable by the current screen becomes marked by blue squares.
*/
void processTScreen() {
  TSPoint touch = ts.getPoint();
  // this was used in the get_restaurant.cpp template but gives a style error.
  // code functionality is unaffected.
  if (touch.z < MINPRESSURE || touch.z > MAXPRESSURE) {}
  int screen_x = map(touch.y, TS_MINY, TS_MAXY, DISPLAY_WIDTH-1, 0);
  int screen_y = map(touch.x, TS_MINX, TS_MAXX, 0, DISPLAY_HEIGHT-1);
  if ((screen_x > 0) && (screen_x < MAP_DISP_WIDTH) && (screen_y > 0) && (screen_y < MAP_DISP_HEIGHT)) {
    for (int i = 0; i < NUM_RESTAURANTS; i++) {
      // retrieves the latitude and longitude of each restaurant and converts them to x
      // and y coordinates on the map.
      getRestaurantFast(i, &rest);
      int restx = lon_to_x(rest.lon);
      int resty = lat_to_y(rest.lat);
        // prints blue squares at the locations of restaurants viewable by the current
        // screen coordinates.
        if ((restx > yegCurrentX) && (restx < yegCurrentX + MAP_DISP_WIDTH) &&
          (resty > yegCurrentY) && (resty < yegCurrentY + MAP_DISP_HEIGHT)) {
          tft.fillRect(restx - yegCurrentX, resty - yegCurrentY, 5, 5, ILI9341_BLUE);
        }
    }
  }
}


/*
Moves around the displayed map and redraws a different section of the map when
the cursor reaches the boundary of the screen. Also displays blue squares at
the locations of all restaurants present on the screen
*/
void processJoystick() {
  int xVal = analogRead(JOY_HORIZ);
  int yVal = analogRead(JOY_VERT);
  int buttonVal = digitalRead(JOY_SEL);

  // copy the cursor position (to check later if it changed)
  int oldX = cursorX;
  int oldY = cursorY;

  // move the cursor, further pushes mean faster movement
  cursorX += (JOY_CENTER - xVal) / JOY_SPEED;
  cursorY += (yVal - JOY_CENTER) / JOY_SPEED;

  // constrain so the cursor does not go off of the map display window
  cursorX = constrain(cursorX, 0, MAP_DISP_WIDTH - CURSOR_SIZE);
  cursorY = constrain(cursorY, 0, MAP_DISP_HEIGHT - CURSOR_SIZE);
  // moveX redraws the map when the cursor reaches the horizontal boundaries.
  // moveY redraws the map when the cursor reaches the vertical boundaries.
  moveY();
  moveX();
  // displays blue squares at the locations of all restaurants within the
  // screen coordinates.
  processTScreen();

  // redraw the cursor only if its position actually changed
  if (cursorX != oldX || cursorY != oldY) {
    redrawCursor(cursorX, cursorY, oldX, oldY);
  }

  delay(10);
}


/*
Runs the mode 0 processes such as cursor movement on the map and
displaying the restaurant locations
*/
void mode0() {
    // cursor movement on map and redrawing map.
    processJoystick();
    int joySelect = digitalRead(JOY_SEL);
    // switches to mode 1 when the joystick is pressed down.
    if (joySelect == LOW) {
        flag = 1;
        while (joySelect == LOW) {
            // Saves the current state of the button for the next time
            // through the loop
            joySelect = digitalRead(JOY_SEL);
        }
    }
}


/*
Sorts the RestDist structs based on their manhattan distances.
Based on pseudocode in assignment description.
Inputs:
  a[] (RestDist): an array of RestDist structs.
*/
void insertionSort(RestDist a[]) {
  int k = 1;
  while (k < NUM_RESTAURANTS) {
    int l = k;
    while (l > 0 && a[l - 1].dist > a[l].dist) {
        RestDist temp = a[l - 1];
        a[l - 1] = a[l];
        a[l] = temp;
      l--;
    }
    k++;
  }
}


/*
Displays the closest 30 restaurants to the cursor's last position
before the mode was switched. The joystick is used to move up and down
the list of restaurants. Pressing the joystick down switches the mode
to mode 0 and draws the map with the coordinates of the selected restaurant
in the center, at the cursor's position.
*/
void processRest() {
  // retrieves the indicies, longitudes and latitudes of all restaurants on
  // the SD card and calculates the manhattan distances from each restaurant
  // to the cursor's position. The indicies and manhattan distances are stored
  // in RestDist structs and stored in a new array.
  for (int i = 0; i < NUM_RESTAURANTS; i++) {
    getRestaurantFast(i, &rest);
    rest_dist[i].index = i;
    int cursorDispX = yegCurrentX + cursorX;
    int cursorDispY = yegCurrentY + cursorY;
    int restx = lon_to_x(rest.lon);
    int resty = lat_to_y(rest.lat);
    rest_dist[i].dist = abs(restx - cursorDispX) + abs(resty - cursorDispY);
  }
  // the array of RestDists is sorted based on manhattan distances.
  insertionSort(rest_dist);
  tft.fillScreen(0);
  tft.setCursor(0, 0);  // where  the  characters  will be  displayed
  tft.setTextWrap(false);

  int selectedRest = 0;  // which  restaurant  is  selected?
    for (int i = 0; i < 30; i++) {
      getRestaurant(rest_dist[i].index , &rest);
      if (i !=  selectedRest) {  // not  highlighted
        // white  characters  on  black  background
        tft.setTextColor(0xFFFF , 0x0000);
      } else {   // highlighted
        // black  characters  on  white  background
        tft.setTextColor(0x0000 , 0xFFFF);
      }
      tft.print(rest.name);
      tft.print("\n");
      }
    tft.print("\n");
    int c = 0;
    // scrolls up and down the list based on input from the joystick.
    while (true) {
      int yVal = analogRead(JOY_VERT);
      getRestaurant(rest_dist[c].index , &rest);
      // moves the highlighted block up if the joystick is moved up.
      if (yVal > JOY_CENTER + JOY_DEADZONE) {
        tft.setCursor(0, 8 * c);
        // white  characters  on  black  background
        tft.setTextColor(0xFFFF , 0x0000);
        tft.print(rest.name);
        c++;
        if (c > 29) {
          c = 0;
        }
        tft.setCursor(0, 8 * c);
        getRestaurant(rest_dist[c].index , &rest);
        // black  characters  on  white  background
        tft.setTextColor(0x0000 , 0xFFFF);
        tft.print(rest.name);
        delay(500);

      // moves the highlighted block down if the joystick is moved down.
      } else if (yVal < JOY_CENTER - JOY_DEADZONE) {
          tft.setCursor(0, 8 * c);
          // white  characters  on  black  background
          tft.setTextColor(0xFFFF , 0x0000);
          getRestaurant(rest_dist[c].index , &rest);
          tft.print(rest.name);
          c--;
          if (c < 0) {
            c = 29;
          }
          tft.setCursor(0, 8 * c);
          getRestaurant(rest_dist[c].index , &rest);
          // black  characters  on  white  background
          tft.setTextColor(0x0000 , 0xFFFF);
          tft.print(rest.name);
          delay(500);
        }
      int joySelect = digitalRead(JOY_SEL);
      // switches mode and stores the x and y map coordinates of the
      // currently selected restaurant.
      if (joySelect == LOW) {
        flag = 0;
          while (joySelect == LOW) {
            // Saves the current state of the button for the next time
            // through the loop
            joySelect = digitalRead(JOY_SEL);
          }
          // converts the longitude and latitude values of the
          // selected restaurant to x and y values.
          int32_t lon = rest.lon;
          int32_t lat = rest.lat;
          int x = lon_to_x(lon);
          int y = lat_to_y(lat);
          // ensures the screen stays in the map boundaries.
          if ((x - MAP_DISP_WIDTH/2) > 0 && (x - MAP_DISP_WIDTH/2) < (LCD_WIDTH - MAP_DISP_WIDTH)) {
            yegCurrentX = x - MAP_DISP_WIDTH/2;
            cursorX = MAP_DISP_WIDTH/2;
          } else if ((x - yegCurrentX) < 0) {
              yegCurrentX = 0;
              // sets cursor's x coordinate at the restaurants position if it cannot be the centre.
              if (x > 0) {
                cursorX = x;
                // sets cursor's x coordinate at the boundary if the restaurant beyond the
                // left boundary.
              } else if (x < 0) {
                cursorX = 5;
                }
            // ensures the screen stays in the map boundaries.
          } else if ((x - yegCurrentX) > (LCD_WIDTH - MAP_DISP_WIDTH)) {
              yegCurrentX = LCD_WIDTH - MAP_DISP_WIDTH;
              if (x < LCD_WIDTH) {
                // sets cursor's x coordinate at the restaurants position if it cannot be the centre.
                cursorX = x;
                // sets cursor's x coordinate at the boundary if the restaurant beyond the
                // right boundary.
              } else if (x > LCD_WIDTH) {
                cursorX = LCD_WIDTH - 5;
              }
          }
          // ensures the screen stays in the map boundaries.
          if ((y - MAP_DISP_HEIGHT/2) > 0 && (y - MAP_DISP_HEIGHT/2) < (LCD_HEIGHT - MAP_DISP_HEIGHT)) {
            yegCurrentY = y - MAP_DISP_HEIGHT/2;
            cursorY = MAP_DISP_HEIGHT/2;
          } else if ((y - yegCurrentY) < 0) {
              yegCurrentY = 0;
              // sets cursor's y coordinate at the restaurants position if it cannot be the centre.
              if (y > 0) {
                cursorY = y;
                // sets cursor's y coordinate at the boundary if the restaurant beyond the
                // top boundary.
              } else if (y < 0) {
                cursorY = 5;
              }
            // ensures the screen stays in the map boundaries.
          } else if ((y - yegCurrentY) > (LCD_HEIGHT - MAP_DISP_HEIGHT)) {
              yegCurrentY = LCD_HEIGHT - MAP_DISP_HEIGHT;
              if (y < LCD_HEIGHT) {
                // sets cursor's y coordinate at the restaurants position if it cannot be the centre.
                cursorY = y;
                // sets cursor's y coordinate at the boundary if the restaurant beyond the
                // bottom boundary.
              } else if (y > LCD_HEIGHT) {
                cursorY = LCD_HEIGHT - 5;
              }
          }
          tft.fillScreen(ILI9341_BLACK);
          lcd_image_draw(&yegImage, &tft, yegCurrentX, yegCurrentY,
            0, 0, MAP_DISP_WIDTH, MAP_DISP_HEIGHT);
          redrawCursor(cursorX, cursorY, cursorX, cursorY);
          break;
          }
      }
}


/*
Runs the code for mode 1, including the list of the closest 30 restaurants.
*/
void mode1() {
    tft.fillScreen(ILI9341_BLACK);
    processRest();
}


// Main function
int main() {
	setup();
  // runs mode 0 is flag is set to 0 and mode 1 if flag is set to 1.
  while (true) {
    if (flag == 0) {
        mode0();
    } else if (flag == 1) {
        mode1();
    }
  }

	Serial.end();
	return 0;
}
