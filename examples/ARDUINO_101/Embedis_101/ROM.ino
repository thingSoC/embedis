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

// Add an "ROM" dictionary to Embedis
//
// To configure an ROM dictionary, call setup_ROM from your
// main setup() function. Optionally, supply the database name you want to use.
// e.g. setup_ROM();
//      setup_ROM( F("ROM") );
//
// Use the Embedis "select" command to enable your optional database name
// e.g. embedis-> select ROM
//
const static char * kvs_rom[] PROGMEM = {
  "vendor", "patternagents", 
  "mykey", "somevalue",
  0x00, 0x00
  };
const size_t kvs_rom_size = (sizeof(kvs_rom))/(sizeof(*kvs_rom));

void setup_ROM() 
{
    setup_ROM( F("ROM") );
}

void setup_ROM(const String& dict) 
{
    Embedis::dictionary( 
        dict,
        kvs_rom_size,
        [](size_t pos) -> char { return rom_kvs_fetch(pos); },
        [](size_t pos, char value) { rom_kvs_store(pos, value); },
        []() { delay(1); }
    );
        LOG( String() + F("[ Embedis : ROM dictionary installed ]") );
        LOG( String() + F("[ Embedis : Note Bene! ROM dictionary is READ_ONLY! ]") );
}

static char rom_kvs_fetch(size_t pos)
{
  char retval;
  retval = pgm_read_byte_near(kvs_rom + pos);
  return retval;
}

static void rom_kvs_store(size_t pos, char value)
{
  // it's read-only!
   delay(1);
}
