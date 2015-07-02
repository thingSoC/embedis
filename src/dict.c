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

static const embedis_dict_config* find_dict_config(int argc, const char* argv[]) {
    const embedis_dict_config* dict = &embedis_dict_keys[0];
    size_t i, length = argv[2] - argv[1] - 1;

    while (dict->name) {
        i = 0;
        while (dict->name[i] == argv[1][i]) i++;
        if (i == length+1) break;
        dict++;
    }
    return dict;
}


void embedis_GET(int argc, const char* argv[]) {
    const embedis_dict_config* dict;
    if (argc != 2) {
        return embedis_response_error(EMBEDIS_ARGS_ERROR);
    }
    dict = find_dict_config(argc, argv);
    (*dict->handle->get)(argc, argv, dict->id);
}


void embedis_SET(int argc, const char* argv[]) {
    const embedis_dict_config* dict;
    if (argc != 3) {
        return embedis_response_error(EMBEDIS_ARGS_ERROR);
    }
    dict = find_dict_config(argc, argv);
    (*dict->handle->set)(argc, argv, dict->id);
}


void embedis_DEL(int argc, const char* argv[]) {
    const embedis_dict_config* dict;
    if (argc != 2) {
        return embedis_response_error(EMBEDIS_ARGS_ERROR);
    }
    dict = find_dict_config(argc, argv);
    (*dict->handle->del)(argc, argv, dict->id);
}


void embedis_dict_error(int argc, const char* argv[], const void* id) {
    embedis_response_error(0);
}


void embedis_dict_rom(int argc, const char* argv[], const void* id) {
    const char* value = id;
    embedis_response_simple(value);
}

/*  NVRAM key-value data is stored starting at the last location.
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


static void dict_reset() {
    embedis_nvram_store(embedis_nvram_size()-1, 0);
    embedis_nvram_store(embedis_nvram_size()-2, 0);
}

// get if value_pos != 0
// set if value != 0
// delete if value == 0
// returns 1 on success
static int dict(const char* key_name, int key_id_or_len, const char* value, size_t* value_len, size_t* value_pos) {

    size_t pos = embedis_nvram_size(), deloffset = 0, key_pos;
    int i;
    char on_key = 1, found_key = 0;

    while (pos > 2) {

        if (on_key) {
            key_pos = pos;
            i = embedis_nvram_fetch(pos-1) + (embedis_nvram_fetch(pos-2) << 8);
            if (i==0) break; // end
            pos -= 2;

            if (deloffset) {
                embedis_nvram_store(pos+deloffset+1, embedis_nvram_fetch(pos+1));
                embedis_nvram_store(pos+deloffset, embedis_nvram_fetch(pos));
            }

            on_key = 0;

            if (i < 0) {
                if (i == key_id_or_len) found_key = 1;
                continue;
            }
            if (i >= pos) {
                // ERROR
                dict_reset();
                return 0;
            }
            pos -= i;

            if (deloffset) {
                while (i) {
                    i--;
                    embedis_nvram_store(pos+i+deloffset, embedis_nvram_fetch(pos+i));
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
                if (embedis_nvram_fetch(pos+i) != key_name[i]) {
                    found_key = 0;
                    break;
                }
            }
            continue;
        }

        i = embedis_nvram_fetch(pos-1) + (embedis_nvram_fetch(pos-2) << 8);
        pos -= 2;
        if (i >= pos) {
            // ERROR
            dict_reset();
            return 0;
        }
        pos -= i;

        if (deloffset) {
            i += 2;
            while (i) {
                i--;
                embedis_nvram_store(pos+i+deloffset, embedis_nvram_fetch(pos+i));
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
        //TODO check for enough room

        pos += deloffset;

        embedis_nvram_store(pos-1, key_id_or_len & 0xFF);
        embedis_nvram_store(pos-2, (key_id_or_len>>8) & 0xFF);
        pos -= 2;

        i = key_id_or_len;
        while (i > 0) {
            i--;
            pos--;
            embedis_nvram_store(pos, key_name[i]);

        }

        embedis_nvram_store(pos-1, *value_len & 0xFF);
        embedis_nvram_store(pos-2, (*value_len>>8) & 0xFF);
        pos -= 2;

        i = *value_len;
        while (i) {
            i--;
            pos--;
            embedis_nvram_store(pos, value[i]);
        }

        i = 0;
        embedis_nvram_store(pos-1, 0);
        embedis_nvram_store(pos-2, 0);

        return 1;

    }

    if (deloffset) {
        embedis_nvram_store(pos+deloffset-1, 0);
        embedis_nvram_store(pos+deloffset-2, 0);
        return 1;
    }

    return 0;
}


void embedis_dict_GET(int argc, const char* argv[], const void* id) {
    size_t value_len, value_pos;
    int key_id_or_len = (int)id;

    if (key_id_or_len) {
        key_id_or_len = -key_id_or_len;
    } else {
        key_id_or_len = argv[2] - argv[1] - 1;
    }

    if(dict(argv[1], key_id_or_len, 0, &value_len, &value_pos)) {
        embedis_response_string_length(value_len);
        while (value_len) {
            embedis_out(embedis_nvram_fetch(value_pos));
            value_len--;
            value_pos++;
        }
        embedis_response_newline();
    } else {
        embedis_response_null();
    }
}


void embedis_dict_SET(int argc, const char* argv[], const void* id) {
    size_t value_len = argv[3] - argv[2] - 1;
    int key_id_or_len = (int)id;

    if (key_id_or_len) {
        key_id_or_len = -key_id_or_len;
    } else {
        key_id_or_len = argv[2] - argv[1] - 1;
    }

    if (dict(argv[1], key_id_or_len, argv[2], &value_len, 0)) {
        embedis_response_error(EMBEDIS_OK);
    } else {
        embedis_response_error(0); // out of space
    }
}


void embedis_dict_DEL(int argc, const char* argv[], const void* id) {
    int key_id_or_len = (int)id;
    if (key_id_or_len) {
        key_id_or_len = -key_id_or_len;
    } else {
        key_id_or_len = argv[2] - argv[1] - 1;
    }
    dict(argv[1], key_id_or_len, 0, 0, 0);
    embedis_response_error(EMBEDIS_OK);
}
