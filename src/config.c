#include "embedis.h"

// This is your Embedis configuration.
// This file is intended to be modified.

// The argc and argv guarantee:
// argv[0] is always the command in uppercase.
// All args are zero-terminated. However, to support binary payloads
// the commands are guaranteed to be in order. This allows you to subtract
// the pointers, plus the zero, to obtain the exact length.
// e.g. ptrdiff_t argv1len = argv[2] - argv[1] - 1;

static void embedis_command_missing(int argc, const char* argv[]) {
    // This default handler can be specialized to support dynamic commands.
    // Normally, it just returns an error is the command isn't found.
    embedis_response_error(EMBEDIS_UNKNOWN_COMMAND);
}

// Adjust this call table to support the desired command set.
// Commands must be upercase or they will not be found.
const embedis_command embedis_commands[] = {
    {"GET", embedis_GET},
    {"SET", embedis_SET},
    {"DEL", embedis_DEL},
    {0, embedis_command_missing}
};

// Dictionary handlers for values stored in NVRAM.
const embedis_dict_handler embedis_nvram_key = {
    embedis_dict_GET, embedis_dict_SET, embedis_dict_DEL
};

// Dictionary handlers for values stored in ROM.
// Note that SET and DEL default to returning an error.
const embedis_dict_handler embedis_rom_key = {
    embedis_dict_rom, embedis_dict_error, embedis_dict_error
};

// Dictionary key configuration.
const embedis_dict_config embedis_dict_keys[] = {
    // Example for read-only data stored in ROM.
    {"vendor", "AE9RB", &embedis_rom_key},
    // Example for an entry stored in NVRAM with an assigned number (1000).
    // Only the number is stored in NVRAM instead of the whole key.
    // In this example, two bytes are stored instead of 22.
    // Valid range is from 1 to 32767.
    {"asset_identification", (void*)1000, &embedis_nvram_key},
    // The default hander allows storage of any arbitraty key-value data.
    {0, 0, &embedis_nvram_key}
};
