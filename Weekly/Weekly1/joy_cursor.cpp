/*
Name: Ryan Brooks
ID: 1530605
Course: CKPUT 275 winter 2019
assignment: Weekly exercise 1: Display and joystick
*/

#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_ILI9341.h>
#include "lcd_image.h"

#define TFT_DC 9
#define TFT_CS 10
#define SD_CS 6

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

#define DISPLAY_WIDTH  320
#define DISPLAY_HEIGHT 240
#define YEG_SIZE 2048

lcd_image_t yegImage = { "yeg-big.lcd", YEG_SIZE, YEG_SIZE };

#define JOY_VERT  A1  // should connect A1 to pin VRx
#define JOY_HORIZ A0  // should connect A0 to pin VRy
#define JOY_SEL   2

#define JOY_CENTER   512
#define JOY_DEADZONE 64

#define CURSOR_SIZE 9

// the cursor position on the display
int cursorX, cursorY;

// forward declaration for redrawing the cursor
void redrawCursor(uint16_t colour);

void setup() {
  init();

  Serial.begin(9600);

	pinMode(JOY_SEL, INPUT_PULLUP);

	tft.begin();

	Serial.print("Initializing SD card...");
	if (!SD.begin(SD_CS)) {
		Serial.println("failed! Is it inserted properly?");
		while (true) {}
	}
	Serial.println("OK!");

	tft.setRotation(3);

  tft.fillScreen(ILI9341_BLACK);

  // draws the centre of the Edmonton map, leaving the rightmost 48 columns black
	int yegMiddleX = YEG_SIZE/2 - (DISPLAY_WIDTH - 48)/2;
	int yegMiddleY = YEG_SIZE/2 - DISPLAY_HEIGHT/2;
	lcd_image_draw(&yegImage, &tft, yegMiddleX, yegMiddleY,
                 0, 0, DISPLAY_WIDTH - 48, DISPLAY_HEIGHT);

  // initial cursor position is the middle of the screen
  cursorX = (DISPLAY_WIDTH - 48)/2;
  cursorY = DISPLAY_HEIGHT/2;

  redrawCursor(ILI9341_RED);
}

// function that redraws the cursor at its new location
// after the joystick has been moved
void redrawCursor(uint16_t colour) {
  tft.fillRect(cursorX - CURSOR_SIZE/2, cursorY - CURSOR_SIZE/2,
               CURSOR_SIZE, CURSOR_SIZE, colour);
}

// function that allows the joystick to move without
// the map disappearing when the cursor on the lcd display
// moves
void processJoystick() {
  int xVal = analogRead(JOY_HORIZ);
  int yVal = analogRead(JOY_VERT);
  int buttonVal = digitalRead(JOY_SEL);
  // re initializing the below variables for redrawing sections of the map
  int yegMiddleX = YEG_SIZE/2 - (DISPLAY_WIDTH - 48)/2;
  int yegMiddleY = YEG_SIZE/2 - DISPLAY_HEIGHT/2;
  // now move the cursor
  // check if the joystick is moved or not.  If it is moved then proceed
    if (((xVal > (JOY_CENTER + JOY_DEADZONE)) || (yVal > (JOY_CENTER + JOY_DEADZONE))) ||
        ((xVal < (JOY_CENTER - JOY_DEADZONE)) || (yVal < (JOY_CENTER - JOY_DEADZONE)))) {
      // redraw parts of the map that the cursor has gone over
      lcd_image_draw(&yegImage, &tft, yegMiddleX + cursorX - 4, yegMiddleY + cursorY - 4,
                 cursorX - 4, cursorY - 4, CURSOR_SIZE, CURSOR_SIZE);
      if (yVal < JOY_CENTER - JOY_DEADZONE) {
        // check if the cursor is inside the boundary
        if (cursorY > 0) {
            // cursor speed control
            if ((yVal <= 448) && (yVal > 224)) {
                cursorY -= 1;
            } else if ((yVal <= 224) && (yVal > 1)) {
                cursorY -= 2;
            } else if  ((yVal <= 20) && (yVal >= 0)) {
                cursorY -= 3;
            }
        }
      } else if (yVal > JOY_CENTER + JOY_DEADZONE) {
        // check if the cursor is inside the boundary
        if (cursorY < 240) {
            // cursor speed control
            if ((yVal < 798) && (yVal >= 576)) {
                cursorY += 1;
            } else if ((yVal < 1022) && (yVal >= 798)) {
                cursorY += 2;
            } else if ((yVal <= 1023) && (yVal >= 1000)) {
                cursorY += 3;
            }
        }
      }

      // remember the x-reading increases as we push left
      if (xVal > JOY_CENTER + JOY_DEADZONE) {
        // check if the cursor is inside the boundary
        if (cursorX > 0) {
            // cursor speed control
            if ((xVal < 798) && (xVal >= 576)) {
                cursorX -= 1;
            } else if ((xVal < 999) && (xVal >= 798)) {
                cursorX -= 2;
            } else if ((xVal <= 1023) && (xVal >= 1000)) {
                cursorX -= 3;
            }
        }
      } else if (xVal < JOY_CENTER - JOY_DEADZONE) {
        // check if the cursor is inside the boundary
        if (cursorX < 268) {
            // cursor speed control
            if ((xVal <= 448) && (xVal > 224)) {
                cursorX += 1;
            } else if ((xVal <= 224) && (xVal > 21)) {
                cursorX += 2;
            } else if ((xVal <= 20) && (xVal >= 0)) {
                cursorX += 3;
            }
        }
      }

      // also, draw a black square at the old cursor position before
      // moving and drawing a red square

      redrawCursor(ILI9341_RED);

      delay(20);
  }
}

int main() {
	setup();

  while (true) {
    processJoystick();
  }

	Serial.end();
	return 0;
}
