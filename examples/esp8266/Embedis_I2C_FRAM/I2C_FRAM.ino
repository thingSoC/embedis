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

/* Using I2C based memories, so include "wire" */
#include <Wire.h>

/* I2C_FRAM (MB85RC256, FM24V10, etc.) */
/* e.g. https://www.adafruit.com/products/1895 */
#include "Adafruit_FRAM_I2C.h"

/* Example wiring for the Adafruit I2C FRAM breakout */
/* Connect SCL    to analog 5
   Connect SDA    to analog 4
   Connect VDD    to 5.0V DC
   Connect GROUND to common ground */
   
Adafruit_FRAM_I2C fram = Adafruit_FRAM_I2C();

/* the settings for your particlar I2C_FRAM type... */
#define  I2C_FRAM_MODEL  FM24V10-G /* Cypress/Ramtron/etc. */
#define  I2C_FRAM_SIZE   131072    /* FM24V10 = 128K x 8   */
uint8_t  I2C_FRAM_Addr = 0x57;     /* A2 = A1 = A0 = "1"   */
 
void setup_I2C_FRAM() 
{
    setup_I2C_FRAM( F("I2C_FRAM") );
}

void setup_I2C_FRAM(const String& dict) 
{
   if (fram.begin(I2C_FRAM_Addr)) {
     LOG( String() + F("[ Found I2C_FRAM ]") );
   } else {
     LOG( String() + F("[ No I2C_FRAM found ... check your connections and address setting! ]") );
     while (1);
   }
  
    Embedis::dictionary( dict,
        (I2C_FRAM_SIZE),
        [](size_t pos) -> char { return fram.read8(pos); },
        [](size_t pos, char value) { fram.write8(pos, value); },
        []() { }
    );
}
