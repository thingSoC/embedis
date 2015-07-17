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


// Primary Dispatchers


void embedis_SELECT(embedis_state* state) {
    if (state->argc < 2) {
        return embedis_response_error(EMBEDIS_ARGS_ERROR);
    }
    const embedis_dictionary* dict = &embedis_dictionaries[0];
    while (dict->name) {
        if (!embedis_stricmp(dict->name, state->argv[1])) {
            state->dictionary = dict;
            (*dict->commands->select)(state);
            return;
        }
        dict++;
    }
    embedis_response_error(0);
}


void embedis_KEYS(embedis_state* state) {
    (*state->dictionary->commands->keys)(state);
}


void embedis_GET(embedis_state* state) {
    if (state->argc != 2) {
        return embedis_response_error(EMBEDIS_ARGS_ERROR);
    }
    if (state->argv[2] - state->argv[1] == 1) return embedis_response_error(0);
    (*state->dictionary->commands->get)(state);
}


void embedis_SET(embedis_state* state) {
    if (state->argc != 3) {
        return embedis_response_error(EMBEDIS_ARGS_ERROR);
    }
    if (state->argv[2] - state->argv[1] == 1) return embedis_response_error(0);
    (*state->dictionary->commands->set)(state);
}


void embedis_DEL(embedis_state* state) {
    if (state->argc != 2) {
        return embedis_response_error(EMBEDIS_ARGS_ERROR);
    }
    if (state->argv[2] - state->argv[1] == 1) return embedis_response_error(0);
    (*state->dictionary->commands->del)(state);
}

// Helpers to translate key strings with their integer values

static short int dictionary_key_to_id(const char* key, size_t length) {
    // Verify we have a strcmp-comparable key
    int i;
    for (i = 0; i < length; i++) {
        if (!key[i]) return 0;
    }
    if (key[i]) return 0;

    const embedis_dictionary_key* dict = &embedis_dictionary_keys[0];
    while (dict->name) {

        if (!embedis_strcmp(dict->name, key)) {
            return -dict->id;
        }
        dict++;
    }
    return 0;
}


static const char* dictionary_id_to_key(short int id) {
    const embedis_dictionary_key* dict = &embedis_dictionary_keys[0];
    while (dict->id) {
        if (dict->id == id) return dict->name;
        dict++;
    }
    return 0;
}

// ROM dictionary

const embedis_dictionary_commands embedis_rom_commands = {
    embedis_rom_SELECT,
    embedis_rom_KEYS,
    embedis_rom_GET,
    embedis_rom_SET,
    embedis_rom_DEL
};


void embedis_rom_SELECT(embedis_state* state) {
    embedis_response_error(EMBEDIS_OK);
}


void embedis_rom_KEYS(embedis_state* state) {
    embedis_response_error(0);
}


void embedis_rom_GET(embedis_state* state) {
    char* const* rom = state->dictionary->context;
    while (*rom) {
        if (!embedis_strcmp(rom[0], state->argv[1])) {
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
 RAM key-value data is stored starting at the last location.
 This allows it to, for example, share an RAM with another
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



static int ram_work(embedis_ram_access* access, const char* key_name, int key_len, const char* value, size_t* value_len, size_t* value_pos);


// Upon detection of an unformatted or corrupt RAM
// we will erase everything and try ram_work again.
static int ram_reset(embedis_ram_access* access, const char* key_name, int key_len, const char* value, size_t* value_len, size_t* value_pos) {
    static char reset_lock = 0;
    int rv;
    if (!reset_lock) {
        reset_lock = 1;
        (*access->store)((*access->size)()-1, 0);
        (*access->store)((*access->size)()-2, 0);
        rv = ram_work(access, key_name, key_len, value, value_len, value_pos);
        reset_lock = 0;
        return rv;
    }
    return 0;
}


/*
 Big evil RAM function. This does many things.

 Ensure RAM is formatted.
 ram_work("", 0, 0, 0, 0);

 GET a value for given key. Returns 0 when not found.
 ram_work(key_name, key_len, 0, &value_len, &value_pos);

 SET value to the given key. This will replace any existing key.
 Caller is responsible for ensuring there is enough space.
 ram_work(key_name, key_len, value, &value_len, 0)

 DEL a key.
 ram_work(key_name, key_len, 0, 0, 0);

 Count the number of keys in dictionary and calculate free bytes.
 free_bytes = count = 0;
 while ((ram_work(0, 0, 0, 0, &free_bytes))) count++;

 Iterate over all keys.
 pos = 0;
 while ((len_or_id = ram_work(0, 0, 0, 0, &pos))) {
   if (len_or_id < 0) .. fetch key_name with dictionary_id_to_key(-len_or_id);
   else ... key_name can be fetched starting at embedis_ram_fetch(pos)
 }
 */
static int ram_work(embedis_ram_access* access, const char* key_name, int key_len, const char* value, size_t* value_len, size_t* value_pos) {
    size_t pos = (*access->size)(), deloffset = 0, key_pos = 0;
    char on_key = 1, found_key = 0;
    int i, key_id_or_len = 0;

    if (!key_name) {
        if (*value_pos) {
            pos = (*value_pos);
            on_key = 0;
        }
    } else {
        key_id_or_len = dictionary_key_to_id(key_name, key_len);
        if (!key_id_or_len) key_id_or_len = key_len;
    }

    while (pos >= 2) {

        if (on_key) {
            key_pos = pos;
            i = (*access->fetch)(pos-1) + ((*access->fetch)(pos-2) << 8);
            if (i==0) break; // end
            pos -= 2;

            if (deloffset) {
                (*access->store)(pos+deloffset+1, (*access->fetch)(pos+1));
                (*access->store)(pos+deloffset, (*access->fetch)(pos));
            }

            on_key = 0;

            if ((i < 0 && pos < 2) || (i >= 0 && i  + 2 > pos)) {
                return ram_reset(access, key_name, key_len, value, value_len, value_pos);
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
                    (*access->store)(pos+i+deloffset, (*access->fetch)(pos+i));
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
                if ((*access->fetch)(pos+i) != key_name[i]) {
                    found_key = 0;
                    break;
                }
            }
            continue;
        }

        i = (*access->fetch)(pos-1) + ((*access->fetch)(pos-2) << 8);
        pos -= 2;
        if (i + 2 > pos) {
            return ram_reset(access, key_name, key_len, value, value_len, value_pos);
        }
        pos -= i;

        if (deloffset) {
            i += 2;
            while (i) {
                i--;
                (*access->store)(pos+i+deloffset, (*access->fetch)(pos+i));
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

        (*access->store)(pos-1, key_id_or_len & 0xFF);
        (*access->store)(pos-2, (key_id_or_len>>8) & 0xFF);
        pos -= 2;

        i = key_id_or_len;
        while (i > 0) {
            i--;
            pos--;
            (*access->store)(pos, key_name[i]);
        }

        (*access->store)(pos-1, *value_len & 0xFF);
        (*access->store)(pos-2, (*value_len>>8) & 0xFF);
        pos -= 2;

        i = *value_len;
        while (i) {
            i--;
            pos--;
            (*access->store)(pos, value[i]);
        }

        i = 0;
        (*access->store)(pos-1, 0);
        (*access->store)(pos-2, 0);

        return 1;

    }

    if (deloffset) {
        (*access->store)(pos+deloffset-1, 0);
        (*access->store)(pos+deloffset-2, 0);
        return 1;
    }

    if (!key_name) *value_pos = pos-2;

    return 0;
}


// Ensures RAM is formatted and doesn't contain
// keys that have been removed from embedis_dictionary_keys.
static void ram_cleanup(embedis_ram_access* access) {
    size_t pos = 0;
    int len_or_id, is_clean = 0;
    // Ensure formatted
    ram_work(access, "", 0, 0, 0, 0);
    // Remove bad keys
    while (!is_clean) {
        is_clean = 1;
        while ((len_or_id = ram_work(access, 0, 0, 0, 0, &pos))) {
            if (len_or_id < 0) {
                if (!dictionary_id_to_key(-len_or_id)) {
                    is_clean = 0;
                    ram_work(access, "", -len_or_id, 0, 0, 0);
                    break;
                }
            }
        }
    }
}


const embedis_dictionary_commands embedis_ram_commands = {
    embedis_ram_SELECT,
    embedis_ram_KEYS,
    embedis_ram_GET,
    embedis_ram_SET,
    embedis_ram_DEL
};


void embedis_ram_SELECT(embedis_state* state) {
    embedis_ram_access* access = (embedis_ram_access*)state->dictionary->context;
    size_t free_bytes = 0;
    char* s = "+OK free = ";
    ram_cleanup(access);

    while (ram_work(access, 0, 0, 0, 0, &free_bytes)) {}

    while (*s) embedis_out(*(s++));
    embedis_emit_integer(free_bytes);
    embedis_emit_newline();
}


void embedis_ram_KEYS(embedis_state* state) {
    embedis_ram_access* access = (embedis_ram_access*)state->dictionary->context;
    size_t pos;
    int len_or_id, i;

    ram_cleanup(access);

    // Count the keys
    pos = i = 0;
    while ((ram_work(access, 0, 0, 0, 0, &pos))) i++;
    embedis_emit_size('*', i);

    pos = 0;
    while ((len_or_id = ram_work(access, 0, 0, 0, 0, &pos))) {
        if (len_or_id > 0) {
            embedis_emit_size('$', len_or_id);
            for (i = 0; i < len_or_id; i++) {
                embedis_out((*access->fetch)(pos + i));
            }
            embedis_emit_newline();
        } else {
            embedis_response_simple(dictionary_id_to_key(-len_or_id));
        }
    }

}


void embedis_ram_GET(embedis_state* state) {
    embedis_ram_access* access = (embedis_ram_access*)state->dictionary->context;
    size_t value_len, value_pos;
    int key_len = state->argv[2] - state->argv[1] - 1;;
    if(ram_work(access, state->argv[1], key_len, 0, &value_len, &value_pos)) {
        embedis_emit_size('$', value_len);
        while (value_len) {
            embedis_out((*access->fetch)(value_pos));
            value_len--;
            value_pos++;
        }
        embedis_emit_newline();
    } else {
        embedis_response_null();
    }
}


void embedis_ram_SET(embedis_state* state) {
    embedis_ram_access* access = (embedis_ram_access*)state->dictionary->context;
    size_t value_len, value_pos;
    int is_found, key_len = state->argv[2] - state->argv[1] - 1;

    // Compute free space
    value_pos = 0;
    while ((ram_work(access, 0, 0, 0, 0, &value_pos))) {}
    int free_bytes = value_pos;

    is_found = ram_work(access, state->argv[1], key_len, 0, &value_len, &value_pos);

    // If a key exists, it's value space is available
    // Else account for key and value size storage
    if (is_found) free_bytes += value_len;
    else free_bytes -= 4;

    // If key is stored-type and doesn't exists, account for its size
    if (!dictionary_key_to_id(state->argv[1], key_len)) {
        if (!is_found) free_bytes -= key_len;
    }

    value_len = state->argv[3] - state->argv[2] - 1;
    if (free_bytes < value_len) {
        return embedis_response_error(0);
    }

    ram_work(access, state->argv[1], key_len, state->argv[2], &value_len, 0);
    embedis_response_error(EMBEDIS_OK);
}


void embedis_ram_DEL(embedis_state* state) {
    embedis_ram_access* access = (embedis_ram_access*)state->dictionary->context;
    int key_len = state->argv[2] - state->argv[1] - 1;;
    ram_work(access, state->argv[1], key_len, 0, 0, 0);
    embedis_response_error(EMBEDIS_OK);
}


// Hardware READ/WRITE dispatchers

static const embedis_rw_key* find_rw_key(embedis_state* state) {
    const embedis_rw_key* key = &embedis_rw_keys[0];
    while(key->name) {
        if (!embedis_strcmp(key->name, state->argv[1])) break;
        key++;
    }
    return key;
}

void embedis_READ(embedis_state* state) {
    const embedis_rw_key* key = find_rw_key(state);
    key->read(state);
}

void embedis_WRITE(embedis_state* state) {
    const embedis_rw_key* key = find_rw_key(state);
    key->write(state);
}
