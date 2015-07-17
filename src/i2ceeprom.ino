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

// Support for 24Cxx external EEPROMs.
// To use:
//  * Change the "#if 0" to "#if 1".
//  * Add the following line to the embedis_dictionaries[] in config.ino.
//    {"EEPROM", &embedis_ram_commands, (void*)&arduino_i2ceeprom_access},
//  * Adjust defines to match your device.

#if 0

#include <Wire.h>

#define I2C_EEPROM_ADDRESS 0x00
#define I2C_EEPROM_BITSIZE 8192
#define I2C_EEPROM_WRDELAY 5

size_t arduino_i2ceeprom_size() {
    Wire.begin();
    return I2C_EEPROM_BITSIZE / 8;
}

char arduino_i2ceeprom_fetch(size_t pos) {
    byte addr = 0x50 | I2C_EEPROM_ADDRESS | (pos >> 8);
    Wire.beginTransmission(addr);
    Wire.write(pos & 0xFF);
    Wire.endTransmission();
    Wire.requestFrom(addr, (byte)1);
    if (Wire.available()) return Wire.read();
    else return 0xFF;
}

void arduino_i2ceeprom_store(size_t pos, char value) {
    byte addr = 0x50 | I2C_EEPROM_ADDRESS | (pos >> 8);
    Wire.beginTransmission(addr);
    Wire.write(pos & 0xFF);
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
