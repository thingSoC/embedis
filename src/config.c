#include "embedis.h"

// This is your Embedis configuration.
// This file is intended to be modified.

// The argc and argv guarantee:
// argv[0] is always the command in uppercase.
// All args are zero-terminated. However, to support binary payloads
// the commands are guaranteed to be in order. This allows you to subtract
// the pointers, plus the zero, to obtain the exact length.
// e.g. ptrdiff_t argv1len = argv[2] - argv[1] - 1;

static void embedis_command_missing(embedis_state* state) {
    // This default handler can be specialized to support dynamic commands.
    // Normally, it just returns an error if the command isn't found.
    embedis_response_error(EMBEDIS_UNKNOWN_COMMAND);
}

// Adjust this call table to support the desired command set.
// Commands must be upercase or they will not be found.
const embedis_command embedis_commands[] = {
    {"SELECT", embedis_SELECT},
    {"KEYS", embedis_KEYS},
    {"GET", embedis_GET},
    {"SET", embedis_SET},
    {"DEL", embedis_DEL},
    {0, embedis_command_missing}
};

const embedis_dictionary embedis_dictionaries[] = {
    {"ROM", embedis_rom_SELECT, embedis_rom_KEYS, embedis_rom_GET, embedis_rom_SET, embedis_rom_DEL},
    {"EEPROM", embedis_eeprom_SELECT, embedis_eeprom_KEYS, embedis_eeprom_GET, embedis_eeprom_SET, embedis_eeprom_DEL}
};

const char* embedis_dictionary_rom[] = {
    "vendor", "AE9RB",
    0
};

// This list maps keys to a numeric value (0-23767).
// It is used to conserve space on an EEPROM by
// storing the number instead of a string and length.
const embedis_dictionary_key embedis_dictionary_keys[] = {
    {"asset_identification", 1000},
    {0,0}
};
