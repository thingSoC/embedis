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
/* SPI_FRAM (MB85RS256, FM25V10, etc.) */
/* e.g. https://www.adafruit.com/product/1897 */

/* 
 * SPI FRAM Pin Assignment for ESP8266
 * (see the Fritzing Wiring Diagram for details)
 */
uint8_t  FRAM_CS   = 15;       /* FRAM Chip Select */
uint8_t  FRAM_SCK  = 14;      /* FRAM Clock */
uint8_t  FRAM_MISO = 12;      /* FRAM Master Out */
uint8_t  FRAM_MOSI = 13;      /* FRAM Mater In */
//uint8_t  FRAM_HOLD = 16;      /* FRAM HOLD */
//uint8_t  FRAM_WRTP =  2;       /* FRAM Write Protect */
uint32_t FRAM_SIZE = 32768;   /* MB85RS256 is 256K bits or 32768 x 8 bits*/

#include <SPI.h>
#include "Adafruit_FRAM_SPI.h"

/* Create the Adafruit_FRAM_SPI instance */
//Adafruit_FRAM_SPI fram = Adafruit_FRAM_SPI(FRAM_CS);  /* use hardware SPI */
Adafruit_FRAM_SPI fram = Adafruit_FRAM_SPI(FRAM_SCK, FRAM_MISO, FRAM_MOSI, FRAM_CS); /* use software SPI */

/* the settings for your particlar SPI_FRAM type... */
#define  SPI_FRAM_MODEL  MB85RS256 /* Cypress/Ramtron/etc. */
 
void setup_SPI_FRAM() 
{
    setup_SPI_FRAM( F("SPI_FRAM") );
}

void setup_SPI_FRAM(const String& dict) 
{
  /* Configure the FRAM control pins correctly */
//  pinMode(FRAM_HOLD, OUTPUT);
//  digitalWrite(FRAM_HOLD, HIGH);
//  pinMode(FRAM_WRTP, OUTPUT);
//  digitalWrite(FRAM_WRTP, HIGH);

   if (fram.begin()) {
     LOG( String() + F("[ Found SPI_FRAM ]") );
   } else {
     LOG( String() + F("[ No SPI_FRAM found ... check your connections and pin settings! ]") );
     while (1);
   }
  
    Embedis::dictionary( dict,
        (FRAM_SIZE),
        [](size_t pos) -> char { return fram.read8(pos); },
        [](size_t pos, char value) { fram.writeEnable(true); fram.write8(pos, value); fram.writeEnable(false); },
        []() { }
    );
}
