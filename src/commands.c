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
/**
  ******************************************************************************
  *
  * @file        dict.c
  * @author      Dave Turnbull
  * @version     0.0.1
  * @date        2015-06-29
  * @copyright   PatternAgents, LLC
  * @brief       The Embedis Dictionary
  *
  ******************************************************************************
  */

/// @file

#include "embedis.h"


// Dispatchers


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
    if (state->argv[2] - state->argv[1] == 1) return embedis_response_error(0);
    (*state->dictionary->get)(state);
}


void embedis_SET(embedis_state* state) {
    if (state->argc != 3) {
        return embedis_response_error(EMBEDIS_ARGS_ERROR);
    }
    if (state->argv[2] - state->argv[1] == 1) return embedis_response_error(0);
    (*state->dictionary->set)(state);
}


void embedis_DEL(embedis_state* state) {
    if (state->argc != 2) {
        return embedis_response_error(EMBEDIS_ARGS_ERROR);
    }
    if (state->argv[2] - state->argv[1] == 1) return embedis_response_error(0);
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


static const char* eeprom_id_to_key(short int id) {
    const embedis_dictionary_key* dict = &embedis_dictionary_keys[0];
    while (dict->id) {
        if (dict->id == id) return dict->name;
        dict++;
    }
    return 0;
}


static int eeprom_work(const char* key_name, int key_len, const char* value, size_t* value_len, size_t* value_pos);


// Upon detection of an unformatted or corrupt EEPROM
// we will erase everything and try eeprom_work again.
static int eeprom_reset(const char* key_name, int key_len, const char* value, size_t* value_len, size_t* value_pos) {
    static char reset_lock = 0;
    int rv;
    if (!reset_lock) {
        reset_lock = 1;
        embedis_eeprom_store(embedis_eeprom_size()-1, 0);
        embedis_eeprom_store(embedis_eeprom_size()-2, 0);
        rv = eeprom_work(key_name, key_len, value, value_len, value_pos);
        reset_lock = 0;
        return rv;
    }
    return 0;
}


/*
 Big evil EEPROM function. This does many things.

 Ensure EEPROM is formatted.
 eeprom_work("", 0, 0, 0, 0);

 GET a value for given key. Returns 0 when not found.
 eeprom_work(key_name, key_len, 0, &value_len, &value_pos);

 SET value to the given key. This will replace any existing key.
 Caller is responsible for ensuring there is enough space.
 eeprom_work(key_name, key_len, value, &value_len, 0)

 DEL a key.
 eeprom_work(key_name, key_len, 0, 0, 0);

 Count the number of keys in dictionary and calculate free bytes.
 free_bytes = count = 0;
 while ((eeprom_work(0, 0, 0, 0, &free_bytes))) count++;

 Iterate over all keys.
 pos = 0;
 while ((len_or_id = eeprom_work(0, 0, 0, 0, &pos))) {
   if (len_or_id < 0) .. fetch key_name with eeprom_id_to_key(-len_or_id);
   else ... key_name can be fetched starting at embedis_eeprom_fetch(pos)
 }
 */
static int eeprom_work(const char* key_name, int key_len, const char* value, size_t* value_len, size_t* value_pos) {
    size_t pos = embedis_eeprom_size(), deloffset = 0, key_pos;
    char on_key = 1, found_key = 0;
    int i, key_id_or_len;

    if (!key_name) {
        if (*value_pos) {
            pos = (*value_pos);
            on_key = 0;
        }
    } else {
        key_id_or_len = eeprom_key_to_id(key_name, key_len);
        if (!key_id_or_len) key_id_or_len = key_len;
    }

    while (pos >= 2) {

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

            if ((i < 0 && pos < 2) || (i >= 0 && i  + 2 > pos)) {
                return eeprom_reset(key_name, key_len, value, value_len, value_pos);
            }


            if (i < 0) {
                if (!key_name) {
                    *value_pos = pos;
                    return i;
                }
                if (i == key_id_or_len) {
                    found_key = 1;
                }
                continue;
            }
            pos -= i;

            if (!key_name) {
                *value_pos = pos;
                return i;
            }

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
        if (i + 2 > pos) {
            return eeprom_reset(key_name, key_len, value, value_len, value_pos);
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

    if (!key_name) *value_pos = pos-2;

    return 0;
}


// Ensures EEPROM is formatted and doesn't contain
// keys that have been removed from embedis_dictionary_keys.
static void eeprom_cleanup() {
    size_t pos = 0;
    int len_or_id, is_clean = 0;
    // Ensure formatted
    eeprom_work("", 0, 0, 0, 0);
    // Remove bad keys
    while (!is_clean) {
        is_clean = 1;
        while ((len_or_id = eeprom_work(0, 0, 0, 0, &pos))) {
            if (len_or_id < 0) {
                if (!eeprom_id_to_key(-len_or_id)) {
                    is_clean = 0;
                    eeprom_work("", -len_or_id, 0, 0, 0);
                    break;
                }
            }
        }
    }
}


void embedis_eeprom_SELECT(embedis_state* state) {
    size_t free_bytes = 0;
    char* s = "+OK free = ";
    eeprom_cleanup();

    while (eeprom_work(0, 0, 0, 0, &free_bytes)) {}

    while (*s) embedis_out(*(s++));
    embedis_emit_integer(free_bytes);
    embedis_emit_newline();

    embedis_response_error(EMBEDIS_OK);
}


void embedis_eeprom_KEYS(embedis_state* state) {
    size_t pos;
    int len_or_id, i;

    eeprom_cleanup();

    // Count the keys
    pos = i = 0;
    while ((eeprom_work(0, 0, 0, 0, &pos))) i++;
    embedis_emit_size('*', i);

    pos = 0;
    while ((len_or_id = eeprom_work(0, 0, 0, 0, &pos))) {
        if (len_or_id > 0) {
            embedis_emit_size('$', len_or_id);
            for (i = 0; i < len_or_id; i++) {
                embedis_out(embedis_eeprom_fetch(pos + i));
            }
            embedis_emit_newline();
        } else {
            embedis_response_simple(eeprom_id_to_key(-len_or_id));
        }
    }

}


void embedis_eeprom_GET(embedis_state* state) {
    size_t value_len, value_pos;
    int key_len = state->argv[2] - state->argv[1] - 1;;
    if(eeprom_work(state->argv[1], key_len, 0, &value_len, &value_pos)) {
        embedis_emit_size('$', value_len);
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
    int key_len = state->argv[2] - state->argv[1] - 1;
    if (eeprom_work(state->argv[1], key_len, state->argv[2], &value_len, 0)) {
        embedis_response_error(EMBEDIS_OK);
    } else {
        embedis_response_error(0); // out of space
    }
}


void embedis_eeprom_DEL(embedis_state* state) {
    int key_len = state->argv[2] - state->argv[1] - 1;;
    eeprom_work(state->argv[1], key_len, 0, 0, 0);
    embedis_response_error(EMBEDIS_OK);
}
