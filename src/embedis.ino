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

int tsoc_pwr_pin=23;
int tsoc_rst_pin=6;
int tsoc_refclk_pin=3;
int tsoc_rtcclk_pin=4;

// Embedis is going to use the primary serial port.
void setup() {
    pinMode(tsoc_pwr_pin, OUTPUT);
    pinMode(tsoc_rst_pin, OUTPUT);
    pinMode(tsoc_refclk_pin, OUTPUT);
    pinMode(tsoc_rtcclk_pin, OUTPUT);
    
    digitalWrite(tsoc_pwr_pin, HIGH);
    digitalWrite(tsoc_rst_pin, HIGH);
    digitalWrite(tsoc_rtcclk_pin, LOW);    
    digitalWrite(tsoc_refclk_pin, LOW);

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
        String temperature = String(random(7130, 7150) / 100.0);
        embedis_publish("temperature", temperature.c_str(), temperature.length());
    }
}

// Forward declarations for included device support.
// Feel free to move this to a header filer instead.
extern const embedis_ram_access arduino_eeprom_access;
extern const embedis_ram_access arduino_i2ceeprom_access;
extern const embedis_ram_access arduino_fram_access;

