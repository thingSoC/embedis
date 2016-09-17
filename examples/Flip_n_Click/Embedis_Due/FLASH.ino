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

/* FLASH for Arduino Due */
#include <DueFlashStorage.h>
DueFlashStorage FlashDue;

void setup_FLASH() {
    setup_FLASH( F("FLASH") );

}

void setup_FLASH(const String& dict) {
    Embedis::dictionary( dict,
        1024,
        [](size_t pos) -> char { return FlashDue.read(pos); },
        [](size_t pos, char value) { FlashDue.write(pos, value); },
        []() { }
    );
    LOG( String() + F("[ Embedis : FLASH dictionary cleared on each Upload! ]") );
    LOG( String() + F("[ Embedis : FLASH dictionary installed ]") );
    LOG( String() + F("[ Embedis : FLASH dictionary selected ]") );
}
