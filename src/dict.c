/*  Embedis - Embedded Dictionary Server
    Copyright (C) 2015 Pattern Agents, LLC

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

/// @file

#include "embedis.h"


void embedis_SELECT(embedis_state* state) {
    if (state->argc < 2) {
        return embedis_response_error(EMBEDIS_ARGS_ERROR);
    }
    const embedis_dictionary* dict = &embedis_dictionaries[0];
    size_t i, length = state->argv[2] - state->argv[1] - 1;
    embedis_capitalize_arg(state, 1);
    while (dict->name) {
        i = 0;
        while (dict->name[i] == state->argv[1][i]) i++;
        if (i == length+1) {
            // found
            state->dictionary = dict;
            (*dict->select)(state);
            return;
        }
        dict++;
    }
    embedis_response_error(0);
}


void embedis_KEYS(embedis_state* state) {
    (*state->dictionary->keys)(state);
}


void embedis_GET(embedis_state* state) {
    if (state->argc != 2) {
        return embedis_response_error(EMBEDIS_ARGS_ERROR);
    }
    (*state->dictionary->get)(state);
}


void embedis_SET(embedis_state* state) {
    if (state->argc != 3) {
        return embedis_response_error(EMBEDIS_ARGS_ERROR);
    }
    (*state->dictionary->set)(state);
}


void embedis_DEL(embedis_state* state) {
    if (state->argc != 2) {
        return embedis_response_error(EMBEDIS_ARGS_ERROR);
    }
    (*state->dictionary->del)(state);
}


// ROM dictionary


void embedis_rom_SELECT(embedis_state* state) {
    embedis_response_error(EMBEDIS_OK);
}


void embedis_rom_KEYS(embedis_state* state) {
    embedis_response_error(0);
}


void embedis_rom_GET(embedis_state* state) {
    const char** rom = embedis_dictionary_rom;
    size_t i, length = state->argv[2] - state->argv[1] - 1;
    while (*rom) {
        i = 0;
        while (rom[0][i] == state->argv[1][i]) i++;
        if (i == length+1) {
            embedis_response_simple(rom[1]);
            return;
        }
        rom += 2;
    }
    embedis_response_null();
}


void embedis_rom_SET(embedis_state* state) {
    embedis_response_error(0);
}


void embedis_rom_DEL(embedis_state* state) {
    embedis_response_error(0);
}


/*
 EEPROM key-value data is stored starting at the last location.
 This allows it to, for example, share an EEPROM with another
 system like Device Tree.

 The following structure repeats until key_id_or_len is 0.

 key_id_or_len: 2 bytes
 -1...-32767 = well-known key
 1...32768 = key length

 key_name: not zero terminated!
 well-known keys are not stored.

 value_len: 2 bytes
 0...65535 = value length

 key_value: not zero terminated
 */


// Upon detection of an unformatted or corrupt EEPROM
// we will erase everything.
static void eeprom_reset() {
    embedis_eeprom_store(embedis_eeprom_size()-1, 0);
    embedis_eeprom_store(embedis_eeprom_size()-2, 0);
}

// get if value_pos != 0
// set if value != 0
// delete if value == 0
// returns 1 on success
static int eeprom_work(const char* key_name, int key_id_or_len, const char* value, size_t* value_len, size_t* value_pos) {

    size_t pos = embedis_eeprom_size(), deloffset = 0, key_pos;
    int i;
    char on_key = 1, found_key = 0;

    while (pos > 2) {

        if (on_key) {
            key_pos = pos;
            i = embedis_eeprom_fetch(pos-1) + (embedis_eeprom_fetch(pos-2) << 8);
            if (i==0) break; // end
            pos -= 2;

            if (deloffset) {
                embedis_eeprom_store(pos+deloffset+1, embedis_eeprom_fetch(pos+1));
                embedis_eeprom_store(pos+deloffset, embedis_eeprom_fetch(pos));
            }

            on_key = 0;

            if (i < 0) {
                if (i == key_id_or_len) found_key = 1;
                continue;
            }
            if (i >= pos) {
                eeprom_reset();
                return 0;
            }
            pos -= i;

            if (deloffset) {
                while (i) {
                    i--;
                    embedis_eeprom_store(pos+i+deloffset, embedis_eeprom_fetch(pos+i));
                }
                continue;
            }

            if (i != key_id_or_len) {
                // not same length
                continue;
            }
            found_key = 1;
            while (i) {
                i--;
                if (embedis_eeprom_fetch(pos+i) != key_name[i]) {
                    found_key = 0;
                    break;
                }
            }
            continue;
        }

        i = embedis_eeprom_fetch(pos-1) + (embedis_eeprom_fetch(pos-2) << 8);
        pos -= 2;
        if (i >= pos) {
            eeprom_reset();
            return 0;
        }
        pos -= i;

        if (deloffset) {
            i += 2;
            while (i) {
                i--;
                embedis_eeprom_store(pos+i+deloffset, embedis_eeprom_fetch(pos+i));
            }
        }

        if (found_key) {
            if (value_pos) {
                *value_len = i;
                *value_pos = pos;
                return 1;
            }
            deloffset = key_pos - pos;
        }
        on_key = 1;
        found_key = 0;
    }

    if (value) {

        pos += deloffset;

        embedis_eeprom_store(pos-1, key_id_or_len & 0xFF);
        embedis_eeprom_store(pos-2, (key_id_or_len>>8) & 0xFF);
        pos -= 2;

        i = key_id_or_len;
        while (i > 0) {
            i--;
            pos--;
            embedis_eeprom_store(pos, key_name[i]);

        }

        embedis_eeprom_store(pos-1, *value_len & 0xFF);
        embedis_eeprom_store(pos-2, (*value_len>>8) & 0xFF);
        pos -= 2;

        i = *value_len;
        while (i) {
            i--;
            pos--;
            embedis_eeprom_store(pos, value[i]);
        }

        i = 0;
        embedis_eeprom_store(pos-1, 0);
        embedis_eeprom_store(pos-2, 0);

        return 1;

    }

    if (deloffset) {
        embedis_eeprom_store(pos+deloffset-1, 0);
        embedis_eeprom_store(pos+deloffset-2, 0);
        return 1;
    }

    return 0;
}


static short int eeprom_key_to_id(const char* key, size_t length) {
    // Verify we have a zero-terminated key
    size_t i = 0;
    while (key[i]) i++;
    if (length != i) return 0;

    const embedis_dictionary_key* dict = &embedis_dictionary_keys[0];
    while (dict->name) {
        i = 0;
        while (dict->name[i] == key[i]) i++;
        if (i == length+1) {
            // found
            return -dict->id;
        }
        dict++;
    }
    return 0;
}

//static const char* eeprom_id_to_key(short int id) {
//
//}


void embedis_eeprom_SELECT(embedis_state* state) {
    embedis_response_error(EMBEDIS_OK);
}

void embedis_eeprom_KEYS(embedis_state* state) {
    embedis_response_error(0);
}

void embedis_eeprom_GET(embedis_state* state) {
    size_t value_len, value_pos;
    int key_id_or_len, length = state->argv[2] - state->argv[1] - 1;;
    key_id_or_len = eeprom_key_to_id(state->argv[1], length);
    if (!key_id_or_len) key_id_or_len = length;
    if(eeprom_work(state->argv[1], key_id_or_len, 0, &value_len, &value_pos)) {
        embedis_response_string_length(value_len);
        while (value_len) {
            embedis_out(embedis_eeprom_fetch(value_pos));
            value_len--;
            value_pos++;
        }
        embedis_emit_newline();
    } else {
        embedis_response_null();
    }
}

void embedis_eeprom_SET(embedis_state* state) {
    size_t value_len = state->argv[3] - state->argv[2] - 1;
    int key_id_or_len, length = state->argv[2] - state->argv[1] - 1;;
    key_id_or_len = eeprom_key_to_id(state->argv[1], length);
    if (!key_id_or_len) key_id_or_len = length;
    if (eeprom_work(state->argv[1], key_id_or_len, state->argv[2], &value_len, 0)) {
        embedis_response_error(EMBEDIS_OK);
    } else {
        embedis_response_error(0); // out of space
    }
}

void embedis_eeprom_DEL(embedis_state* state) {
    int key_id_or_len, length = state->argv[2] - state->argv[1] - 1;;
    key_id_or_len = eeprom_key_to_id(state->argv[1], length);
    if (!key_id_or_len) key_id_or_len = length;
    eeprom_work(state->argv[1], key_id_or_len, 0, 0, 0);
    embedis_response_error(EMBEDIS_OK);
}
