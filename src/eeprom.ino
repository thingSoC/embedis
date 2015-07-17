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

// Support for Arduino internal EEPROM. Some devices do not have EEPROM (Due).
// If you get an error about EEPROM.h not found, your device does not have EEPROM
// but you can use the i2ceeprom with an external EEPROM.

// To use, change the "#if 0" to "#if 1" and add the following line to
// the embedis_dictionaries[] in config.ino.
// {"EEPROM", &embedis_ram_commands, (void*)&arduino_eeprom_access},

#if 0

#include <EEPROM.h>

size_t arduino_eeprom_size() {
    // E2END is from GCC headers for your specific hardware.
    return E2END + 1;
}

char arduino_eeprom_fetch(size_t pos) {
    return EEPROM.read(pos);
}

void arduino_eeprom_store(size_t pos, char value) {
    EEPROM.write(pos, value);
}

const embedis_ram_access arduino_eeprom_access = {
    arduino_eeprom_size,
    arduino_eeprom_fetch,
    arduino_eeprom_store
};

#endif
