/*  Embedis - Embedded Dictionary Server
    Copyright (C) 2015 PatternAgents, LLC

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
/**
  ******************************************************************************
  *
  * @file        i2ceeprom.ino
  * @copyright   PatternAgents, LLC
  * @brief       The Embedis Dictionary
  *
  ******************************************************************************
  */

// Support for 24Cxx external EEPROMs.
// To use:
//  * Change the "#if 0" to "#if 1".
//  * Add the following line to the embedis_dictionaries[] in config.ino.
//    {"EEPROM", &embedis_ram_commands, (void*)&arduino_i2ceeprom_access},
//  * Adjust defines to match your device.

#if 1

#include <Wire.h>

#define I2C_EEPROM_ADDRESS 0x54
#define I2C_EEPROM_BITSIZE 65536
#define I2C_EEPROM_BITSPERBYTE 8
#define I2C_EEPROM_WRDELAY 5

size_t arduino_i2ceeprom_size() {
    Wire.begin();
    return (I2C_EEPROM_BITSIZE / I2C_EEPROM_BITSPERBYTE);
}

char arduino_i2ceeprom_fetch(size_t pos) {
    uint8_t hiaddr = (uint8_t) (pos >> 8);
    uint8_t loaddr = (uint8_t) (pos & 0x00ff);
    Wire.beginTransmission(I2C_EEPROM_ADDRESS);
    Wire.write(hiaddr);
    Wire.write(loaddr);
    Wire.endTransmission();
    Wire.requestFrom((int) I2C_EEPROM_ADDRESS, (int)1);
    // todo: check return status - need all eight bits
    if (Wire.available()) return Wire.read();
    else return 0xFF;
}

void arduino_i2ceeprom_store(size_t pos, char value) {
    uint8_t hiaddr = (uint8_t) (pos >> 8);
    uint8_t loaddr = (uint8_t) (pos & 0x00ff);
    // insert read before write check to reduce wear...
    Wire.beginTransmission(I2C_EEPROM_ADDRESS);
    Wire.write(hiaddr);
    Wire.write(loaddr);
    Wire.write(value);
    Wire.endTransmission();
    delay(I2C_EEPROM_WRDELAY);
}

const embedis_ram_access arduino_i2ceeprom_access = {
    arduino_i2ceeprom_size,
    arduino_i2ceeprom_fetch,
    arduino_i2ceeprom_store
};

#endif
