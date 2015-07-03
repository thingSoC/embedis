/* Example program for Arduino */

#include "embedis.h"
#include <EEPROM.h>

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

// Embedis will use EEPROM for dictionary storage.
// This configures the amount of storage available.
size_t embedis_eeprom_size() {
    // E2END is from GCC headers for your specific hardware.
    return E2END + 1;
}

// Fetch characters from dictionary storage.
size_t embedis_eeprom_fetch(size_t pos) {
    return EEPROM.read(pos);
}

// Put characters in dictionary storage.
void embedis_eeprom_store(size_t pos, char value) {
    EEPROM.write(pos, value);
}
