/* Example program for Arduino */
/**
  ******************************************************************************
  *
  * @file        embedis.ino
  * @copyright   PatternAgents, LLC
  * @brief       Embedis Example Sketch for the Arduino IDE
  *     
  ******************************************************************************
  */

// IMPORTANT: Arduino requires that you rename config.c to config.ino

#include "embedis.h"

// Embedis is going to use the primary serial port.
void setup() {
    Serial.begin(57600);
}

// Create embedis_state_0 for use on serial port.
// 128 byte input buffer supporting 8 arguments.
EMBEDIS_STATE_INSTANCE(0, serial_write, 128, 8);
void serial_write(char b) {
    Serial.write(b);
}

void loop() {
    // In the main loop, send any characters received over
    // the serial port to Embedis for processing.
    int b = Serial.read();
    if (b >= 0) embedis_in(&embedis_state_0, b);
    
    // Publish a temperature message every 5 seconds
    static unsigned long time_temperature = millis();
    if (millis() > time_temperature) {
        time_temperature = millis() + 5000;
        String temperature = random(7130, 7150) / 100.0;
        embedis_publish("temperature", temperature.c_str(), temperature.length());
    }
}

// Forward declarations for included device support.
// Feel free to move this to a header filer instead.
extern const embedis_ram_access arduino_eeprom_access;
extern const embedis_ram_access arduino_i2ceeprom_access;
