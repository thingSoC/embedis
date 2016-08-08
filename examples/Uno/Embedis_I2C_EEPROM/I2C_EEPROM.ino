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

/* extEEPROM Library from: https://github.com/PaoloP74/extEEPROM */
#include <extEEPROM.h>

/* define the device size, number of devices, page size */
#define I2C_EEPROM_SIZE 8192
extEEPROM ext_i2c_eeprom(kbits_64, 1, 32);
 
void setup_I2C_EEPROM() 
{
    setup_I2C_EEPROM( F("I2C_EEPROM") );
}

void setup_I2C_EEPROM(const String& dict) 
{
  uint8_t ext_i2c_eepromStatus = ext_i2c_eeprom.begin(twiClock400kHz);
  if (ext_i2c_eepromStatus) {
    LOG( String() + F("[ No I2C_EEPROM found ... check your connections and address setting! ]") );
  } else {
    LOG( String() + F("[ Found I2C_EEPROM ]") );
  }


    Embedis::dictionary( dict,
        (I2C_EEPROM_SIZE),
        [](size_t pos) -> char { return ext_i2c_eeprom.read(pos); },
        [](size_t pos, char value) { ext_i2c_eeprom.write(pos, value); },
        []() { }
    );

}
