/* Example program for Arduino */
/**
  ******************************************************************************
  *
  * @file        embedis.ino
  * @author      Dave Turnbull
  * @version     0.0.1
  * @date        2015-06-29
  * @copyright   PatternAgents, LLC
  * @brief       Embedis Example Sketch for the Arduino IDE
  *     
  ******************************************************************************
  */

// IMPORTANT: Arduino requires that you rename config.c to config.ino

#include "embedis.h"

// Embedis is going to use the primary serial port.
void setup() {
    embedis_init();
    Serial.begin(57600);
}

// In the main loop, send any characters received over
// the serial port to Embedis for processing.
void loop() {
    int b = Serial.read();
    if (b >= 0) embedis_in(b);
}

// When Embedis needs to send a response, it will
// call this function for every character.
void embedis_out(char b) {
    Serial.write(b);
}

// Forward declarations for included device support.
// Feel free to move this to a header filer instead.
extern const embedis_ram_access arduino_eeprom_access;
extern const embedis_ram_access arduino_i2ceeprom_access;
