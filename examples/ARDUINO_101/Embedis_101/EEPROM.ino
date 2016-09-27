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

// Add an "EEPROM" dictionary to Embedis, using the Ardunio EEPROM API
//
// To configure an EEPROM dictionary, call setup_EEPROM from your
// main setup() function. Optionally, supply the database name you want to use.
// e.g. setup_EEPROM();
//      setup_EEPROM( F("EEPROM") );
//
// Use the Embedis "select" command to enable your optional database name
// e.g. embedis-> select EEPROM
// The Embedis default database is "EEPROM", using the internal EEPROM memory.
//
#include <EEPROM.h>

// Set your particular EEPROM size, which may want to be less than the physical device size.
// As an example, a 16K byte EEPROM might use only the first 4096 bytes to
// hold an IMPI, DeviceTree or other persistant data structure. 
// Since Embedis writes from high memory to low memory, you could use 12K bytes
// of that memory for an Embedis Keystore, coexisting with the other persistant data structure.
// If E2END isn't defined you can uncoment the line below and manually set the size (in bytes).
//
#ifndef E2END
  #define E2END 1023
#endif
const size_t emEEPROM_SIZE = E2END + 1;

void setup_EEPROM() 
{
    setup_EEPROM( F("EEPROM") );
}

void setup_EEPROM(const String& dict) 
{
    EEPROM.begin();
    Embedis::dictionary( dict,
        emEEPROM_SIZE,
        [](size_t pos) -> char { return EEPROM.read(pos); },
        [](size_t pos, char value) { EEPROM.write(pos, value); },
        []() { delay(10); }
    );
        LOG( String() + F("[ Embedis : EEPROM dictionary installed ]") );
        LOG( String() + F("[ Embedis : EEPROM dictionary selected ]") );
        LOG( String() + F("[ Embedis : Note Bene! EEPROM 'Set' takes a long time on Arduino101... (wait for it!)]") );
}

