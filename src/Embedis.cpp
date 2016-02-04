/*  Embedis - Embedded Dictionary Server
    Copyright (C) 2016 PatternAgents, LLC

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

#include "Embedis.h"


Embedis::TVec<Embedis::Command> Embedis::commands;
Embedis::TVec<Embedis::Channel> Embedis::channels;
Embedis::TVec<Embedis::Dictionary> Embedis::dictionaries;
Embedis::TVec<Embedis::Key> Embedis::keys;
Embedis::TVec<Embedis::Hardware> Embedis::hardwares;


// This is too slow in Arduino String. It would copy s2. Doh.
static int stricmp(const String& s, const char* s2) {
    int rc, us1, us2;
    const char* s1 = s.c_str();
    while (1) {
        if (!*s1 && !*s2) return 0;
        us1 = *s1;
        us2 = *s2;
        if (us1 >= 'a' && us1 <= 'z') us1 -= 32;
        if (us2 >= 'a' && us2 <= 'z') us2 -= 32;
        rc = us1 - us2;
        if (rc) return rc;
        s1++;
        s2++;
    }
}


// Wrestle Arduino into making strings that may contain zeros.
static String binstring(const char* str, size_t len) {
    String s;
    s.reserve(len);
    for (size_t i = 0; i < len; i++) s += str[i];
    return s;
}


Embedis::Embedis(Stream& stream, size_t buflen, size_t argvlen) :
    argvlen(argvlen),
    buflen(buflen),
    buf((char*)malloc(buflen * sizeof(char))),
    subcount(0),
    current_dict(0),
    stream(&stream),
    argv((char**)malloc((argvlen+1) * sizeof(char*)))
{
    reset();
}


Embedis::~Embedis()
{
    reset(true); // for unsubscribe
    free(buf);
    free(argv);
}


void Embedis::reset(bool everything)
{
    pos = 0;
    mode = 0;
    argc = 0;
    argv[0] = &buf[0];
    mode_args = 1;

    if (everything) {
        current_dict = 0;
        subcount = 0;
        for (size_t i = channels.size(); i > 0 ;) {
            --i;
            TVec<Embedis*>* subs = &channels[i].subs;
            for (size_t j = 0; j < subs->size(); ++j) {
                if ((*subs)[j] == this) {
                    subs->remove(j);
                    break;
                }
            }
            if (subs->size() == 0) channels.remove(i);
        }
    }
}


void Embedis::process()
{

    int i;
    while ((i = stream->read()) >= 0) {
        char c = i;

        // \0   start of command
        // ' '  human friendly basic collection
        //  "   human friendly collecting inside quotes
        //  .   human friendly found second quote, deciding if closing
        //  $   binary determining string length
        //  *   binary determining args count
        // +-:  binary collecting \r terminated
        //  ?   binary collecting known length
        // \r   binary expecting required \r
        // \n   binary expecting required \n

        if (mode == 0 || mode == '?') {
            switch (c) {
                case '+':
                case '-':
                case ':':
                    mode_chars = 0;
                    mode = '+';
                    continue;
                case '*':
                    if (mode != 0) {
                        break;
                    }
                    mode_chars = -1;
                    mode_args = 0;
                    mode = c;
                    continue;
                case '$':
                    mode_chars = 0;
                    mode = c;
                    continue;
            }
            if (mode != 0) {
                reset();
                response(SYNTAX_ERROR);
                continue;
            }
        }

        if (mode == 0) {
            switch (c) {
            case '\r':
            case '\n':
                // nop, throw these away
                continue;
            default:
                if (pos != 0) {
                    reset();
                    response(SYNTAX_ERROR);
                    continue;
                }
                mode = ' ';
                break;
            }
        }

        if (mode == '+') {
            if (c == '\r') {
                mode = '\n';
                if (pos < buflen) {
                    buf[pos] = 0;
                    pos++;
                }
                continue;
            }
            if (pos < buflen) {
                buf[pos] = c;
                pos++;
            }
            continue;
        }

        if (mode == 'b') {
            if (pos < buflen) {
                buf[pos] = c;
                pos++;
            }
            mode_chars -= 1;
            if (mode_chars > 0) {
                continue;
            }
            mode = '\r';
            if (pos < buflen) {
                buf[pos] = 0;
                pos++;
            }
            continue;
        }

        if (mode == '\r') {
            if (c != '\r') {
                reset();
                response(SYNTAX_ERROR);
                continue;
            }
            mode = '\n';
            continue;
        }

        if (mode == '\n') {
            if (c != '\n') {
                reset();
                response(SYNTAX_ERROR);
                continue;
            }
            if (mode_chars>0) {
                mode = 'b';
                continue;
            }
            if (mode_chars==0) {
                argc += 1;
                if (argc <= argvlen) {
                    argv[argc] = &buf[pos];
                }
                mode_args -= 1;
                if (mode_args == 0) {
                    if (pos >= buflen) {
                        response(BUFFER_OVERFLOW);
                        reset();
                        continue;
                    }
                    dispatch();
                    reset();
                    continue;
                }
            }
            mode = '?';
            continue;
        }

        if (mode == '*') {
            if (c == '\r') {
                mode = '\n';
                continue;
            }
            if (c < '0' || c > '9') {
                reset();
                response(SYNTAX_ERROR);
                continue;
            }
            mode_args = mode_args * 10 + c - '0';
            continue;
        }

        if (mode == '$') {
            if (c == '\r') {
                mode = '\n';
                continue;
            }
            if (c=='-' && mode_chars==0) {
                mode_chars = -1;
                continue;
            }
            if (c < '0' || c > '9') {
                reset();
                response(SYNTAX_ERROR);
                continue;
            }
            if (mode_chars != -1) {
                mode_chars = mode_chars * 10 + c - '0';
                continue;
            }
        }

        if (mode == ' ' || mode == '"' || mode == '.') {
            if (c == '\r' || c == '\n') {
                if (!pos) {
                    reset();
                    continue;
                }
                // Deal with trailing spaces
                if (mode == '.' || argv[argc] != &buf[pos]) {
                    argc++;
                } else {
                    pos--;
                }
                // Check for overflows
                if (argc > argvlen) {
                    response(ARGS_ERROR);
                    reset();
                    continue;
                }
                if (pos >= buflen) {
                    response(BUFFER_OVERFLOW);
                    reset();
                    continue;
                }
                buf[pos] = 0;
                pos++;
                argv[argc] = &buf[pos];
                dispatch();
                reset();
                return; // no continue after dispatch
            }

            if (c == '"') {
                if (mode == ' ' && argv[argc] == &buf[pos]) {
                    mode = '"';
                    continue;
                } else if (mode == '"') {
                    mode = '.';
                    continue;
                }
            }

            if (c == ' ' && mode != '"') {
                if (mode != '.' && argv[argc] == &buf[pos]) {
                    // drop extra spaces
                    continue;
                }
                mode = ' ';
                if (pos < buflen) {
                    buf[pos] = 0;
                    pos++;
                }
                argc++;
                if (argc <= argvlen) {
                    argv[argc] = &buf[pos];
                }
                continue;
            }

            if (mode == '.') {
                mode = '"';
                if (pos < buflen) {
                    buf[pos] = '"';
                    pos++;
                }
            }

            if (pos < buflen) {
                buf[pos] = c;
                pos++;
            }

            continue;
        }
    }
}


void Embedis::response(Status s)
{
    if (s == OK) {
        stream->println(F("+OK"));
        return;
    }
    stream->print(F("-ERROR "));
    switch (s) {
    case UNKNOWN_COMMAND:
        stream->print(F("unknown command"));
        break;
    case SYNTAX_ERROR:
        stream->print(F("syntax error"));
        break;
    case BUFFER_OVERFLOW:
        stream->print(F("buffer overflow"));
        break;
    case ARGS_ERROR:
        stream->print(F("bad argument count"));
        break;
    case NOT_FOUND:
        stream->print(F("not found"));
        break;
    default: /* ERROR of non-specific type */
        break;
    }
    stream->println();
}


void Embedis::response(char c, int i)
{
    stream->print(c);
    stream->println(i);
}


void Embedis::response(const String& s)
{
    bool useSimple = true;
    for (size_t i = 0; i < s.length(); ++i) {
        if (s[i] < 32 || s[i] > 126) {
            useSimple = false;
            break;
        }
    }
    if (s.length() == 0) useSimple = false;
    if (useSimple) {
        stream->print('+');
    } else {
        response('$', s.length());
    }
    stream->println(s);
}


void Embedis::response(const __FlashStringHelper *ifsh)
{
    bool useSimple = true;
    size_t length = 0;
    PGM_P p = reinterpret_cast<PGM_P>(ifsh);
    while (1) {
        unsigned char c = pgm_read_byte(p++);
        if (c == 0) break;
        length++;
        if (c < 32 || c > 126) useSimple = false;
    }
    if (length == 0) useSimple = false;
    if (useSimple) {
        stream->print('+');
    } else {
        response('$', length);
    }
    stream->println(ifsh);
}


void Embedis::command(const String& name, void (*call)(Embedis*))
{
    add_basic();
    size_t pos;
    for (pos = 0; pos < commands.size(); ++pos) {
        if (commands[pos].name.equalsIgnoreCase(name)) break;
    }
    if (pos == commands.size()) {
        commands.resize(commands.size()+1);
        commands[pos].name = name;
    }
    commands[pos].call = call;
}


void Embedis::hardware(const String& name, void (*read)(Embedis*), void (*write)(Embedis*))
{
    add_readwrite();
    size_t pos;
    for (pos = 0; pos < hardwares.size(); ++pos) {
        if (hardwares[pos].name == name) break;
    }
    if (pos == hardwares.size()) {
        hardwares.resize(hardwares.size()+1);
        hardwares[pos].name = name;
    }
    hardwares[pos].read = read;
    hardwares[pos].write = write;
}


void Embedis::dictionary(const String& name,
                         bool (*get)(const String& key, String& value),
                         bool (*set)(const String& key, const String& value),
                         bool (*del)(const String& key),
                         void (*keys)(Embedis*),
                         void (*select)(Embedis*))
{
    if (!get) return;
    add_dict();
    size_t dict = find_dictionary(name, true);
    dictionaries[dict].get = get;
    dictionaries[dict].set = set;
    dictionaries[dict].del = del;
    dictionaries[dict].keys = keys;
    dictionaries[dict].select = select;
}


void Embedis::dictionary(const String& name,
                         size_t size,
                         char (*fetch)(size_t pos),
                         void (*store)(size_t pos, char value),
                         void (*commit)())
{
    if (!size || !fetch || !store) return;
    add_dict();
    size_t dict = find_dictionary(name, true);
    dictionaries[dict].custom = 0;
    dictionaries[dict].size = size;
    dictionaries[dict].fetch = fetch;
    dictionaries[dict].store = store;
    dictionaries[dict].commit = commit;
}


void Embedis::key(const __FlashStringHelper* name, short int id)
{
    PGM_P pname = reinterpret_cast<PGM_P>(name);
    size_t pos;
    if (id <= 0) return;
    for (pos = 0; pos < keys.size(); ++pos) {
        // Does gcc consolidate PGM_P literals?
        if (keys[pos].name == pname) return;
        if (keys[pos].id == id) return;
    }
    keys.resize(pos+1);
    keys[pos].name = pname;
    keys[pos].id = id;
}


size_t Embedis::publish(const String& channel, const String& message)
{
    size_t qty = 0;
    for (size_t chan = 0; chan < channels.size(); ++chan) {
        if (channels[chan].name == channel) {
            TVec<Embedis*>* subs = &channels[chan].subs;
            for (size_t j = 0; j < subs->size(); ++j) {
                (*subs)[j]->response('*', 3);
                (*subs)[j]->response(F("message"));
                (*subs)[j]->response(channel);
                (*subs)[j]->response(message);
                ++qty;
            }
            break;
        }
    }
    return qty;
}


bool Embedis::get(const String& key, String& value) {
    if (!dictionaries.size()) return false;
    return get(dictionaries[0].name, key, value);
}


bool Embedis::get(const String& dict, const String& key, String& value)
{
    size_t dnum = find_dictionary(dict);
    if (dnum >= dictionaries.size()) return false;
    Dictionary* dptr = &dictionaries[dnum];
    if (dptr->custom) {
        return dptr->get(key, value);
    } else {
        return kvs_get(dptr, key.c_str(), key.length(),  value);
    }
}


bool Embedis::set(const String& key, const String& value) {
    if (!dictionaries.size()) return false;
    return set(dictionaries[0].name, key, value);
}


bool Embedis::set(const String& dict, const String& key, const String& value)
{
    size_t dnum = find_dictionary(dict);
    if (dnum >= dictionaries.size()) return false;
    Dictionary* dptr = &dictionaries[dnum];
    kvs_work(dptr, "", 0, 0, 0, 0); // ensure formatted
    if (dptr->custom) {
        if (!dptr->set) return false;
        return dptr->set(key, value);
    } else {
        return kvs_set(dptr, key.c_str(), key.length(),  value.c_str(), value.length());
    }
}


bool Embedis::del(const String& key) {
    if (!dictionaries.size()) return false;
    return del(dictionaries[0].name, key);
}


bool Embedis::del(const String& dict, const String& key)
{
    size_t dnum = find_dictionary(dict);
    if (dnum >= dictionaries.size()) return false;
    Dictionary* dptr = &dictionaries[dnum];
    if (dptr->custom) {
        if (!dptr->del) return false;
        return dptr->del(key);
    } else {
        return kvs_del(dptr, key.c_str(), key.length());
    }
}


void Embedis::dispatch()
{
    add_basic();
    for (size_t pos = 0; pos < commands.size(); ++pos) {
        if (!stricmp(commands[pos].name, argv[0])) {
            commands[pos].call(this);
            return;
        }
    }
    response(UNKNOWN_COMMAND);
}


size_t Embedis::find_dictionary(const String& name, bool allow_create)
{
    size_t pos;
    for (pos = 0; pos < dictionaries.size(); ++pos) {
        if (dictionaries[pos].name.equalsIgnoreCase(name)) break;
    }
    if (allow_create && pos == dictionaries.size()) {
        dictionaries.resize(dictionaries.size()+1);
        dictionaries[pos].name = name;
    }
    return pos;
}


void Embedis::add_basic()
{
    static bool added = false;
    if (added) return;
    added = true;
    command(F("COMMANDS"), cmd_COMMANDS);
    command(F("PUBLISH"), cmd_PUBLISH);
    command(F("SUBSCRIBE"), cmd_SUBSCRIBE);
    command(F("UNSUBSCRIBE"), cmd_UNSUBSCRIBE);
}


void Embedis::add_readwrite()
{
    static bool added = false;
    if (added) return;
    added = true;
    add_basic();
    command(F("HARDWARE"), cmd_HARDWARE);
    command(F("READ"), cmd_READ);
    command(F("WRITE"), cmd_WRITE);
}


void Embedis::add_dict()
{
    static bool added = false;
    if (added) return;
    added = true;
    add_basic();
    command(F("DICTIONARIES"), cmd_DICTIONARIES);
    command(F("GET"), cmd_GET);
    command(F("SET"), cmd_SET);
    command(F("DEL"), cmd_DEL);
    command(F("KEYS"), cmd_KEYS);
    command(F("SELECT"), cmd_SELECT);
}


void Embedis::cmd_COMMANDS(Embedis* e)
{
    e->response('*', commands.size());
    for (size_t i = 0; i < commands.size(); ++i) {
        e->response(commands[i].name);
    }
}


void Embedis::cmd_PUBLISH(Embedis* e)
{
    if (e->argc != 3) return e->response(ARGS_ERROR);
    e->response(':', publish(e->argv[1], e->argv[2]));
}


void Embedis::cmd_SUBSCRIBE(Embedis* e)
{
    if (e->argc != 2) return e->response(ARGS_ERROR);
    bool subbed = false;
    size_t chan;
    for (chan = 0; chan < channels.size(); ++chan) {
        if (channels[chan].name == e->argv[1]) {
            TVec<Embedis*>* subs = &channels[chan].subs;
            for (size_t j = 0; j < subs->size(); ++j) {
                if ((*subs)[j] == e) {
                    subbed = true;
                    break;
                }
            }
            break;
        }
    }
    if (chan == channels.size()) {
        channels.resize(channels.size()+1);
        channels[chan].name = e->argv[1];
    }
    if (!subbed) {
        size_t siz = channels[chan].subs.size();
        channels[chan].subs.resize(siz+1);
        channels[chan].subs[siz] = e;
        ++e->subcount;
    }
    e->response('*', 3);
    e->response(F("subscribe"));
    e->response(channels[chan].name);
    e->response(':', e->subcount);
}


void Embedis::cmd_UNSUBSCRIBE(Embedis* e)
{
    if (e->argc > 2) return e->response(ARGS_ERROR);
    for (size_t i = channels.size(); i > 0 ;) {
        --i;
        TVec<Embedis*>* subs = &channels[i].subs;
        bool chan_match = false;
        if (e->argc == 2) chan_match = channels[i].name == e->argv[1];
        for (size_t j = 0; j < subs->size(); ++j) {
            if ((*subs)[j] == e) {
                if (e->argc == 1 || chan_match) {
                    --e->subcount;
                    subs->remove(j);
                    e->response('*', 3);
                    e->response(F("unsubscribe"));
                    e->response(channels[i].name);
                    e->response(':', e->subcount);
                }
                break;
            }
        }
        if (subs->size() == 0) channels.remove(i);
        if (chan_match) break;
    }
}


void Embedis::cmd_HARDWARE(Embedis* e)
{
    e->response('*', hardwares.size());
    for (size_t i = 0; i < hardwares.size(); ++i) {
        String s;
        if (hardwares[i].read && hardwares[i].write) s = "RW: ";
        else if (hardwares[i].read) s = "RO: ";
        else if (hardwares[i].write) s = "WO: ";
        e->response(s + hardwares[i].name);
    }
}


void Embedis::cmd_READ(Embedis* e)
{
    if (e->argc < 2) return e->response(ARGS_ERROR);
    for (size_t i = 0; i < hardwares.size(); ++i) {
        if (hardwares[i].name == e->argv[1]) {
            if (hardwares[i].read) return hardwares[i].read(e);
        }
    }
    e->response(ERROR);
}


void Embedis::cmd_WRITE(Embedis* e)
{
    if (e->argc < 2) return e->response(ARGS_ERROR);
    for (size_t i = 0; i < hardwares.size(); ++i) {
        if (hardwares[i].name == e->argv[1]) {
            if (hardwares[i].write) return hardwares[i].write(e);
        }
    }
    e->response(ERROR);
}


void Embedis::cmd_DICTIONARIES(Embedis* e)
{
    e->response('*', dictionaries.size());
    for (size_t i = 0; i < dictionaries.size(); ++i) {
        e->response(dictionaries[i].name);
    }
}

void Embedis::cmd_GET(Embedis* e)
{
    if (e->argc != 2) return e->response(ARGS_ERROR);
    Dictionary* dict = &dictionaries[e->current_dict];
    String value;
    int key_len = e->argv[2] - e->argv[1] - 1;
    if (dict->custom) {
        if (dict->get(binstring(e->argv[1], key_len), value)) {
            return e->response(value);
        }
    } else {
        if (kvs_get(dict, e->argv[1], key_len, value)) {
            return e->response(value);
        }
    }
    e->response('$', -1);
}


void Embedis::cmd_SET(Embedis* e)
{
    if (e->argc != 3) return e->response(ARGS_ERROR);

    Dictionary* dict = &dictionaries[e->current_dict];
    String value;
    int key_len = e->argv[2] - e->argv[1] - 1;
    int value_len = e->argv[3] - e->argv[2] - 1;

    if (dict->custom) {
        if (dict->set) {
            if (dict->set(binstring(e->argv[1], key_len), binstring(e->argv[2], value_len))) {
                return e->response(OK);
            }
        }
    } else {
        if (kvs_set(dict, e->argv[1], key_len, e->argv[2], value_len)) {
            return e->response(OK);
        }
    }
    e->response(ERROR);
}


void Embedis::cmd_DEL(Embedis* e)
{
    if (e->argc != 2) return e->response(ARGS_ERROR);

    Dictionary* dict = &dictionaries[e->current_dict];
    bool deleted = false;
    int key_len = e->argv[2] - e->argv[1] - 1;

    if (dict->custom) {
        if (dict->del) {
            deleted = dict->del(binstring(e->argv[1], key_len));
        }
    } else {
        deleted = kvs_del(dict, e->argv[1], key_len);
    }
    if (deleted) {
        e->response(':', 1);
    } else {
        e->response(':', 0);
    }
}


void Embedis::cmd_KEYS(Embedis* e)
{
    if (e->argc != 1) return e->response(ARGS_ERROR);

    Dictionary* dict = &dictionaries[e->current_dict];
    if (dict->custom) {
        if (dict->keys) return dict->keys(e);
    } else {
        return kvs_keys(dict, e);
    }
    e->response(ERROR);
}


void Embedis::cmd_SELECT(Embedis* e)
{
    if (e->argc < 2) return e->response(ARGS_ERROR);
    size_t dict = find_dictionary(e->argv[1]);
    if (dict >= dictionaries.size()) {
        return e->response(NOT_FOUND);
    }
    e->current_dict = dict;
    if (dictionaries[dict].custom) {
        if (dictionaries[dict].select) {
            return dictionaries[dict].select(e);
        }
    } else {
        kvs_cleanup(&dictionaries[dict]);
        size_t free_bytes = 0;
        while (kvs_work(&dictionaries[dict], 0, 0, 0, 0, &free_bytes)) {}
        e->stream->print(F("+OK free = "));
        e->stream->println(free_bytes);
        return;
    }
    e->response(OK);
}


short int Embedis::key_to_id(const char* key, size_t length)
{
    for (size_t pos = 0; pos < keys.size(); ++pos) {
        PGM_P p = keys[pos].name;
        size_t i;
        for (i = 0; i < length; i++) {
            unsigned char c = pgm_read_byte(p++);
            if (!c || c != key[i]) break;
        }
        if (i != length) continue;
        if (pgm_read_byte(p)) continue;
        return keys[pos].id;
    }
    return 0;
}


PGM_P Embedis::key_from_id(short int id)
{
    for (size_t pos = 0; pos < keys.size(); ++pos) {
        if (keys[pos].id == id) {
            return keys[pos].name;
        }
    }
    return 0;
}


bool Embedis::kvs_get(Dictionary* dict, const char* key, size_t key_len, String& value)
{
    size_t value_len, value_pos;
    if(kvs_work(dict, key, key_len, 0, &value_len, &value_pos)) {
        if (value.reserve(value_len)) {
            while (value_len) {
                value += (dict->fetch)(value_pos);
                value_len--;
                value_pos++;
            }
            return true;
        }
    }
    return false;
}


bool Embedis::kvs_set(Dictionary* dict,
                      const char* key,
                      size_t key_len,
                      const char* value,
                      size_t value_len)
{
    if (!key_len) return false; // 0 is store end marker
    size_t value_pos, len;
    int is_found;
    // Compute free space
    value_pos = 0;
    while ((kvs_work(dict, 0, 0, 0, 0, &value_pos))) {}
    size_t free_bytes = value_pos;
    is_found = kvs_work(dict, key, key_len, 0, &len, &value_pos);
    // If a key exists, it's value space is available
    // Else account for key and value size storage
    if (is_found) free_bytes += len;
    else free_bytes -= 4;
    // If key is stored-type and doesn't exists, account for its size
    if (!key_to_id(key, key_len)) {
        if (!is_found) free_bytes -= key_len;
    }
    if (free_bytes < value_len) return false;
    kvs_work(dict, key, key_len, value, &value_len, 0);
    return true;
}


bool Embedis::kvs_del(Dictionary* dict, const char* key, size_t key_len)
{
    return kvs_work(dict, key, key_len, 0, 0, 0);
}


void Embedis::kvs_keys(Dictionary* dict, Embedis* e)
{
    size_t pos;
    int len_or_id, i;

    kvs_cleanup(dict);

    // Count the keys
    pos = i = 0;
    while ((kvs_work(dict, 0, 0, 0, 0, &pos))) i++;
    e->response('*', i);

    pos = 0;
    while ((len_or_id = kvs_work(dict, 0, 0, 0, 0, &pos))) {
        if (len_or_id > 0) {
            String s;
            s.reserve(len_or_id);
            for (i = 0; i < len_or_id; i++) {
                s += (dict->fetch)(pos + i);
            }
            e->response(s);
        } else {
            const __FlashStringHelper* k =
                reinterpret_cast<const __FlashStringHelper *>(key_from_id(-len_or_id));
            e->response(k);
        }
    }
}


/*
 Big evil k-v store function. This does many things.

 Ensure is formatted.
 kvs_work("", 0, 0, 0, 0);

 GET a value for given key. Returns 0 when not found.
 kvs_work(key_name, key_len, 0, &value_len, &value_pos);

 SET value to the given key. This will replace any existing key.
 Caller is responsible for ensuring there is enough space.
 kvs_work(key_name, key_len, value, &value_len, 0)

 DEL a key.
 kvs_work(key_name, key_len, 0, 0, 0);

 Count the number of keys in dictionary and calculate free bytes.
 free_bytes = count = 0;
 while ((kvs_work(0, 0, 0, 0, &free_bytes))) count++;

 Iterate over all keys.
 pos = 0;
 while ((len_or_id = kvs_work(0, 0, 0, 0, &pos))) {
 if (len_or_id < 0) .. fetch key_name with key_from_id(-len_or_id);
 else ... key_name can be fetched starting at fetch(pos)
 }
*/
int Embedis::kvs_work(Dictionary* dict,
                      const char* key_name,
                      int key_len,
                      const char* value,
                      size_t* value_len,
                      size_t* value_pos)
{
    size_t pos = dict->size, deloffset = 0, key_pos = 0;
    char on_key = 1, found_key = 0;
    int i, key_id_or_len = 0;

    if (!key_name) {
        if (*value_pos) {
            pos = (*value_pos);
            on_key = 0;
        }
    } else {
        key_id_or_len = -key_to_id(key_name, key_len);
        if (!key_id_or_len) key_id_or_len = key_len;
    }

    while (pos >= 2) {

        if (on_key) {
            key_pos = pos;
            i = (short)(unsigned char)(dict->fetch)(pos-1) + ((dict->fetch)(pos-2) << 8);
            if (i==0) break; // end
            pos -= 2;

            if (deloffset) {
                (dict->store)(pos+deloffset+1, (dict->fetch)(pos+1));
                (dict->store)(pos+deloffset, (dict->fetch)(pos));
            }

            on_key = 0;

            if ((i < 0 && pos < 2) || (i >= 0 && i + 2 > (int)pos)) {
                return kvs_reset(dict, key_name, key_len, value, value_len, value_pos);
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
                    (dict->store)(pos+i+deloffset, (dict->fetch)(pos+i));
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
                if ((dict->fetch)(pos+i) != key_name[i]) {
                    found_key = 0;
                    break;
                }
            }
            continue;
        }

        i = (short)(unsigned char)(dict->fetch)(pos-1) + ((dict->fetch)(pos-2) << 8);

        pos -= 2;
        if (i + 2 > (int)pos) {
            return kvs_reset(dict, key_name, key_len, value, value_len, value_pos);
        }
        pos -= i;

        if (deloffset) {
            i += 2;
            while (i) {
                i--;
                (dict->store)(pos+i+deloffset, (dict->fetch)(pos+i));
            }
        }

        if (found_key) {
            if (value_pos) {
                *value_len = i;
                *value_pos = pos;
                return true;
            }
            deloffset = key_pos - pos;
        }
        on_key = 1;
        found_key = 0;
    }

    if (value) {
        pos += deloffset;

        (dict->store)(pos-1, key_id_or_len & 0xFF);
        (dict->store)(pos-2, (key_id_or_len>>8) & 0xFF);
        pos -= 2;

        i = key_id_or_len;
        while (i > 0) {
            i--;
            pos--;
            (dict->store)(pos, key_name[i]);
        }

        (dict->store)(pos-1, *value_len & 0xFF);
        (dict->store)(pos-2, (*value_len>>8) & 0xFF);
        pos -= 2;

        i = *value_len;
        while (i) {
            i--;
            pos--;
            (dict->store)(pos, value[i]);
        }

        i = 0;
        (dict->store)(pos-1, 0);
        (dict->store)(pos-2, 0);
        if (dict->commit) (dict->commit)();
        return true;
    }

    if (deloffset) {
        (dict->store)(pos+deloffset-1, 0);
        (dict->store)(pos+deloffset-2, 0);
        if (dict->commit) (dict->commit)();
        return true;
    }

    if (!key_name) *value_pos = pos-2;

    return false;
}


// Upon detection of an unformatted or corrupt RAM
// we will erase everything and try kvs_work again.
int Embedis::kvs_reset(Dictionary* dict,
                       const char* key_name,
                       int key_len,
                       const char* value,
                       size_t* value_len,
                       size_t* value_pos)
{
    static char reset_lock = 0;
    int rv;
    if (!reset_lock) {
        reset_lock = 1;
        (dict->store)(dict->size - 1, 0);
        (dict->store)(dict->size - 2, 0);
        rv = kvs_work(dict, key_name, key_len, value, value_len, value_pos);
        reset_lock = 0;
        return rv;
    }
    return 0;
}


// Ensures k-v store is formatted and doesn't contain
// keys that have been removed.
void Embedis::kvs_cleanup(Dictionary* dict)
{
    size_t pos = 0;
    int len_or_id, is_clean = 0;
    // Ensure formatted
    kvs_work(dict, "", 0, 0, 0, 0);
    // Remove bad keys
    while (!is_clean) {
        is_clean = 1;
        while ((len_or_id = kvs_work(dict, 0, 0, 0, 0, &pos))) {
            if (len_or_id < 0) {
                if (!key_from_id(-len_or_id)) {
                    is_clean = 0;
                    kvs_work(dict, "", -len_or_id, 0, 0, 0);
                    break;
                }
            }
        }
    }
}
