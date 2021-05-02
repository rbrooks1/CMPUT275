/*
Name: Ryan Brooks, Charlie Chen
ID: 1530605, 1530488
Course: CmPUT 275 winter 2019
assignment: Assignment 2 part 2: Client
Credits: In README
*/
#include <Arduino.h>
#include <Adafruit_ILI9341.h>
#include <SD.h>
#include "consts_and_types.h"
#include "map_drawing.h"

// the variables to be shared across the project, they are declared here!
shared_vars shared;

Adafruit_ILI9341 tft = Adafruit_ILI9341(clientpins::tft_cs, clientpins::tft_dc);


void setup() {
  // initialize Arduino
  init();

  // initialize zoom pins
  pinMode(clientpins::zoom_in_pin, INPUT_PULLUP);
  pinMode(clientpins::zoom_out_pin, INPUT_PULLUP);

  // initialize joystick pins and calibrate centre reading
  pinMode(clientpins::joy_button_pin, INPUT_PULLUP);
  // x and y are reverse because of how our joystick is oriented
  shared.joy_centre = xy_pos(analogRead(clientpins::joy_y_pin), analogRead(clientpins::joy_x_pin));

  // initialize serial port
  Serial.begin(9600);
  Serial.flush();  // get rid of any leftover bits

  // initially no path is stored
  shared.num_waypoints = 0;

  // initialize display
  shared.tft = &tft;
  shared.tft->begin();
  shared.tft->setRotation(3);
  shared.tft->fillScreen(ILI9341_BLUE);  // so we know the map redraws properly

  // initialize SD card
  if (!SD.begin(clientpins::sd_cs)) {
      Serial.println("Initialization has failed. Things to check:");
      Serial.println("* Is a card inserted properly?");
      Serial.println("* Is your wiring correct?");
      Serial.println("* Is the chipSelect pin the one for your shield or module?");
      while (1) {}  // nothing to do here, fix the card issue and retry
  }
  // initialize the shared variables, from map_drawing.h
  // doesn't actually draw anything, just initializes values
  initialize_display_values();

  // initial draw of the map, from map_drawing.h
  draw_map();
  draw_cursor();

  // initial status message
  status_message("FROM?");
}


/** Writes an uint32_t to Serial, starting from the least-significant
 * and finishing with the most significant byte.
    Input:
    num(int32_t)
 */
void int32_to_serial(int32_t num) {
  Serial.print(num);
}


// implements the joystick movement and zooming, taken from the template code on EClass,
// unedited.
void process_input() {
  // read the zoom in and out buttons
  shared.zoom_in_pushed = (digitalRead(clientpins::zoom_in_pin) == LOW);
  shared.zoom_out_pushed = (digitalRead(clientpins::zoom_out_pin) == LOW);

  // read the joystick button
  shared.joy_button_pushed = (digitalRead(clientpins::joy_button_pin) == LOW);

  // joystick speed, higher is faster
  const int16_t step = 64;

  // get the joystick movement, dividing by step discretizes it
  // currently a far joystick push will move the cursor about 5 pixels
  xy_pos delta(
    // the funny x/y swap is because of our joystick orientation
    (analogRead(clientpins::joy_y_pin)-shared.joy_centre.x)/step,
    (analogRead(clientpins::joy_x_pin)-shared.joy_centre.y)/step);
  delta.x = -delta.x;  // horizontal axis is reversed in our orientation

  // check if there was enough movement to move the cursor
  if (delta.x != 0 || delta.y != 0) {
    // if we are here, there was noticeable movement

    // the next three functions are in map_drawing.h
    erase_cursor();       // erase the current cursor
    move_cursor(delta);   // move the cursor, and the map view if the edge was nudged
    if (shared.redraw_map == 0) {
      // it looks funny if we redraw the cursor before the map scrolls
      draw_cursor();      // draw the new cursor position
    }
  }
}

// sends an int32 through the serial connection on e byte at a time
// start(lon_lat_32): the starting point
// end(lon_lat_32): the ending point
void sendPoints(lon_lat_32 start, lon_lat_32 end) {
    Serial.print('R');
    Serial.print(' ');
    int32_to_serial(start.lat);
    Serial.print(' ');
    int32_to_serial(start.lon);
    Serial.print(' ');
    int32_to_serial(end.lat);
    Serial.print(' ');
    int32_to_serial(end.lon);
    Serial.print("\n"); 
}


// calls the functions for zoom and cursor movement and handles the communication.
int main() {
  setup();
  bool isroute = false;
  // very simple finite state machine:
  // which endpoint are we waiting for?
  enum {WAIT_FOR_START, WAIT_FOR_STOP} curr_mode = WAIT_FOR_START;
  // the two points that are clicked
  lon_lat_32 start, end;
  shared.num_waypoints = 0;
  while (true) {
    // clear entries for new state
    shared.zoom_in_pushed = 0;
    shared.zoom_out_pushed = 0;
    shared.joy_button_pushed = 0;
    shared.redraw_map = 0;

    // reads the three buttons and joystick movement
    // updates the cursor view, map display, and sets the
    // shared.redraw_map flag to 1 if we have to redraw the whole map
    // NOTE: this only updates the internal values representing
    // the cursor and map view, the redrawing occurs at the end of this loop
    process_input();

    // if a zoom button was pushed, update the map and cursor view values
    // for that button push (still need to redraw at the end of this loop)
    // function zoom_map() is from map_drawing.h
    if (shared.zoom_in_pushed) {
      zoom_map(1);
      shared.redraw_map = 1;
    } else if (shared.zoom_out_pushed) {
      zoom_map(-1);
      shared.redraw_map = 1;
    }
    // if the joystick button was clicked
    if (shared.joy_button_pushed) {
      if (curr_mode == WAIT_FOR_START) {
        // if we were waiting for the start point, record it
        // and indicate we are waiting for the end point
        start = get_cursor_lonlat();
        curr_mode = WAIT_FOR_STOP;
        status_message("TO?");

        // wait until the joystick button is no longer pushed
        while (digitalRead(clientpins::joy_button_pin) == LOW) {}
      } else {
        // if we were waiting for the end point, record it
        // and then communicate with the server to get the path
        end = get_cursor_lonlat();
        // sends the start and end points through the serial connection
        sendPoints(start, end);
        int i = 0;
        while (true) {
            while (Serial.available() == 0) {}
            String inputc = Serial.readStringUntil(' ');
            if (inputc == "N") {
                String f = Serial.readStringUntil('\n');
                // read in the number of waypoints
                shared.num_waypoints = f.toInt();
                Serial.flush();
                isroute = true;
                // ensures there is a path less than 500
                if ((shared.num_waypoints == 0) || (shared.num_waypoints > 500)) {
                  isroute = false;
                  shared.redraw_map = 0;
                  break;
                }
                Serial.print("A\n");
            } else if ((inputc == "W") && (isroute == true)) {
                lon_lat_32 wP;
                // reads the coordinates of the waypoints
                String l1 = Serial.readStringUntil(' ');
                String l2 = Serial.readStringUntil('\n');
                wP.lat = l1.toInt();
                wP.lon = l2.toInt();
                // populates array with coordinates of vertices along the path
                shared.waypoints[i] = wP;
                Serial.flush();
                Serial.print("A\n");
                i++;
            } else if (inputc == "E") {
                Serial.flush();
                isroute = true;
                shared.redraw_map = 1;
                break;
            }
        }
        curr_mode = WAIT_FOR_START;
        // wait until the joystick button is no longer pushed
        while (digitalRead(clientpins::joy_button_pin) == LOW) {}
      }
    }
    if (shared.redraw_map) {
        // redraw the map and cursor
        draw_map();
        draw_cursor();
        // if there is a route then draw it
        if (isroute == true) {
            // draws blue lines between waypoints.
            for (int i = 1; i < shared.num_waypoints - 1; i++) {
                int16_t ycoord1 = latitude_to_y(shared.map_number, shared.waypoints[i - 1].lat);
                int16_t xcoord1 = longitude_to_x(shared.map_number, shared.waypoints[i - 1].lon);
                int16_t ycoord2 = latitude_to_y(shared.map_number, shared.waypoints[i].lat);
                int16_t xcoord2 = longitude_to_x(shared.map_number, shared.waypoints[i].lon);
                tft.drawLine(xcoord1, xcoord1, xcoord2, ycoord2, ILI9341_BLUE);
            }
        }
        // redraw the status message
        if (curr_mode == WAIT_FOR_START) {
            status_message("FROM?");
        } else {
            status_message("TO?");
        }
    }
  }

  Serial.flush();

  return 0;
}
