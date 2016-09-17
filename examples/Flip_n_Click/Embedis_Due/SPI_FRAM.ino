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

/* FRAM_Click */

#include "flip_n_click_pins.h"

/* FRAM CLICK Pin Assignment
 * FRAM installed in Socket "C" on the Flip-n-Click Board
 * 
 * To install in Socket "A", just change the "C" suffix to "A"
 * i.e.  FRAM_CS   = CSNA; 
 *       FRAM_SCK  = SCKA;
 *       FRAM_MISO = MISOA;
 *       FRAM_MOSI = MOSIA;
 */
uint8_t  FRAM_CS   = CSNC;  /* FRAM Chip Select */
uint8_t  FRAM_SCK  = SCKC;  /* FRAM Clock */
uint8_t  FRAM_MISO = MISOC; /* FRAM Master Out */
uint8_t  FRAM_MOSI = MOSIC; /* FRAM Mater In */
uint8_t  FRAM_HOLD = RSTSC;  /* FRAM HOLD */
uint8_t  FRAM_WRTP = PWMC;  /* FRAM Write Protect */
uint32_t FRAM_SIZE = 32768; /* MB85RS256 is 256K bits or 32768 x 8 bits*/

#include <SPI.h>
#include "Adafruit_FRAM_SPI.h"

/* Create the Adafruit_FRAM_SPI instance */
//Adafruit_FRAM_SPI fram = Adafruit_FRAM_SPI(FRAM_CS);  /* use hardware SPI */
Adafruit_FRAM_SPI fram = Adafruit_FRAM_SPI(FRAM_SCK, FRAM_MISO, FRAM_MOSI, FRAM_CS); /* use software SPI */

void setup_SPI_FRAM() 
{
    setup_SPI_FRAM( F("SPI_FRAM") );
}

void setup_SPI_FRAM(const String& dict) 
{
  /* Configure the FRAM control pins correctly */
  pinMode(FRAM_HOLD, OUTPUT);
  digitalWrite(FRAM_HOLD, HIGH);
  pinMode(FRAM_WRTP, OUTPUT);
  digitalWrite(FRAM_WRTP, HIGH);
  if (fram.begin()) {
    LOG( String() + F("[ Embedis : Found SPI_FRAM ]") );
    fram.writeEnable(false);
  } else {
    LOG( String() + F("[ Embedis : No SPI_FRAM found ... check your connections and address setting! ]") );
    while (1);
  }

    Embedis::dictionary( dict,
        (FRAM_SIZE),
        [](size_t pos) -> char { return fram.read8(pos); },
        [](size_t pos, char value) { fram.writeEnable(true); fram.write8(pos, value); fram.writeEnable(false); },
        []() { }
    );
        LOG( String() + F("[ Embedis : SPI_FRAM dictionary installed ]") );
}
