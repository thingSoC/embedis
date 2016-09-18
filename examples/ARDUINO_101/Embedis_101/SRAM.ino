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

// Add an "SRAM" dictionary to Embedis
//
// To configure an SRAM dictionary, call setup_SRAM from your
// main setup() function. Optionally, supply the database name you want to use.
// e.g. setup_SRAM();
//      setup_SRAM( F("SRAM") );
//
// Use the Embedis "select" command to enable your optional database name
// e.g. embedis-> select SRAM
//

#include "Stream.h"

// Set your particular SRAM dictionary size
// 128 bytes by default
const size_t SRAMDICT_SIZE = 128;
char  kvs_data[SRAMDICT_SIZE];

void setup_SRAM() 
{
    setup_SRAM( F("SRAM") );
}

void setup_SRAM(const String& dict) 
{
    Embedis::dictionary( 
        dict,
        SRAMDICT_SIZE,
        [](size_t pos) -> char { return ram_kvs_fetch(pos); },
        [](size_t pos, char value) { ram_kvs_store(pos, value); },
        []() { delay(1); }
    );
        LOG( String() + F("[ Embedis : SRAM dictionary installed ]") );
        LOG( String() + F("[ Embedis : Note Bene! SRAM dictionary is NOT persistent! ]") );
        LOG( String() + F("[ Embedis : Note Bene! SRAM dictionary is lost on Power down! ]") );
}

static char ram_kvs_fetch(size_t pos)
{
  char retval;
  retval = char (kvs_data[pos]);
  return retval;
}


static void ram_kvs_store(size_t pos, char value)
{
   kvs_data[pos]=value;
}
