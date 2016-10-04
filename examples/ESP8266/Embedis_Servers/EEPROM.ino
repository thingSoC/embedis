/*  Embedis - Embedded Dictionary Server
    Copyright (C) 2015, 2016 PatternAgents, LLC

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

// The esp8266 does not have real EEPROM internal to the device, 
// so it is emulated using SPI Flash memory. 
// Use "SET/Write" sparingly to avoid SPI Flash memory wear leveling issues.
// (we show in this example how to use Embedis to hold configuration settings.)
//
// To configure an EEPROM dictionary, call setup_esp8266_EEPROM from your
// main setup() function. Optionally, supply the database name you want to use.
// e.g. setup_EEPROM();
//      setup_EEPROM( F("MYEEPROM") );
//
// Use the Embedis "select" command to enable your optional database name
// e.g. embedis-> select MYEEPROM
// The Embedis default database is "EEPROM", using the internal EEPROM memory.
// (or in this specific case, Emulated EEPROM...)
//
#include <EEPROM.h>

// We need this because the ESP8266 EEPROM is emulated in SPI Flash
// this is also the case in the Arduino Due platform
#include "spi_flash.h"

void setup_EEPROM() 
{
    setup_EEPROM( F("EEPROM") );
}

void setup_EEPROM(const String& dict) 
{
    EEPROM.begin(SPI_FLASH_SEC_SIZE);
    Embedis::dictionary( dict,
        SPI_FLASH_SEC_SIZE,
        [](size_t pos) -> char { return EEPROM.read(pos); },
        [](size_t pos, char value) { EEPROM.write(pos, value); },
        []() { EEPROM.commit(); }
    );
    LOG( String() + F("[ Embedis : EEPROM dictionary installed ]") );
    LOG( String() + F("[ Embedis : EEPROM dictionary selected ]") );
}

