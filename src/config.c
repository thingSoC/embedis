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

/**
  ******************************************************************************
  *
  * @file        config.c
  * @author      Dave Turnbull
  * @version     0.0.1
  * @date        2015-06-29
  * @copyright   PatternAgents, LLC
  * @brief       The Embedis Dictionary Server Configuration
  *
  ******************************************************************************
  */

#include "embedis.h"

// This is your Embedis configuration.
// This file is intended to be modified.

// The argc and argv guarantee:
// argv[0] is always the command.
// All args are zero-terminated. However, to support binary payloads
// the commands are guaranteed to be in order. This allows you to subtract
// the pointers, plus the zero, to obtain the exact length.
// e.g. size_t argv1len = argv[2] - argv[1] - 1;


// This default handler can be specialized to support dynamic commands.
// Normally, it just returns an error if the command isn't found.
static void embedis_command_missing(embedis_state* state) {
    embedis_response_error(EMBEDIS_UNKNOWN_COMMAND);
}

// Adjust this call table to support the desired command set.
const embedis_command embedis_commands[] = {
    {"SELECT", embedis_SELECT},
    {"KEYS", embedis_KEYS},
    {"GET", embedis_GET},
    {"SET", embedis_SET},
    {"DEL", embedis_DEL},
    {"READ", embedis_READ},
    {"WRITE", embedis_WRITE},
    {0, embedis_command_missing}
};

// A ROM dictionary is useful for information about the device.
// Vendor name, device name, and firmware version are good examples.
static char const * const embedis_dictionary_rom[] = {
    "vendor", "PatternAgents",
    0
};

// An actual RAM dictionary is not very useful outside of testing.
// You'll want to implement this for some kind of non-volatile RAM.
// EEPROM, NVSRAM, FRAM, FLASH, etc.

#define MOCK_RAM_SIZE (64)

static char mock_ram[MOCK_RAM_SIZE];

static size_t mock_ram_size() {
    return MOCK_RAM_SIZE;
}

static char mock_ram_fetch(size_t pos) {
    if (pos >= MOCK_RAM_SIZE) return 0xFF;
    return mock_ram[pos];
}

static void mock_ram_store(size_t pos, char value) {
    if (pos >= MOCK_RAM_SIZE) return;
    mock_ram[pos] = value;
}

const embedis_ram_access mock_ram_access = {
    mock_ram_size,
    mock_ram_fetch,
    mock_ram_store
};

// Dictionaries for the SELECT command. The first one is the default.

const embedis_dictionary embedis_dictionaries[] = {
    {"ROM", &embedis_rom_commands, (void*)&embedis_dictionary_rom},
    // A RAM dictionary is not suggested for production.
    {"RAM", &embedis_ram_commands, (void*)&mock_ram_access},
    // Uncomment for Arduino EEPROM support
    // {"EEPROM", &embedis_ram_commands, (void*)&arduino_eeprom_access},
    {0}
};

// This list maps keys to a numeric value (1-32767).
// It is used to conserve space on an EEPROM by
// storing the number instead of a string and length.
const embedis_dictionary_key embedis_dictionary_keys[] = {
    {"asset_identification", 1000},
    {0}
};

// Example configuration for hardware keys.

static void mock_READ(embedis_state* state) {
    // Responds with the number of the mock requested
    embedis_response_simple(&state->argv[1][4]);
}

static void mock_WRITE(embedis_state* state) {
    embedis_response_error(EMBEDIS_OK);
}

static void mock_missing(embedis_state* state) {
    embedis_response_error(0);
}

const embedis_rw_key embedis_rw_keys[] = {
    {"mock0", mock_READ, mock_WRITE},
    {"mock1", mock_READ, mock_WRITE},
    {0, mock_missing, mock_missing}
};
