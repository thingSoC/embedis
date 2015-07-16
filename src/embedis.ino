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

// Support for internal EEPROM. Some devices do not have EEPROM (Due).
// Uncomment this section and the EEPROM line in config.ino to enable.

//#include <EEPROM.h>
//
//size_t arduino_eeprom_size() {
//    // E2END is from GCC headers for your specific hardware.
//    return E2END + 1;
//}
//
//char arduino_eeprom_fetch(size_t pos) {
//    return EEPROM.read(pos);
//}
//
//void arduino_eeprom_store(size_t pos, char value) {
//    EEPROM.write(pos, value);
//}
//
//const embedis_ram_access arduino_eeprom_access = {
//    arduino_eeprom_size,
//    arduino_eeprom_fetch,
//    arduino_eeprom_store
//};
